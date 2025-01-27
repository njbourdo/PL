/***************************************************************************************
 * @file    intersection.c
 * @date    January 20th 2025
 *
 * @brief   Intersection management
 *
 ****************************************************************************************/
#define _POSIX_C_SOURCE 200809L     //necessary for CLOCK_MONOTONIC

#include <time.h>
#include <string.h>

#include "main.h"
#include "intersection.h"
#include "config.h"
#include "lightSet.h"
#include "display.h"

//*********************** Static variables ***********************************//
STATIC intState_t intState = IS_off;        //currently active directions of the intersection
STATIC const lightSetStep_t errorSteps[MAX_STEPS_IN_PATTERN] = PATTERN_FLASH_RED;    //error pattern

//********************* Local function prototypes ****************************//
STATIC uint64_t getMillis(void);
STATIC error_t toggleActiveDirection(uint64_t millis);
STATIC error_t changeActiveDirection(intState_t state, uint64_t millis);

//************************* Function pointers ********************************//
STATIC error_t (*changeActiveDirection_ptr)(intState_t, uint64_t) = changeActiveDirection;  //function ptr for mocking
STATIC lightSet_t* (*CFG_getLightSet_ptr)(intDirection_t) = CFG_getLightSet;                //function ptr for mocking

//************************ Public functions *********************************//

 /*****************************************************************************
 ** @brief Intersection initialization
 **     Simply initializes the intersection configuration
 **
 ** @param filepath: path to config file
 **
 ** @return error code
******************************************************************************/
error_t INT_init(char* filepath)
{
    return CFG_init(filepath);
}

 /*****************************************************************************
 ** @brief Intersection state machine
 **     Clocks the intersection state machine, initializing to North-South,
 **     then switching between that and East-West when each direction's pattern
 **     has reached its end state.
 **
 ** @param none
 **
 ** @return none
******************************************************************************/
void INT_stateMachine(void)
{   
    uint64_t millis = getMillis();   

    switch(intState)
    {
        case IS_ns:
        case IS_ew:
            if(SET_stateMachine(millis) == LSS_end)
            {
                if(toggleActiveDirection(millis) != ERR_success)
                {
                    changeActiveDirection(IS_error, millis);
                }
            }
            break;
        default:
            if(changeActiveDirection_ptr(IS_ns, millis) != ERR_success)
            {
                changeActiveDirection(IS_error, millis);
            }
            break;
    }
    
    DISP_printLightStates();
}

//************************* Local functions *********************************//

 /*****************************************************************************
 ** @brief Get milliseconds
 **     Get the current number of milliseconds since the epoch. CLOCK_MONOTONIC
 **     is used so the value does not jump during time changes.
 **
 ** @param none
 **
 ** @return mS since epoch
******************************************************************************/
STATIC uint64_t getMillis(void)
{
    struct timespec ts;
    
    clock_gettime(CLOCK_MONOTONIC, &ts);    //monotonic so it's not affected by time changes
    return (((uint64_t)(ts.tv_sec) * 1000) + (ts.tv_nsec / 1000000));
}

 /*****************************************************************************
 ** @brief Toggle active direction
 **     Switch from North-South to East-West or vice versa. The active
 **     direction is the one whose lights are moving through their configured 
 **     pattern(s).
 **
 ** @param millis: current mS since epoch
 **
 ** @return error code
******************************************************************************/
STATIC error_t toggleActiveDirection(uint64_t millis)
{
    error_t result = ERR_success;
    
    if(intState == IS_ns)
    {
        result = changeActiveDirection_ptr(IS_ew, millis);
    }
    else if(intState == IS_ew)
    {
        result = changeActiveDirection_ptr(IS_ns, millis);
    }
    
    return result;
}

 /*****************************************************************************
 ** @brief Change active direction
 **     Switch the active direction to the specified direction. The active
 **     direction is the one whose lights are moving through their configured 
 **     pattern(s).
 **
 ** @param state: requested direction to activate
 ** @param millis: current mS since epoch
 **
 ** @return error code
******************************************************************************/
STATIC error_t changeActiveDirection(intState_t state, uint64_t millis)
{
    error_t result;
    lightSet_t *set1, *set2;
    
    //confirm new state request is valid
    if(state >= IS_off)
    {
        printf("Invalid intersection state request: %u\n", state);
        return ERR_value;
    }
    
    //check if a new state is requested
    if(state == intState)
    {
        //already in this state; nothing to do
        return ERR_success;
    }
    
    if(state == IS_ns)
    {
        set1 = CFG_getLightSet_ptr(ID_north);
        set2 = CFG_getLightSet_ptr(ID_south);
        //printf("North-south\n");
    }
    else if(state == IS_ew)
    {
        set1 = CFG_getLightSet_ptr(ID_east);
        set2 = CFG_getLightSet_ptr(ID_west);
        //printf("East-west\n");
    }
    else
    {
        //error happened, switch to error pattern to simulate hardware taking over to flash red lights
        printf("Changing to flashing red pattern!\n");
        set1 = CFG_getLightSet_ptr(ID_north);
        set2 = CFG_getLightSet_ptr(ID_south);
        memcpy(set1->steps, errorSteps, sizeof(errorSteps));
        memcpy(set2->steps, errorSteps, sizeof(errorSteps));
        set1 = CFG_getLightSet_ptr(ID_east);
        set2 = CFG_getLightSet_ptr(ID_west);
        memcpy(set1->steps, errorSteps, sizeof(errorSteps));
        memcpy(set2->steps, errorSteps, sizeof(errorSteps));
        //return ERR_success;
        state = IS_ew;
    }
    
    //set new active configurations in lightSet module
    result = SET_assignLights(set1, set2, millis);
    if(result != ERR_success)
    {
        return result;
    }
    
    intState = state;
    
    return ERR_success;
}

