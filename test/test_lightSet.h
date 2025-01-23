/***************************************************************************************
 * @file    lightSet.h
 * @date    January 20th 2025
 *
 * @brief   
 *
 ****************************************************************************************/

#ifndef _LIGHTSET_H_
#define _LIGHTSET_H_

#include "main.h"

#define MAX_LIGHTS_IN_SET       5
#define MAX_STEPS_IN_PATTERN    10

typedef enum lightsetstate
{
    LSS_LPSG = 0,    //left protected, straight green
    LSS_LPSY,        //left protected, straight yellow
    LSS_LPSR,        //left protected, straight red
    LSS_LUSG,        //left unprotected, straight green
    LSS_LUSY,        //left unprotected, straight yellow
    LSS_LUSR,        //left unprotected, straight red
    LSS_LYSG,        //left yellow, straight green
    LSS_LYSY,        //left yellow, straight yellow
    LSS_LYSR,        //left yellow, straight red
    LSS_LRSG,        //left red, straight green
    LSS_LRSY,        //left red, straight yellow
    LSS_LRSR,        //left red, straight red
    LSS_off,         //all lights red with no time restrictions
    LSS_unused       //unused step in a pattern
} lightSetState_t;

typedef enum lightstate
{
    LS_green = 0,
    LS_yellowArrow,
    LS_yellow,
    LS_red,
    LS_numStates
} lightState_t;

typedef enum lightdisplaytype
{
    LDT_unused = 0,         //unused light in set
    LDT_solid,              //Normal, solid green light
    LDT_arrow,              //green light is an arrow
    LDT_numTypes            //last item in list; number of valid options
} lightDisplayType_t;

typedef struct light
{
    lightDisplayType_t type;
    lightState_t state;
} light_t;

typedef struct lightsetstep
{
    lightSetState_t state;
    uint64_t expirationOffset;  //time from cycleStartTime that the state will expire
} lightSetStep_t;

typedef struct lightset
{
    light_t lights[MAX_LIGHTS_IN_SET];    //lights contained in set
    lightSetStep_t steps[MAX_STEPS_IN_PATTERN];     //steps in the set's light pattern
    uint8_t currentStep;        //index of the active step in the pattern
    uint64_t cycleStartTime;    //timestamp of when the current cycle started
} lightSet_t;


void SET_init(void);
void SET_assignLights(lightSet_t* set1, lightSet_t* set2, uint64_t startTime);
void SET_turnAllOff(void);
lightSetState_t SET_stateMachine(uint64_t millis);


#endif //_LIGHTSET_H_
