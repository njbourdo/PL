/***************************************************************************************
 * @file    main.c
 * @date    January 19th 2025
 *
 * @brief   Main.c for traffic lights application
 *
 ****************************************************************************************/

#include "main.h"

#include "intersection.h"

/*****************************************************************************
 ** @brief main function
 **     Initializes the intersection and clocks its state machine
 **
 ** @param single argument: path to config file
 **
 ** @return 1
******************************************************************************/
int main (int argc, char *argv[])
{
    char* filepath = NULL;
    
    printf("Nick Bourdon's Traffic Light Management Application, v%s\n\n", VERSION);
    
    //check for config file argument
    if(argc >= 2)
    {
        filepath = argv[1];
        printf("Using %s\n", filepath);
    }
    else
    {
        printf("Using default configuration\n");
    }
    
    //initialize config
    INT_init(filepath);
    
    while(1)
    {
        INT_stateMachine();
    }

    return 1;
}

