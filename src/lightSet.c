/***************************************************************************************
 * @file    lightSet.c
 * @date    January 20th 2025
 *
 * @brief   
 *
 ****************************************************************************************/
 
#include "main.h"
#include "lightSet.h"

STATIC lightSet_t* lightSet1 = NULL;
STATIC lightSet_t* lightSet2 = NULL;


STATIC lightSetState_t clockLightSetStateMachine(lightSet_t* set, uint64_t millis);
STATIC lightSetState_t incrementLightSetStep(lightSet_t* set);
STATIC lightState_t getArrowState(lightSetState_t setState);
STATIC lightState_t getSolidGreenState(lightSetState_t setState);


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

lightSetState_t SET_stateMachine(uint64_t millis)
{
    lightSetState_t overallState = LSS_end;
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
    
    //printf("%lu vs %lu + %lu\n", millis, set->steps[set->currentStep].expirationOffset, set->cycleStartTime);
    //check if it's time to increment the step in the pattern
    if(millis >= (set->steps[set->currentStep].expirationOffset + set->cycleStartTime))
    {
        //return active state
        return incrementLightSetStep(set);
    }
    
    //return active state
    return set->steps[set->currentStep].state;
}

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

STATIC lightState_t getArrowState(lightSetState_t setState)
{
    lightState_t arrowState = LS_off;
    
    switch(setState)
    {
        case LSS_LPSG:
        case LSS_LPSY:
        case LSS_LPSR:
            arrowState = LS_green;
            break;
        case LSS_LUSG:
        case LSS_LUSY:
        case LSS_LUSR:
            arrowState = LS_yellowArrow;
            break;
        case LSS_LYSG:
        case LSS_LYSY:
        case LSS_LYSR:
            arrowState = LS_yellow;
            break;
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

STATIC lightState_t getSolidGreenState(lightSetState_t setState)
{
    lightState_t solidGreenState = LS_off;
    
    switch(setState)
    {
        case LSS_LPSG:
        case LSS_LUSG:
        case LSS_LYSG:
        case LSS_LRSG:
            solidGreenState = LS_green;
            break;
        case LSS_LPSY:
        case LSS_LUSY:
        case LSS_LYSY:
        case LSS_LRSY:
            solidGreenState = LS_yellow;
            break;
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

