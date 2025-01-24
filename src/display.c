/***************************************************************************************
 * @file    config.c
 * @date    January 20th 2025
 *
 * @brief   
 *
 ****************************************************************************************/

#include "main.h"
#include "display.h"
#include "config.h"
#include "lightSet.h"

#define COLOR_RESET         "\033[0m"
#define COLOR_GREY          "\033[90m"
#define COLOR_GREEN         "\033[32m"
#define COLOR_YELLOW        "\033[33m"
#define COLOR_RED           "\033[31m"
#define LIGHT_UNUSED_STR    "  "
#define LIGHT_SOLID_STR     "O "
#define LIGHT_ARROW_STR     "<-"
const char* lightStrings[] = {LIGHT_UNUSED_STR, LIGHT_SOLID_STR, LIGHT_ARROW_STR};    //aligned with lightDisplayType_t
const char* lightColors[] = {COLOR_GREEN, COLOR_YELLOW, COLOR_YELLOW, COLOR_RED, COLOR_GREY};    //aligned with lightState_t
STATIC uint8_t printedSetSteps[INT_DIRECTIONS];

void printNorthOrSouthLights(lightSet_t* set);

void DISP_printLightStates(void)
{
    bool printStates = false;

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
    
    if(!printStates)
    {
        return;
    }
    
    printf("\033[2J"); // Clear the screen
    printf("\033[H");  // Move the cursor to the top-left corner

    //print lights visible for vehicles heading North
    printf("North: %u\n", CFG_getLightSet(ID_north)->currentStep);
    printNorthOrSouthLights(CFG_getLightSet(ID_north));
    
    //print lights visible for vehicles heading West and East
    //printWestAndEastLights(&lightConfigs[ID_west], &lightConfigs[ID_east]);
    printf("East: %u\n", CFG_getLightSet(ID_east)->currentStep);
    printNorthOrSouthLights(CFG_getLightSet(ID_east));
    printf("West: %u\n", CFG_getLightSet(ID_west)->currentStep);
    printNorthOrSouthLights(CFG_getLightSet(ID_west));
    
    //print lights visible for vehicles heading South
    //printNorthOrSouthLights(&lightConfigs[ID_south]);
    printf("South: %u\n", CFG_getLightSet(ID_south)->currentStep);
    printNorthOrSouthLights(CFG_getLightSet(ID_south));
    
    //printf("\033[1;32mThis text is bright green.\033[0m\n");

}



void printNorthOrSouthLights(lightSet_t* set)
{
    const char* lstr = NULL;
    const char* cstr = NULL;
    
    for(uint8_t j = 0; j < 3; j++)
    {
        for(uint8_t i = 0; i < MAX_LIGHTS_IN_SET; i++)
        {
            if(set->lights[i].type == LDT_unused)
            {
                printf("%s ", lightStrings[LDT_unused]);
                continue;
            }
            
            if(j == 0) //red light
            {
                if(set->lights[i].state == LS_red)
                {
                    cstr = lightColors[LS_red];
                }
                else
                {
                    cstr = lightColors[LS_off];
                }
                
                lstr = lightStrings[LDT_solid];
            }
            else if(j == 1) //yellow light
            {
                if(set->lights[i].state == LS_yellow)
                {
                    cstr = lightColors[LS_yellow];
                }
                else
                {
                    cstr = lightColors[LS_off];
                }
                
                lstr = lightStrings[LDT_solid];
            }
            else if(j == 2) //green/arrow light
            {
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
                
                lstr = lightStrings[set->lights[i].type];
            }
            printf("%s%s %s", cstr, lstr, COLOR_RESET);
        }
        printf("\n");
    }
    printf("\n");
}

