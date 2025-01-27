/***************************************************************************************
 * @file    intersection.h
 * @date    January 20th 2025
 *
 * @brief   Intersection management header
 *
 ****************************************************************************************/

#ifndef _INTERSECTION_H_
#define _INTERSECTION_H_

#include "main.h"


//active heading index
typedef enum
{
    IS_ns = 0,      //North-South active
    IS_ew,          //East-West active
    IS_error,       //flashing red all directions
    IS_off          //All off (red)
} intState_t;

//********************* Public function prototypes ****************************//

error_t INT_init(char* filepath);
void INT_stateMachine(void);


#endif //_INTERSECTION_H_
