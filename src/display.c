/***************************************************************************************
 * @file    display.c
 * @date    January 20th 2025
 *
 * @brief   Display the current state of lights in an intersection
 *
 ****************************************************************************************/

#include "main.h"
#include "display.h"
#include "config.h"
#include "lightSet.h"

//light colors and states for console
#define COLOR_RESET         "\033[0m"
#define COLOR_GREY          "\033[90m"
#define COLOR_GREEN         "\033[32m"
#define COLOR_YELLOW        "\033[33m"
#define COLOR_RED           "\033[31m"
#define LIGHT_UNUSED_STR    "  "
#define LIGHT_SOLID_STR     "O "
#define LIGHT_ARROW_STR     "<-"

//************************* Local types **************************************//
typedef enum lightid
{
    LID_red = 0,
    LID_yellow,
    LID_green,
    LID_lightIDs
} lightID_t;

//*********************** Static variables ***********************************//
const char* lightStrings[] = {LIGHT_UNUSED_STR, LIGHT_SOLID_STR, LIGHT_ARROW_STR};    //aligned with lightDisplayType_t
const char* lightColors[] = {COLOR_GREEN, COLOR_YELLOW, COLOR_YELLOW, COLOR_RED, COLOR_GREY};    //aligned with lightState_t
STATIC uint8_t printedSetSteps[INT_DIRECTIONS];     //most recently printed light states

//********************* Local function prototypes ****************************//
void printNorthOrSouthLights(lightSet_t* set);
void printWestAndEastLights(lightSet_t* west, lightSet_t* east);
void printLightTypeString(lightID_t LID, lightSet_t* set);

//************************ Public functions *********************************//

 /*****************************************************************************
 ** @brief Display light states
 **     Prints the most recent lights states to the console
 **
 ** @param none
 **
 ** @return none
******************************************************************************/
void DISP_printLightStates(void)
{
    bool printStates = false;

    //check if any state has changed since they were last printed
    for(uint8_t i = 0; i < INT_DIRECTIONS; i++)
    {
        if(printedSetSteps[i] != CFG_getLightSet(i)->currentStep)
        {
            printStates = true;
            
            //update tracking variable
            printedSetSteps[i] = CFG_getLightSet(i)->currentStep;
        }
    }
    
#ifdef TESTING     //disable printing while testing
    printStates = false;
#endif
    
    //no new states to be printed
    if(!printStates)
    {
        return;
    }
    
    printf("\033[2J"); // Clear the screen
    printf("\033[H");  // Move the cursor to the top-left corner

    //print lights visible for vehicles heading North
    if(CFG_getLightSet(ID_north))
    {
        printf("             North: %u\n", CFG_getLightSet(ID_north)->currentStep);
        printNorthOrSouthLights(CFG_getLightSet(ID_north));
    }
    
    //print lights visible for vehicles heading West and East
    if(CFG_getLightSet(ID_west))
    {
        printf("West: %u", CFG_getLightSet(ID_west)->currentStep);
    }
    else
    {
        printf("       ");
    }
    printf("                   ");
    if(CFG_getLightSet(ID_east))
    {
        printf("East: %u", CFG_getLightSet(ID_east)->currentStep);
    }
    printf("\n");
    printWestAndEastLights(CFG_getLightSet(ID_west), CFG_getLightSet(ID_east));
    
    //print lights visible for vehicles heading South
    if(CFG_getLightSet(ID_south))
    {
        printf("             South: %u\n", CFG_getLightSet(ID_south)->currentStep);
        printNorthOrSouthLights(CFG_getLightSet(ID_south));
    }
}

 /*****************************************************************************
 ** @brief Print North or South lights
 **     Prints states of lights visible to vehicles heading north or south
 **
 ** @param set: pointer to set of lights to print
 **
 ** @return none
******************************************************************************/
void printNorthOrSouthLights(lightSet_t* set)
{    
    if(!set)
    {
        return;
    }
    
    for(lightID_t lid = LID_red; lid < LID_lightIDs; lid++)
    {
        printf("             ");
        printLightTypeString(lid, set);
        printf("\n");
    }
    printf("\n");
}

 /*****************************************************************************
 ** @brief Print East and West lights
 **     Prints states of lights visible to vehicles heading east and west
 **
 ** @param set: pointer to set of lights to print
 **
 ** @return none
******************************************************************************/
void printWestAndEastLights(lightSet_t* west, lightSet_t* east)
{    
    for(lightID_t lid = LID_red; lid < LID_lightIDs; lid++)
    {
        printLightTypeString(lid, west);
        printf("           ");
        printLightTypeString(lid, east);
        printf("\n");
    }
    printf("\n");
}

 /*****************************************************************************
 ** @brief Print light type string
 **     Prints a single color of light (red, yellow, or green) for all
 **     configured lights in a set/direction based on their light type (arrow
 **     or solid) and which light is currently active.
 **
 ** @param LID: light color ID
 ** @param set: pointer to set of lights to print
 **
 ** @return none
******************************************************************************/
void printLightTypeString(lightID_t LID, lightSet_t* set)
{
    const char* lstr = NULL;
    const char* cstr = NULL;
    
    for(uint8_t i = 0; i < MAX_LIGHTS_IN_SET; i++)
    {
        //null light set
        if(!set)
        {
            printf("%s ", lightStrings[LDT_unused]);
            continue;
        }
        //unused light set
        if(set->lights[i].type == LDT_unused)
        {
            printf("%s ", lightStrings[LDT_unused]);
            continue;
        }
    
        switch(LID)
        {
            case LID_red:
                if(set->lights[i].state == LS_red)
                {
                    cstr = lightColors[LS_red];
                }
                else
                {
                    cstr = lightColors[LS_off];
                }
                break;
            case LID_yellow:
                if(set->lights[i].state == LS_yellow)
                {
                    cstr = lightColors[LS_yellow];
                }
                else
                {
                    cstr = lightColors[LS_off];
                }
                break;
            case LID_green:
                if(set->lights[i].state == LS_green)
                {
                    cstr = lightColors[LS_green];
                }
                else if(set->lights[i].state == LS_yellowArrow)
                {
                    cstr = lightColors[LS_yellowArrow];
                }
                else
                {
                    cstr = lightColors[LS_off];
                }
                break;
            default:
                cstr = lightStrings[LDT_unused];
                break;
        }
        lstr = lightStrings[set->lights[i].type];
        printf("%s%s %s", cstr, lstr, COLOR_RESET);
    }
}

