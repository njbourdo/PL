/***************************************************************************************
 * @file    intersection.c
 * @date    January 20th 2025
 *
 * @brief   
 *
 ****************************************************************************************/
 
#include "main.h"
#include "intersection.h"
#include "config.h"
#include "lights.h"

static intState_t intState = IS_off;        //currently active directions of the intersection

void INT_init(void)
{
    CFG_init();
}


void INT_stateMachine(uint64_t millis)
{
    static uint64_t lastTime = 0;
    
    
    

    
    switch(intState)
    {
        case IS_ns:
        case IS_ew:
            if(LIGHT_stateMachine(millis) == LS_off)
            {
                toggleActiveDirection();
            }
            break;
        default:
            changeActiveDirection(IS_ns);
            break;
    }
    
}

static void toggleActiveDirection(void)
{
    if(state == IS_ns)
    {
        changeActiveDirection(IS_ew);
    }
    else if(state == IS_ew)
    {
        changeActiveDirection(IS_ns);
    }
}

static void changeActiveDirection(intState_t state)
{
    lightSet_t* set1, set2;
    
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
    }
    else if(state == IS_ew)
    {
        set1 = CFG_getLightSet(ID_east);
        set2 = CFG_getLightSet(ID_west);
    }
    else
    {
        LIGHT_turnAllOff();
        return;
    }
    
    LIGHT_assignLights(set1, set2);
    
    intState = state;
}

