/***************************************************************************************
 * @file    lightSet.c
 * @date    January 20th 2025
 *
 * @brief   Management of an set of lights (i.e. all light facing one direction)
 *
 ****************************************************************************************/
 
#include "main.h"
#include "lightSet.h"

//*********************** Static variables ***********************************//
STATIC lightSet_t* lightSet1 = NULL;    //ptr to config for active light set 1
STATIC lightSet_t* lightSet2 = NULL;    //ptr to config for active light set 2

//********************* Local function prototypes ****************************//
STATIC lightSetState_t clockLightSetStateMachine(lightSet_t* set, uint64_t millis);
STATIC lightSetState_t incrementLightSetStep(lightSet_t* set);
STATIC lightState_t getArrowState(lightSetState_t setState);
STATIC lightState_t getSolidGreenState(lightSetState_t setState);

//************************ Public functions *********************************//

 /*****************************************************************************
 ** @brief Assign lights
 **     Set active light config pointers to a new set of configs and set the 
 **     start time for the current iteration of light pattern.
 **
 ** @param set1: pointer to active light set config 1
 ** @param set2: pointer to active light set config 2
 ** @param startTime: mS since epoch at which this pattern started
 **
 ** @return error code
******************************************************************************/
error_t SET_assignLights(lightSet_t* set1, lightSet_t* set2, uint64_t startTime)
{
    lightSet1 = set1;
    lightSet2 = set2;
    
    //check if set pointers are valid
    if(!lightSet1 || !lightSet2)
    {
        return ERR_nullPtr;
    }
    
    //set cycle start times for both sets
    lightSet1->cycleStartTime = startTime;
    lightSet2->cycleStartTime = startTime;
    
    return ERR_success;
}

 /*****************************************************************************
 ** @brief Light set state machine
 **     Clocks the state machines for the currently active light set patterns.
 **
 ** @param millis: current mS since epoch
 **
 ** @return lowest illumination state of the active light sets
******************************************************************************/
lightSetState_t SET_stateMachine(uint64_t millis)
{
    lightSetState_t overallState = LSS_end; //lowest illumination state tracker
    lightSetState_t lightSetState;
        
    //clock the state machines for each light set and determine the state with the lowest index
    lightSetState = clockLightSetStateMachine(lightSet1, millis);
    if(lightSetState < overallState)
    {
        overallState = lightSetState;
    }

    lightSetState = clockLightSetStateMachine(lightSet2, millis);
    if(lightSetState < overallState)
    {
        overallState = lightSetState;
    }    
    
    return overallState;
}

//************************* Local functions *********************************//

 /*****************************************************************************
 ** @brief Clock light set state machine
 **     Clock the state machine of an individual light set
 **
 ** @param set: pointer to active light set to clock
 ** @param millis: current mS since epoch
 **
 ** @return current illumination state of the light set
******************************************************************************/
STATIC lightSetState_t clockLightSetStateMachine(lightSet_t* set, uint64_t millis)
{
    //check if set pointer is valid
    if(!set)
    {
        printf("Invaid pointer!\n");
        return LSS_end;
    }
    
    //check if set is being used by checking first step in pattern
    if(set->steps[0].state == LSS_unused)
    {
        //printf("Unused light set\n");
        return LSS_end;
    }
    
    //check if it's time to increment the step in the pattern
    if(millis >= (set->steps[set->currentStep].expirationOffset + set->cycleStartTime))
    {
        //return active state
        return incrementLightSetStep(set);
    }
    
    //return active state
    return set->steps[set->currentStep].state;
}

 /*****************************************************************************
 ** @brief Increment light set step
 **     Increment to the next step of the illumination pattern for a given 
 **     light set.
 **
 ** @param set: pointer to light set to increment
 **
 ** @return current illumination state of the light set
******************************************************************************/
STATIC lightSetState_t incrementLightSetStep(lightSet_t* set)
{
    uint8_t nextStep;
    lightSetState_t nextState;
    lightState_t arrowState;
    lightState_t solidGreenState;
    
    nextStep = (set->currentStep + 1) % MAX_STEPS_IN_PATTERN;
    while(set->steps[nextStep].state == LSS_unused)
    {
        nextStep = (nextStep + 1) % MAX_STEPS_IN_PATTERN;
    }
    nextState = set->steps[nextStep].state;
    arrowState = getArrowState(nextState);
    solidGreenState = getSolidGreenState(nextState);
    
    for(uint8_t i = 0; i < MAX_LIGHTS_IN_SET; i++)
    {        
        if(set->lights[i].type == LDT_solid)
        {
            set->lights[i].state = solidGreenState;
        }
        else if(set->lights[i].type == LDT_arrow)
        {
            set->lights[i].state = arrowState;
        }
        else    //LDT_unused or invalid
        {
            //no more populated lights in the set
            break;
        }
    }
    
    set->currentStep = nextStep;
    //printf("Step %u\n", nextStep);
    
    return nextState;
}

 /*****************************************************************************
 ** @brief Get arrow light state
 **     Gets the arrow state index for a given light set illumination state.
 **
 ** @param setState: illumination state 
 **
 ** @return arrow light state
******************************************************************************/
STATIC lightState_t getArrowState(lightSetState_t setState)
{
    lightState_t arrowState = LS_off;
    
    switch(setState)
    {
        //any protected left state
        case LSS_LPSG:
        case LSS_LPSY:
        case LSS_LPSR:
            arrowState = LS_green;
            break;
        //any unprotected left state
        case LSS_LUSG:
        case LSS_LUSY:
        case LSS_LUSR:
            arrowState = LS_yellowArrow;
            break;
        //any yellow left state
        case LSS_LYSG:
        case LSS_LYSY:
        case LSS_LYSR:
            arrowState = LS_yellow;
            break;
        //any red left state
        case LSS_LRSG:
        case LSS_LRSY:
        case LSS_LRSR:
        case LSS_end:
            arrowState = LS_red;
            break;
        default:
            //arrowState = LS_off;
            break;
    }
    
    return arrowState;
}

 /*****************************************************************************
 ** @brief Get solid light state
 **     Gets the solif light state index for a given light set illumination state.
 **
 ** @param setState: illumination state 
 **
 ** @return arrow light state
******************************************************************************/
STATIC lightState_t getSolidGreenState(lightSetState_t setState)
{
    lightState_t solidGreenState = LS_off;
    
    switch(setState)
    {
        //any solif green state
        case LSS_LPSG:
        case LSS_LUSG:
        case LSS_LYSG:
        case LSS_LRSG:
            solidGreenState = LS_green;
            break;
        //any solid yellow state
        case LSS_LPSY:
        case LSS_LUSY:
        case LSS_LYSY:
        case LSS_LRSY:
            solidGreenState = LS_yellow;
            break;
        //any solid red state
        case LSS_LPSR:
        case LSS_LUSR:
        case LSS_LYSR:
        case LSS_LRSR:
        case LSS_end:
            solidGreenState = LS_red;
            break;
        default:
            //solidGreenState = LS_off;
            break;
    }
    
    return solidGreenState;
}

