/***************************************************************************************
 * @file    config.h
 * @date    January 20th 2025
 *
 * @brief   Defines the configuration for a set of lights
 *
 ****************************************************************************************/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "main.h"
#include "lights.h"


#define INT_DIRECTIONS          4   //number of intersection directions (i.e. max number of light sets)

#define DEFAULT_LIGHT_SET_N     {.numLights = 2, \
                                 .types = {LT_advancedLeft, LT_normalGreen, LT_unused, LT_unused, LT_unused}, \
                                 .lightState = LS_off }
                                 
#define DEFAULT_LIGHT_SET_E     DEFAULT_LIGHT_SET_N
#define DEFAULT_LIGHT_SET_S     DEFAULT_LIGHT_SET_N
#define DEFAULT_LIGHT_SET_W     DEFAULT_LIGHT_SET_N

#define DEFAULT_CONFIG          {DEFAULT_LIGHT_SET_N, DEFAULT_LIGHT_SET_E, DEFAULT_LIGHT_SET_S, DEFAULT_LIGHT_SET_W}

typedef enum
{
    ID_north = 0,
    ID_east,
    ID_south,
    ID_west
} intDirection_t;


//TODO: support error codes
void CFG_init(void);
lightSet_t* CFG_getLightSet(intDirection_t direction);


#endif //_CONFIG_H
