/***************************************************************************************
 * @file    lights.h
 * @date    January 20th 2025
 *
 * @brief   
 *
 ****************************************************************************************/

#ifndef _LIGHTS_H_
#define _LIGHTS_H_

#include "main.h"

#define MAX_LIGHTS_IN_SET       5
#define ACTIVE_SETS             2   //number of active sets (directions) at a time

typedef enum
{
    LT_unused = 0,          //entire light set unpopulated
    LT_advancedLeft,        //green light is a left arrow and is enabled BEFORE opposite side's normal green
    LT_normalGreen,         //green light is solid 
    LT_lateLeft,            //green light is a left arrow and is enabled AFTER opposite side's normal green
    LT_numTypes             //last item in list; number of valid options
} lightTiming_t;

typedef enum
{
    LDT_unused = 0,         //unused light in set
    LDT_solid,              //Normal, solid green light
    LDT_arrowStrict,        //green light is an arrow; left turns restricted to green arrow
    LDT_arrowYellow,        //green and yellow lights are arrows; left turns protected on green but still allowed on yellow
    LDT_numTypes            //last item in list; number of valid options
} lightDisplayType_t;

typedef enum
{
    LS_advancedLeft = 0,
    LS_tempYellow,
    LS_tempRed,
    LS_green,
    LS_lateLeft,
    LS_yellow,
    LS_red,
    LS_off             //all lights are red with no time restrictions
} lightState_t;

typedef struct lightdelays
{
    uint32_t advancedLeft;
    uint32_t green;
    uint32_t lateLeft;
    uint32_t yellow;
    uint32_t red;
} lightDelays_t;

typedef struct lightset
{
    uint8_t numLights;          //number of lights in the set (1-MAX_LIGHTS_IN_SET)
    lightTiming_t setTiming;    //timing used for the entire light set
    lightDisplayType_t types[MAX_LIGHTS_IN_SET];    //configuration of each light in set (left to right)
    lightState_t lightState;    //current state of the light set
    uint64_t stateExpiry;       //time at which the current state expires
    lightDelays_t delays;       //struct of delay times for this set
} lightSet_t;






void LIGHT_init(void);
void LIGHT_assignLights(void);
void LIGHT_turnAllOff(void);
lightState_t LIGHT_stateMachine(uint64_t millis);


#endif //_LIGHTS_H_
