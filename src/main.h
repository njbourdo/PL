/***************************************************************************************
 * @file    main.h
 * @date    January 20th 2025
 *
 * @brief   
 *
 ****************************************************************************************/

#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef TESTING
#define STATIC  
#else
#define STATIC  static
#endif

typedef enum error
{
    ERR_success = 0,
    ERR_file,
    ERR_format,
    ERR_json,
} error_t;



#endif //_MAIN_H_
