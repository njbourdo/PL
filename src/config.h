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
#include "lightSet.h"


#define INT_DIRECTIONS          4   //number of intersection directions (i.e. max number of light sets)

#define LIGHT_ADV_GRN           {.type = LDT_arrow, .state = LS_red}
#define LIGHT_SOLID_GRN         {.type = LDT_solid, .state = LS_red}
#define LIGHT_UNUSED            {.type = LDT_unused, .state = LS_red}

#define STEP_UNUSED             {.state = LSS_unused, .expirationOffset = 0}
#define STEP_END                {.state = LSS_end, .expirationOffset = -1}
#define STEP_YELLOW             {.state = LSS_LYSY, .expirationOffset = 9000}
#define STEP_RED                {.state = LSS_LRSR, .expirationOffset = 11000}
#define STEP_PROT_GRN           {.state = LSS_LPSR, .expirationOffset = 3000}
#define STEP_PROT_GRN_EXP       {.state = LSS_LYSR, .expirationOffset = 5000}
#define STEP_UNPROT_GRN         {.state = LSS_LUSG, .expirationOffset = 7000}
#define STEP_FLASH_RED          {.state = LSS_LRGR, .expirationOffset = 1000}
#define STEP_FLASH_OFF          {.state = LSS_disable, .expirationOffset = 1000}

#define PATTERN_ADV_GRN         {STEP_PROT_GRN, STEP_PROT_GRN_EXP, STEP_UNPROT_GRN, STEP_YELLOW, STEP_RED, STEP_END, STEP_UNUSED, STEP_UNUSED, STEP_UNUSED, STEP_UNUSED}

#define PATTERN_FLASH_RED       {STEP_FLASH_OFF, STEP_END, STEP_UNUSED, STEP_UNUSED, STEP_UNUSED, STEP_UNUSED, STEP_UNUSED, STEP_UNUSED, STEP_UNUSED, STEP_UNUSED}

#define PATTERN_UNUSED          {STEP_UNUSED, STEP_UNUSED, STEP_UNUSED, STEP_UNUSED, STEP_UNUSED, STEP_UNUSED, STEP_UNUSED, STEP_UNUSED, STEP_UNUSED, STEP_UNUSED}

#define DEFAULT_LIGHT_SET_N     {.lights = {LIGHT_ADV_GRN, LIGHT_SOLID_GRN, LIGHT_UNUSED, LIGHT_UNUSED, LIGHT_UNUSED}, \
                                 .steps = PATTERN_ADV_GRN, \
                                 .currentStep = MAX_STEPS_IN_PATTERN - 1}
#define UNUSED_LIGHT_SET        {.lights = {LIGHT_UNUSED, LIGHT_UNUSED, LIGHT_UNUSED, LIGHT_UNUSED, LIGHT_UNUSED}, \
                                 .steps = PATTERN_UNUSED, \
                                 .currentStep = MAX_STEPS_IN_PATTERN - 1}
                                 
#define DEFAULT_LIGHT_SET_E     DEFAULT_LIGHT_SET_N
#define DEFAULT_LIGHT_SET_S     DEFAULT_LIGHT_SET_N
#define DEFAULT_LIGHT_SET_W     DEFAULT_LIGHT_SET_N

#define DEFAULT_CONFIG          {DEFAULT_LIGHT_SET_N, DEFAULT_LIGHT_SET_E, DEFAULT_LIGHT_SET_S, DEFAULT_LIGHT_SET_W}
#define UNUSED_CONFIG           {UNUSED_LIGHT_SET, UNUSED_LIGHT_SET, UNUSED_LIGHT_SET, UNUSED_LIGHT_SET}


#define CFG_DIR_STR_NORTH       "north"
#define CFG_DIR_STR_EAST        "east"
#define CFG_DIR_STR_WEST        "west"
#define CFG_DIR_STR_SOUTH       "south"

#define CFG_LIGHT_TYPE_SOLID1   'o'
#define CFG_LIGHT_TYPE_SOLID2   'O'
#define CFG_LIGHT_TYPE_SOLID3   '0'
#define CFG_LIGHT_TYPE_ARROW    '<'

#define CFG_STEP_STATE_LPSG     "LPSG"
#define CFG_STEP_STATE_LPSY     "LPSY"
#define CFG_STEP_STATE_LPSR     "LPSR"
#define CFG_STEP_STATE_LUSG     "LUSG"
#define CFG_STEP_STATE_LUSY     "LUSY"
#define CFG_STEP_STATE_LUSR     "LUSR"
#define CFG_STEP_STATE_LYSG     "LYSG"
#define CFG_STEP_STATE_LYSY     "LYSY"
#define CFG_STEP_STATE_LYSR     "LYSR"
#define CFG_STEP_STATE_LRSG     "LRSG"
#define CFG_STEP_STATE_LRSY     "LRSY"
#define CFG_STEP_STATE_LRSR     "LRSR"
#define CFG_STEP_STATE_END      "end"
#define CFG_STEP_STATE_DISABLE  "disable"

//direction heading indices
typedef enum
{
    ID_north = 0,
    ID_east,
    ID_south,
    ID_west,
    ID_numDirections    //last item in list; number of valid options
} intDirection_t;

//********************* Public function prototypes ****************************//
error_t CFG_init(char* filepath);
void CFG_loadDefaults(void);
lightSet_t* CFG_getLightSet(intDirection_t direction);


#endif //_CONFIG_H
