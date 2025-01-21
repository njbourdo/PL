/***************************************************************************************
 * @file    lights.c
 * @date    January 20th 2025
 *
 * @brief   
 *
 ****************************************************************************************/
 
#include "lights.h"

//static lightSet_t* lightSets[ACTIVE_SETS];  //pointers to active light sets
static lightSet_t* lightSet1 = NULL;
static lightSet_t* lightSet2 = NULL;
static lightState_t overallLightState;      //overall state of the currently active light sets

void LIGHT_init(void)
{
    /*for(uint8_t i = 0; i < ACTIVE_SETS; i++)
    {
        lightSets[i] = NULL;
    }*/
}

/*void LIGHT_assignLights(lightSet_t* sets[], uint8_t len)
{
    for(uint8_t i = 0; (i < ACTIVE_SETS) && (i < len); i++)
    {
        lightSets[i] = sets[i];

        if(lightSets[i])
        {
            //reset expiration so it exits off state immediately
            lightSets[i]->stateExpiry = 0;
        }
    }
}*/
void LIGHT_assignLights(lightSet_t* set1, lightSet_t* set2)
{
    lightSet1 = set1;
    lightSet2 = set2;
    
    //check if set pointers are valid
    if(!lightSet1 || !lightSet2)
    {
        //TODO: support error codes
        return;
    }
    
    //reset expiration so it exits off state immediately
    lightSet1->stateExpiry = 0;
    lightSet2->stateExpiry = 0;
}

void LIGHT_turnAllOff(void)
{
    //start transition to all reds if necessary, otherwise transition to LS_off
}

lightState_t LIGHT_stateMachine(uint64_t millis)
{
    //check if set pointers are valid
    if(!lightSet1 || !lightSet2)
    {
        return LS_off;
    }
    //check if set is being used
    /*if(lightSets[i]->setTiming == LT_unused)
    {
        continue;
    }*/
    
    //check if it's time to switch states for each set
    if(millis >= lightSet1->stateExpiry)
    {
        incrementLightState(lightSet1);
    }
    if(millis >= lightSet2->stateExpiry)
    {
        incrementLightState(lightSet2);
    }
        
        //clock state machine
        /*switch(currentLightState)
        {
            case LS_advancedLeft:
                switchLightStates(LS_green);
                break;
            case LS_green:
                switchLightStates(LS_lateLeft);
                break;
            case LS_lateLeft:
                switchLightStates(LS_yellow);
                break;
            case LS_yellow:
                switchLightStates(LS_red);
                break;
            case LS_red:
                switchLightStates(LS_off);
                break;
            case LS_off:
                switchLightStates(LS_advancedLeft);
                break;
            default:
                break;
        }*/
        //incrementLightState(lightSets[i]);
    }
    
    
    
    
    return 
}

//static void switchLightStates(lightState_t state)
static void incrementSetLightState(lightSet_t set)
{
    switch(state)
    {
        case LS_advancedLeft:
            if(lightSet1->setTiming == LT_advancedLeft)
            {
                
            }
            break;
        case LS_green:
            switchLightStates(LS_lateLeft);
            break;
        case LS_lateLeft:
            switchLightStates(LS_yellow);
            break;
        case LS_yellow:
            switchLightStates(LS_red);
            break;
        case LS_red:
            switchLightStates(LS_off);
            break;
        case LS_off:
            if(isTimingPatternUsed(LT_advancedLeft))
            {
                changeLightState(LS_advancedLeft, set);
            }
            else
            {
                changeLightState(LS_green, set);
            }
            switchLightStates(LS_advancedLeft);
            break;
        default:
            break;
    }
    
    currentLightState = state;
}

static void changeLightState(lightState_t state, lightSet_t set)
{
    switch(state)
    {
        case LS_advancedLeft:
            if(set->setTiming
            break;
        case LS_green:
            switchLightStates(LS_lateLeft);
            break;
        case LS_lateLeft:
            switchLightStates(LS_yellow);
            break;
        case LS_yellow:
            switchLightStates(LS_red);
            break;
        case LS_red:
            switchLightStates(LS_off);
            break;
        case LS_off:
            switchLightStates(LS_advancedLeft);
            break;
        default:
            break;
    }*/
}

static uint64_t getLatestExpiry(void)
{
    uint64_t expiry = 0;
    
    for(uint8_t i = 0; i < ACTIVE_SETS; i++)
    {
        //check if set pointer is valid
        if(!lightSets[i])
        {
            continue;
        }
        //check if set is being used
        if(lightSets[i]->setTiming == LT_unused)
        {
            continue;
        }
        
        if(lightSets[i]->stateExpiry > expiry)
        {
            expiry = lightSets[i]->stateExpiry;
        }
    }
    
    return expiry;
}

static bool isTimingPatternUsed(lightTiming_t timingPattern)
{    
    if((lightSet1->setTiming == timingPattern || (lightSet2->setTiming == timingPattern))
    {
        return TRUE;
    }
    
    return FALSE;
}

