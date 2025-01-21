/***************************************************************************************
 * @file    config.c
 * @date    January 20th 2025
 *
 * @brief   
 *
 ****************************************************************************************/

#include "config.h"


static lightSet_t lightConfigs[INT_DIRECTIONS] = DEFAULT_CONFIG;
static uint8_t printedSetSteps[INT_DIRECTIONS];

void printNorthOrSouthLights(lightSet_t* set);
 
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

void CFG_printLightStates(void)
{
    bool printStates = false;
    for(uint8_t i = 0; i < INT_DIRECTIONS; i++)
    {
        if(printedSetSteps[i] != lightConfigs[i].currentStep)
        {
            printStates = true;
            
            //update tracking variable
            printedSetSteps[i] = lightConfigs[i].currentStep;
        }
    }
    
    if(!printStates)
    {
        return;
    }
    
//    return;
    
    printf("\033[2J"); // Clear the screen
    printf("\033[H");  // Move the cursor to the top-left corner

    //print lights visible for vehicles heading North
    printf("North: %u\n", lightConfigs[ID_north].currentStep);
    printNorthOrSouthLights(&lightConfigs[ID_north]);
    
    //print lights visible for vehicles heading West and East
    //printWestAndEastLights(&lightConfigs[ID_west], &lightConfigs[ID_east]);
    printf("East: %u\n", lightConfigs[ID_east].currentStep);
    printNorthOrSouthLights(&lightConfigs[ID_east]);
    printf("West: %u\n", lightConfigs[ID_west].currentStep);
    printNorthOrSouthLights(&lightConfigs[ID_west]);
    
    //print lights visible for vehicles heading South
    //printNorthOrSouthLights(&lightConfigs[ID_south]);
    printf("South: %u\n", lightConfigs[ID_south].currentStep);
    printNorthOrSouthLights(&lightConfigs[ID_south]);
    
    //printf("\033[1;32mThis text is bright green.\033[0m\n");

}


#define COLOR_RESET         "\033[0m"
#define COLOR_GREY          "\033[90m"
const char* lightStrings[] = {"  ", "O ", "<-"};    //aligned with lightDisplayType_t
const char* lightColors[] = {"\033[32m", "\033[33m", "\033[33m", "\033[31m", COLOR_GREY};    //aligned with lightState_t
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
                    cstr = lightColors[LS_numStates];   //used for grey
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
                    cstr = lightColors[LS_numStates];   //used for grey
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
                    cstr = lightColors[LS_numStates];   //used for grey
                    
                }
                
                lstr = lightStrings[set->lights[i].type];
            }
            printf("%s%s %s", cstr, lstr, COLOR_RESET);
        }
        printf("\n");
    }
    printf("\n");
}
