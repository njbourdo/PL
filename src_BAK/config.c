/***************************************************************************************
 * @file    config.c
 * @date    January 20th 2025
 *
 * @brief   
 *
 ****************************************************************************************/

#include "config.h"


static lightSet_t lightConfigs[INT_DIRECTIONS] = DEFAULT_CONFIG;
 
void CFG_init(void)
{
    //read config file and overwrite lightConfigs
}

lightSet_t* CFG_getLightSet(intDirection_t direction)
{
    if(direction > ID_west)
    {
        return NULL;
    }
    
    return &lightConfigs[direction];
}
