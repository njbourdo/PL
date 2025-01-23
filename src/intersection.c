/***************************************************************************************
 * @file    intersection.c
 * @date    January 20th 2025
 *
 * @brief   
 *
 ****************************************************************************************/
#define _POSIX_C_SOURCE 200809L

#include <time.h>

#include "main.h"
#include "intersection.h"
#include "config.h"
#include "lightSet.h"
#include "display.h"

STATIC intState_t intState = IS_off;        //currently active directions of the intersection

STATIC void toggleActiveDirection(uint64_t millis);
STATIC void changeActiveDirection(intState_t state, uint64_t millis);

void INT_init(char* filepath)
{
    CFG_init(filepath);
}

void INT_stateMachine(void)
{   
    uint64_t millis;
    struct timespec ts;
    
    clock_gettime(CLOCK_MONOTONIC, &ts);    //monotonic so it's not affected by time changes
    millis = (((uint64_t)(ts.tv_sec) * 1000) + (ts.tv_nsec / 1000000));

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
    
    DISP_printLightStates();
    
}

STATIC void toggleActiveDirection(uint64_t millis)
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

STATIC void changeActiveDirection(intState_t state, uint64_t millis)
{
    lightSet_t *set1, *set2;
    
    //confirm new state request is valid
    if(state >= IS_off)
    {
        return;
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
        //not possible
        return;
    }
    
    SET_assignLights(set1, set2, millis);
    
    intState = state;
}

