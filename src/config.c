/***************************************************************************************
 * @file    config.c
 * @date    January 20th 2025
 *
 * @brief   
 *
 ****************************************************************************************/

#include <strings.h>
#include <stdlib.h>

#include "main.h"
#include "config.h"
#include "cJSON/cJSON.h"


static lightSet_t lightConfigs[INT_DIRECTIONS] = DEFAULT_CONFIG;
static uint8_t printedSetSteps[INT_DIRECTIONS];

void printNorthOrSouthLights(lightSet_t* set);
error_t parseConfig(const char* json);
intDirection_t getDirectionIdxFromString(char* dir);
lightDisplayType_t getLightTypeFromString(char* type);
lightSetState_t getStepStateFromString(char* state);
 
void CFG_init(char* filepath)
{
    FILE* file;
    long fileSize;
    char* json;
    size_t readBytes;
    
    //open file
    file = fopen(filepath, "r");
    if(!file)
    {
        printf("Failed to open file, using default values\n");
        return;
    }

    //determine file size
    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    rewind(file);

    //malloc space for file contents
    json = (char *)malloc(fileSize + 1);  // +1 for null terminator
    if(!json)
    {
        printf("Failed to allocate memory for JSON content, using default values\n");
        fclose(file);
        return;
    }

    //read and null terminate the resulting string, just in case
    readBytes = fread(json, 1, fileSize, file);
    if((long)readBytes != fileSize)
    {
        printf("Failed to read all bytes from file (%lu of %li), using default values\n", readBytes, fileSize);
        fclose(file);
        free(json);
        return;
    }
    json[fileSize] = '\0';

    fclose(file);
    
    if(parseConfig(json) != ERR_success)
    {
        printf("Failed to load config, using default values\n");
    }
    
    free(json);
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

error_t parseConfig(const char* json)
{
    cJSON* root;
    const cJSON* intersection = NULL;
    const cJSON* direction = NULL;
    const cJSON* lights = NULL;
    const cJSON* light = NULL;
    const cJSON* steps = NULL;
    const cJSON* step = NULL;
    const cJSON* value = NULL;
    intDirection_t directionIdx;
    uint8_t lightIdx;
    lightDisplayType_t lightType;
    uint8_t stepIdx;
    lightSetState_t stepState;
    uint64_t lastTime;
    
    //convert JSON string to cJSON object
    root = cJSON_Parse(json);
    if (!root) 
    {
        printf("Failed to parse JSON config: %s\n", cJSON_GetErrorPtr());
        return ERR_json;
    }
    
    //get intersection object
    intersection = cJSON_GetObjectItem(root, "intersection");
    
    //for each direction in intersection...
    cJSON_ArrayForEach(direction, intersection)
    {
        //get direction
        value = cJSON_GetObjectItem(direction, "direction");
        if(!cJSON_IsString(value))
        {
            printf("Direction value not a string! Skipping...\n");
            continue;
        }
        directionIdx = getDirectionIdxFromString(value->valuestring);
        if(directionIdx >= ID_numDirections)
        {
            printf("Invalid direction string: %s. Skipping...\n", value->valuestring);
            continue;
        }
        printf("\n%s\n", value->valuestring);
        
        //get lights array
        lights = cJSON_GetObjectItem(direction, "lights");
        
        //for each light...
        lightIdx = 0;
        cJSON_ArrayForEach(light, lights)
        {
            if(lightIdx >= MAX_LIGHTS_IN_SET)
            {
                printf("Logic only supports %u lights per set, skipping remainder\n", MAX_LIGHTS_IN_SET);
                break;
            }
            if(!cJSON_IsString(light))
            {
                printf("Light type value not a string! Skipping...\n");
                lightIdx++;
                continue;
            }
            
            lightType = getLightTypeFromString(light->valuestring);
            lightConfigs[directionIdx].lights[lightIdx].type = lightType;
            lightConfigs[directionIdx].lights[lightIdx].state = LS_red;
            
            lightIdx++;
        }
        
        //get steps array
        steps = cJSON_GetObjectItem(direction, "steps");
        
        //for each step...
        stepIdx = 0;
        lastTime = 0;
        cJSON_ArrayForEach(step, steps)
        {
            if(stepIdx >= MAX_STEPS_IN_PATTERN)
            {
                printf("Logic only supports %u steps per pattern; disabling pattern\n", MAX_STEPS_IN_PATTERN);
                lightConfigs[directionIdx].steps[0].state = LSS_unused;
                break;
            }
            
            //get state
            value = cJSON_GetObjectItem(step, "state");
            if(!cJSON_IsString(value))
            {
                printf("Step state value not a string! Disabling pattern\n");
                lightConfigs[directionIdx].steps[0].state = LSS_unused;
                break;
            }
            stepState = getStepStateFromString(value->valuestring);
            if(stepState >= LSS_unused)
            {
                printf("Invalid step state string: %s. Disabling pattern\n", value->valuestring);
                lightConfigs[directionIdx].steps[0].state = LSS_unused;
                break;
            }
            printf("%s\n", value->valuestring);
            
            //get time
            value = cJSON_GetObjectItem(step, "time");
            if(!cJSON_IsNumber(value))
            {
                printf("Step time value not a number! Disabling pattern\n");
                lightConfigs[directionIdx].steps[0].state = LSS_unused;
                break;
            }
            
            //assign step state and time values
            lightConfigs[directionIdx].steps[stepIdx].state = stepState;
            //users enter state start times but config expects state end time, so set first to 0, last to never expire, and the rest to the time of the previous step
            if(stepIdx == 0)    //first step
            {
                lightConfigs[directionIdx].steps[stepIdx].expirationOffset = 0;
            }
            else if(stepState == LSS_off) //last step
            {
                lightConfigs[directionIdx].steps[stepIdx - 1].expirationOffset = (uint64_t)value->valueint;
                lightConfigs[directionIdx].steps[stepIdx].expirationOffset = (uint64_t)-1;
            }
            else    //every step in between
            {
                lightConfigs[directionIdx].steps[stepIdx - 1].expirationOffset = lastTime;
            }
            
            lastTime = (uint64_t)value->valueint;
            
            stepIdx++;
        }
    }
    
    //free memory for cJSON object
    cJSON_Delete(root);
    
    return ERR_success;
}

intDirection_t getDirectionIdxFromString(char* dir)
{
    if(strcasecmp(CFG_DIR_STR_NORTH, dir))
    {
        return ID_north;
    }
    else if(strcasecmp(CFG_DIR_STR_EAST, dir))
    {
        return ID_east;
    }
    else if(strcasecmp(CFG_DIR_STR_SOUTH, dir))
    {
        return ID_south;
    }
    else if(strcasecmp(CFG_DIR_STR_WEST, dir))
    {
        return ID_west;
    }
    return ID_numDirections;
}

lightDisplayType_t getLightTypeFromString(char* type)
{
    switch(type[0])
    {
        case CFG_LIGHT_TYPE_SOLID1:
        case CFG_LIGHT_TYPE_SOLID2:
        case CFG_LIGHT_TYPE_SOLID3:
            return LDT_solid;
        case CFG_LIGHT_TYPE_ARROW:
            return LDT_arrow;
        default:
            break;
    }
    
    return LDT_unused;
}

lightSetState_t getStepStateFromString(char* state)
{
    if(strcasecmp(CFG_STEP_STATE_LPSG, state))
    {
        return LSS_LPSG;
    }
    else if(strcasecmp(CFG_STEP_STATE_LPSY, state))
    {
        return LSS_LPSY;
    }
    else if(strcasecmp(CFG_STEP_STATE_LPSR, state))
    {
        return LSS_LPSR;
    }
    else if(strcasecmp(CFG_STEP_STATE_LUSG, state))
    {
        return LSS_LUSG;
    }
    else if(strcasecmp(CFG_STEP_STATE_LUSY, state))
    {
        return LSS_LUSY;
    }
    else if(strcasecmp(CFG_STEP_STATE_LUSR, state))
    {
        return LSS_LUSR;
    }
    else if(strcasecmp(CFG_STEP_STATE_LYSG, state))
    {
        return LSS_LYSG;
    }
    else if(strcasecmp(CFG_STEP_STATE_LYSY, state))
    {
        return LSS_LYSY;
    }
    else if(strcasecmp(CFG_STEP_STATE_LYSR, state))
    {
        return LSS_LYSR;
    }
    else if(strcasecmp(CFG_STEP_STATE_LRSG, state))
    {
        return LSS_LRSG;
    }
    else if(strcasecmp(CFG_STEP_STATE_LRSY, state))
    {
        return LSS_LRSY;
    }
    else if(strcasecmp(CFG_STEP_STATE_LRSR, state))
    {
        return LSS_LRSR;
    }
    else if(strcasecmp(CFG_STEP_STATE_off, state))
    {
        return LSS_off;
    }
    return LSS_unused;
}

