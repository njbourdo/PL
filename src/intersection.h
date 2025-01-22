/***************************************************************************************
 * @file    intersection.h
 * @date    January 20th 2025
 *
 * @brief   
 *
 ****************************************************************************************/

#ifndef _INTERSECTION_H_
#define _INTERSECTION_H_

#include "main.h"



typedef enum
{
    IS_ns = 0,      //North-South active
    IS_ew,          //East-West active
    IS_off          //All off (red)
} intState_t;





void INT_init(char* filepath);
void INT_stateMachine(uint64_t millis);


#endif //_INTERSECTION_H_
