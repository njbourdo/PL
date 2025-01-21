/***************************************************************************************
 * @file    main.c
 * @date    January 19th 2025
 *
 * @brief   Main.c for traffic lights application
 *
 ****************************************************************************************/
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <time.h>

#include "main.h"getconf

#include "config.h"
#include "lights.h"

/*****************************************************************************
 ** @brief main function
 **     Initialized the intersection and clocks its state machine
 **
 ** @param none
 **
 ** @return 1
******************************************************************************/
int main ()
{
    uint64_t currentMillis = 0;     //current milliseconds since epoch
    
    printf("Hello world!\n");
    
    INT_init();
    
    while(1)
    {
        currentMillis = get_time_millis();
        INT_stateMachine(currentMillis);
        //usleep?
    }

    return 1;
}


uint64_t get_time_millis() 
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);    //monotonic so it's not affected by time changes
    return (uint64_t)(ts.tv_sec) * 1000 + (ts.tv_nsec / 1000000);
}
