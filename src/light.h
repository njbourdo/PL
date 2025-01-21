/***************************************************************************************
 * @file    light.h
 * @date    January 20th 2025
 *
 * @brief   
 *
 ****************************************************************************************/

#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "main.h"

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

//void LIGHT_setState(light_t light, lightState_t state);


#endif //_LIGHTS_H_
