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

#include "main.h"

#include "intersection.h"
#include "config.h"
#include "lightSet.h"

uint64_t get_time_millis(void);

/*****************************************************************************
 ** @brief main function
 **     Initialized the intersection and clocks its state machine
 **
 ** @param none
 **
 ** @return 1
******************************************************************************/
int main (int argc, char *argv[])
{
    char* filepath = NULL;
    uint64_t currentMillis = 0;     //current milliseconds since epoch
    
    if(argc >= 2)
    {
        filepath = argv[1];
        printf("Using %s\n", filepath);
    }
    else
    {
        printf("Using default configuration\n");
    }
    
    INT_init(filepath);
    
    while(1)
    {
        currentMillis = get_time_millis();
        INT_stateMachine(currentMillis);
        //usleep?
    }

    return 1;
}


uint64_t get_time_millis(void) 
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);    //monotonic so it's not affected by time changes
    return (((uint64_t)(ts.tv_sec) * 1000) + (ts.tv_nsec / 1000000));
}
