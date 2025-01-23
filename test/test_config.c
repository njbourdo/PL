/***************************************************************************************
 * @file    config.c
 * @date    January 20th 2025
 *
 * @brief   
 *
 ****************************************************************************************/
/*
#include <strings.h>
#include <stdlib.h>

#include "main.h"
#include "config.h"
#include "cJSON/cJSON.h"


static lightSet_t lightConfigs[INT_DIRECTIONS] = DEFAULT_CONFIG;

error_t parseConfig(const char* json);
error_t parseDirection(const cJSON* direction);
error_t parseLights(lightSet_t* lightConfig, const cJSON* lights);
error_t parseSteps(lightSet_t* lightConfig, const cJSON* steps);
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
        CFG_loadDefaults();
    }
    
    free(json);
}

void CFG_loadDefaults(void)
{
    lightSet_t defaultConfigs[INT_DIRECTIONS] = DEFAULT_CONFIG;
    
    for(uint8_t i = 0; i < INT_DIRECTIONS; i++)
    {
        lightConfigs[i] = defaultConfigs[i];
    }
}

lightSet_t* CFG_getLightSet(intDirection_t direction)
{
    if(direction > ID_west)
    {
        return NULL;
    }
    
    return &lightConfigs[direction];
}

error_t parseConfig(const char* json)
{
    error_t result = ERR_success;
    cJSON* root;
    const cJSON* intersection = NULL;
    const cJSON* direction = NULL;
    
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
        result = parseDirection(direction);
        if(result != ERR_success)
        {
            break;
        }
    }
    
    //free memory for cJSON object
    cJSON_Delete(root);
    
    return result;
}

error_t parseDirection(const cJSON* direction)
{
    const cJSON* lights = NULL;
    const cJSON* steps = NULL;
    const cJSON* value = NULL;
    intDirection_t directionIdx;
    error_t result = ERR_success;
    
    //get direction
    value = cJSON_GetObjectItem(direction, "direction");
    if(!cJSON_IsString(value))
    {
        printf("Direction value not a string!\n");
        return ERR_format;
    }
    directionIdx = getDirectionIdxFromString(value->valuestring);
    if(directionIdx >= ID_numDirections)
    {
        printf("Invalid direction string: %s\n", value->valuestring);
        return ERR_format;
    }
    printf("\n%s\n", value->valuestring);
    
    //get lights array
    lights = cJSON_GetObjectItem(direction, "lights");
    
    //parse lights
    result = parseLights(&lightConfigs[directionIdx], lights);
    if(result != ERR_success)
    {
        return result;
    }
    
    //get steps array
    steps = cJSON_GetObjectItem(direction, "steps");
    
    //parse steps
    result = parseLights(&lightConfigs[directionIdx], steps);
    if(result != ERR_success)
    {
        return result;
    }
    
    return result;
}

error_t parseLights(lightSet_t* lightConfig, const cJSON* lights)
{
    const cJSON* light = NULL;
    uint8_t lightIdx;
    lightDisplayType_t lightType;
    
    //for each light...
    lightIdx = 0;
    cJSON_ArrayForEach(light, lights)
    {
        if(lightIdx >= MAX_LIGHTS_IN_SET)
        {
            printf("Logic only supports %u lights per set\n", MAX_LIGHTS_IN_SET);
            return ERR_format;
        }
        if(!cJSON_IsString(light))
        {
            printf("Light type value not a string!\n");
            return ERR_format;
        }
        
        lightType = getLightTypeFromString(light->valuestring);
        lightConfig->lights[lightIdx].type = lightType;
        lightConfig->lights[lightIdx].state = LS_red;
        
        lightIdx++;
    }
    
    return ERR_success;
}

error_t parseSteps(lightSet_t* lightConfig, const cJSON* steps)
{
    const cJSON* step = NULL;
    const cJSON* value = NULL;
    uint8_t stepIdx;
    lightSetState_t stepState;
    uint64_t lastTime;
    
    //for each step...
    stepIdx = 0;
    lastTime = 0;
    cJSON_ArrayForEach(step, steps)
    {
        if(stepIdx >= MAX_STEPS_IN_PATTERN)
        {
            printf("Logic only supports %u steps per pattern\n", MAX_STEPS_IN_PATTERN);
            return ERR_format;
        }
        
        //get and validate state
        value = cJSON_GetObjectItem(step, "state");
        if(!cJSON_IsString(value))
        {
            printf("Step state value not a string!\n");
            return ERR_format;
        }
        stepState = getStepStateFromString(value->valuestring);
        if(stepState >= LSS_unused)
        {
            printf("Invalid step state string: %s\n", value->valuestring);
            return ERR_format;
        }
        printf("%s\n", value->valuestring);
        
        //get and validate time
        value = cJSON_GetObjectItem(step, "time");
        if(!cJSON_IsNumber(value))
        {
            printf("Step time value not a number!\n");
            return ERR_format;
        }
        
        //assign step state and time values
        lightConfig->steps[stepIdx].state = stepState;
        
        //users enter state start times but config expects state end times, so set first to 0, last to never expire, and the rest to the time of the previous step
        if(stepIdx == 0)    //first step
        {
            lightConfig->steps[stepIdx].expirationOffset = 0;
        }
        else if(stepState == LSS_off) //last step
        {
            lightConfigs->steps[stepIdx - 1].expirationOffset = (uint64_t)value->valueint;
            lightConfigs->steps[stepIdx].expirationOffset = (uint64_t)-1;
        }
        else    //every step in between
        {
            lightConfig->steps[stepIdx - 1].expirationOffset = lastTime;
        }
        
        lastTime = (uint64_t)value->valueint;
        
        stepIdx++;
    }
    
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
}*/

