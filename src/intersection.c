/***************************************************************************************
 * @file    intersection.c
 * @date    January 20th 2025
 *
 * @brief   
 *
 ****************************************************************************************/

#include <stdlib.h>

#include "main.h"
#include "intersection.h"
#include "config.h"
#include "lightSet.h"

static intState_t intState = IS_off;        //currently active directions of the intersection

static void toggleActiveDirection(uint64_t millis);
static void changeActiveDirection(intState_t state, uint64_t millis);

void INT_init(char* filepath)
{
    CFG_init(filepath);
}


void INT_stateMachine(uint64_t millis)
{    
    switch(intState)
    {
        case IS_ns:
        case IS_ew:
            if(SET_stateMachine(millis) == LSS_off)
            {
                toggleActiveDirection(millis);
            }
            break;
        default:
            changeActiveDirection(IS_ns, millis);
            break;
    }
    
    CFG_printLightStates();
    
}

static void toggleActiveDirection(uint64_t millis)
{
    if(intState == IS_ns)
    {
        changeActiveDirection(IS_ew, millis);
    }
    else if(intState == IS_ew)
    {
        changeActiveDirection(IS_ns, millis);
    }
}

static void changeActiveDirection(intState_t state, uint64_t millis)
{
    lightSet_t *set1, *set2;
    
    //confirm new state request is valid
    if(state > IS_off)
    {
        state = IS_off;
    }
    
    //check if a new state is requested
    if(state == intState)
    {
        return;
    }
    
    if(state == IS_ns)
    {
        set1 = CFG_getLightSet(ID_north);
        set2 = CFG_getLightSet(ID_south);
        //printf("North-south\n");
    }
    else if(state == IS_ew)
    {
        set1 = CFG_getLightSet(ID_east);
        set2 = CFG_getLightSet(ID_west);
        //printf("East-west\n");
    }
    else
    {
        SET_turnAllOff();
        return;
    }
    
    SET_assignLights(set1, set2, millis);
    
    intState = state;
}

