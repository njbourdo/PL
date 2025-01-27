/***************************************************************************************
 * @file    test_config.c
 * @date    January 25th 2025
 *
 * @brief   
 *
 ****************************************************************************************/
#include <stdlib.h>

#include "test_main.h"
#include "test_config.h"
//#include "intersection.h"
#include "config.h"
#include "lightSet.h"
#include "cJSON/cJSON.h"

//from lightSet.c
//extern lightSet_t* lightSet1;
//extern lightSet_t* lightSet2;



//from display.c
//extern uint8_t printedSetSteps[];

//from intersection.c
//extern intState_t intState;

//from config.c
extern lightSet_t lightConfigs[];
extern error_t parseConfig(const char* json);
extern error_t parseDirection(const cJSON* direction);
extern error_t parseLights(lightSet_t* lightConfig, const cJSON* lights);
extern error_t parseSteps(lightSet_t* lightConfig, const cJSON* steps);
extern intDirection_t getDirectionIdxFromString(char* dir);
extern lightDisplayType_t getLightTypeFromString(char* type);
extern lightSetState_t getStepStateFromString(char* state);
extern void* (*malloc_ptr)(size_t);  //function ptr for mocking
extern size_t (*fread_ptr)(void*, size_t, size_t, FILE*);  //function ptr for mocking

static size_t rcvdFileSize = 0;
static size_t rcvdMemSize = 0;

static void test_CFG_init(void **state);
static void test_CFG_loadDefaults(void **state);
static void test_CFG_getLightSet(void **state);
static void test_parseConfig(void **state);
static void test_parseDirection(void **state);
static void test_parseLights(void **state);
static void test_parseSteps(void **state);
static void test_getDirectionIdxFromString(void **state);
static void test_getLightTypeFromString(void **state);
static void test_getStepStateFromString(void **state);

static void* MOCK_malloc(size_t size)
{
    rcvdFileSize = size;
    //printf("%lu\n", size);
    return NULL;
}

static size_t MOCK_fread(void* ptr, size_t size, size_t count, FILE* stream)
{
    (void)ptr;
    (void)size;
    rcvdMemSize = count;
    (void)stream;
    return 0;
}

int test_config(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_CFG_init),
        cmocka_unit_test(test_CFG_loadDefaults),
        cmocka_unit_test(test_CFG_getLightSet),
        cmocka_unit_test(test_parseConfig),
        cmocka_unit_test(test_parseDirection),
        cmocka_unit_test(test_parseLights),
        cmocka_unit_test(test_parseSteps),
        cmocka_unit_test(test_getDirectionIdxFromString),
        cmocka_unit_test(test_getLightTypeFromString),
        cmocka_unit_test(test_getStepStateFromString),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}


 
//error_t CFG_init(char* filepath)
static void test_CFG_init(void **state)
{
    (void)state;
    
    //invalid file path
    assert_int_equal(CFG_init(TEST_CFG1_PATH TEST_CFG1_PATH), ERR_file);
    
    //mock malloc failure and confirm correct file size
    malloc_ptr = MOCK_malloc;
    assert_int_equal(CFG_init(TEST_CFG1_PATH), ERR_mem);
    assert_int_equal(rcvdFileSize, TEST_CFG1_SIZE+1);
    malloc_ptr = malloc;
    
    //mock fread failure and confirm correct memory size was allocated
    fread_ptr = MOCK_fread;
    assert_int_equal(CFG_init(TEST_CFG1_PATH), ERR_other);
    assert_int_equal(rcvdMemSize, TEST_CFG1_SIZE);
    fread_ptr = fread;
    
    //parse config failure
    assert_int_equal(CFG_init(TEST_CFG_INV1_PATH), ERR_format); //invalid JSON object (incorrectly spelled "intersection" key)
    
    //successful init
    assert_int_equal(CFG_init(TEST_CFG1_PATH), ERR_success);
}

//void CFG_loadDefaults(void)
static void test_CFG_loadDefaults(void **state)
{
    (void)state;
    
    lightSet_t defaultConfigs[INT_DIRECTIONS] = DEFAULT_CONFIG;
    
    //test update to default config
    assert_int_equal(CFG_init(TEST_CFG2_PATH), ERR_success);    
    //confirm no light or step arrays match
    for(intDirection_t dir = 0; dir < ID_numDirections; dir++)
    {
        assert_memory_not_equal(&lightConfigs[dir].lights, &defaultConfigs[dir].lights, SIZE_LIGHT_ARRAY);
        assert_memory_not_equal(&lightConfigs[dir].steps, &defaultConfigs[dir].steps, SIZE_STEP_ARRAY);
    }
    //load defaults
    CFG_loadDefaults();
    //confirm all light and step arrays match
    for(intDirection_t dir = 0; dir < ID_numDirections; dir++)
    {
        assert_memory_equal(&lightConfigs[dir].lights, &defaultConfigs[dir].lights, SIZE_LIGHT_ARRAY);
        assert_memory_equal(&lightConfigs[dir].steps, &defaultConfigs[dir].steps, SIZE_STEP_ARRAY);
    }
    
}

//lightSet_t* CFG_getLightSet(intDirection_t direction)
static void test_CFG_getLightSet(void **state)
{
    (void)state;
    
    //valid directions
    for(intDirection_t dir = 0; dir < ID_numDirections; dir++)
    {
        assert_ptr_equal(CFG_getLightSet(dir), &lightConfigs[dir]);
    }
    
    //invalid direction
    assert_ptr_equal(CFG_getLightSet(ID_numDirections), NULL);
}

//error_t parseConfig(const char* json)
static void test_parseConfig(void **state)
{
    (void)state;
    
    //invalid JSON format
    assert_int_equal(CFG_init(TEST_CFG_INV2_PATH), ERR_json);
    
    //invalid intersection object
    assert_int_equal(CFG_init(TEST_CFG_INV1_PATH), ERR_format); //invalid JSON object (incorrectly spelled "intersection" key)
    
    //no directions in config
    assert_int_equal(CFG_init(TEST_CFG_INV5_PATH), ERR_format);
    
    //one bad direction
    assert_int_equal(CFG_init(TEST_CFG_INV6_PATH), ERR_format);
    
    //only one direction per NS/EW
    assert_int_equal(CFG_init(TEST_CFG3_PATH), ERR_success);
    
    //four valid directions
    assert_int_equal(CFG_init(TEST_CFG1_PATH), ERR_success);
}

//error_t parseDirection(const cJSON* direction)
static void test_parseDirection(void **state)
{
    (void)state;
    
    //direction not a string (does not exist)
    assert_int_equal(CFG_init(TEST_CFG_INV5_PATH), ERR_format);
    
    //invalid direction string
    assert_int_equal(CFG_init(TEST_CFG_INV6_PATH), ERR_format); //spelled wring
    assert_int_equal(CFG_init(TEST_CFG_INV3_PATH), ERR_format); //number
    
    //invalid lights array
    assert_int_equal(CFG_init(TEST_CFG_INV4_PATH), ERR_format);
    
    //failed to parse lights
    assert_int_equal(CFG_init(TEST_CFG_INV7_PATH), ERR_format);
    
    //invalid steps array
    assert_int_equal(CFG_init(TEST_CFG_INV8_PATH), ERR_format);
    
    //failed to parse steps
    assert_int_equal(CFG_init(TEST_CFG_INV9_PATH), ERR_format);
    
    //success
    assert_int_equal(CFG_init(TEST_CFG1_PATH), ERR_success);
}

//error_t parseLights(lightSet_t* lightConfig, const cJSON* lights)
static void test_parseLights(void **state)
{
    (void)state;
    
    //too many lights
    assert_int_equal(CFG_init(TEST_CFG_INV10_PATH), ERR_format);    //6 lights
    
    //invalid light string
    assert_int_equal(CFG_init(TEST_CFG_INV7_PATH), ERR_format); //second light is 2 instead of "<" or "o"
    
    //valid light arrays
    assert_int_equal(CFG_init(TEST_CFG1_PATH), ERR_success); //2, 3, 4, and 5 lights
}

//error_t parseSteps(lightSet_t* lightConfig, const cJSON* steps)
static void test_parseSteps(void **state)
{
    (void)state;
    
    //too many steps
    assert_int_equal(CFG_init(TEST_CFG_INV11_PATH), ERR_format);    //11 steps
    
    //invalid step state string
    assert_int_equal(CFG_init(TEST_CFG_INV12_PATH), ERR_format);    //"State1" instead of "State" or "state"
    
    //invalid step state value type
    assert_int_equal(CFG_init(TEST_CFG_INV9_PATH), ERR_format);     //2 instead of "LxSx", "end", or "disable"
    
    //invalid step state string
    assert_int_equal(CFG_init(TEST_CFG_INV14_PATH), ERR_format);     //"badString"
    
    //invalid time value
    assert_int_equal(CFG_init(TEST_CFG_INV13_PATH), ERR_format);    //"0" instead of 0
    
    //correct time value parsing
    assert_int_equal(CFG_init(TEST_CFG3_PATH), ERR_success);
    assert_int_equal(lightConfigs[ID_north].steps[0].expirationOffset, 2000);
    assert_int_equal(lightConfigs[ID_north].steps[1].expirationOffset, 3000);
    assert_int_equal(lightConfigs[ID_north].steps[2].expirationOffset, 4000);
    assert_int_equal(lightConfigs[ID_north].steps[3].expirationOffset, (uint64_t)-1);
}

//intDirection_t getDirectionIdxFromString(char* dir)
static void test_getDirectionIdxFromString(void **state)
{
    (void)state;
    
    //successes
    assert_int_equal(getDirectionIdxFromString("north"), ID_north);
    assert_int_equal(getDirectionIdxFromString("NORTH"), ID_north);
    assert_int_equal(getDirectionIdxFromString("south"), ID_south);
    assert_int_equal(getDirectionIdxFromString("SOUTH"), ID_south);
    assert_int_equal(getDirectionIdxFromString("east"), ID_east);
    assert_int_equal(getDirectionIdxFromString("EAST"), ID_east);
    assert_int_equal(getDirectionIdxFromString("west"), ID_west);
    assert_int_equal(getDirectionIdxFromString("WEST"), ID_west);
    
    //fails
    assert_int_equal(getDirectionIdxFromString("TEST"), ID_numDirections);
    assert_int_equal(getDirectionIdxFromString("north "), ID_numDirections);
    assert_int_equal(getDirectionIdxFromString(" east"), ID_numDirections);
    assert_int_equal(getDirectionIdxFromString("0"), ID_numDirections);
}

//lightDisplayType_t getLightTypeFromString(char* type)
static void test_getLightTypeFromString(void **state)
{
    (void)state;
    
    //successes
    assert_int_equal(getLightTypeFromString("o"), LDT_solid);
    assert_int_equal(getLightTypeFromString("O"), LDT_solid);
    assert_int_equal(getLightTypeFromString("0"), LDT_solid);
    assert_int_equal(getLightTypeFromString("<"), LDT_arrow);
    
    //fails
    assert_int_equal(getLightTypeFromString(">"), LDT_unused);
    assert_int_equal(getLightTypeFromString(" <"), LDT_unused);
    assert_int_equal(getLightTypeFromString("1o"), LDT_unused);
    assert_int_equal(getLightTypeFromString("c"), LDT_unused);
}

//lightSetState_t getStepStateFromString(char* state)
static void test_getStepStateFromString(void **state)
{
    (void)state;
    
    //successes
    assert_int_equal(getStepStateFromString(CFG_STEP_STATE_LPSG), LSS_LPSG);
    assert_int_equal(getStepStateFromString(CFG_STEP_STATE_LPSY), LSS_LPSY);
    assert_int_equal(getStepStateFromString(CFG_STEP_STATE_LPSR), LSS_LPSR);
    assert_int_equal(getStepStateFromString(CFG_STEP_STATE_LUSG), LSS_LUSG);
    assert_int_equal(getStepStateFromString(CFG_STEP_STATE_LUSY), LSS_LUSY);
    assert_int_equal(getStepStateFromString(CFG_STEP_STATE_LUSR), LSS_LUSR);
    assert_int_equal(getStepStateFromString(CFG_STEP_STATE_LYSG), LSS_LYSG);
    assert_int_equal(getStepStateFromString(CFG_STEP_STATE_LYSY), LSS_LYSY);
    assert_int_equal(getStepStateFromString(CFG_STEP_STATE_LYSR), LSS_LYSR);
    assert_int_equal(getStepStateFromString("lysr"), LSS_LYSR);
    assert_int_equal(getStepStateFromString(CFG_STEP_STATE_LRSG), LSS_LRSG);
    assert_int_equal(getStepStateFromString(CFG_STEP_STATE_LRSY), LSS_LRSY);
    assert_int_equal(getStepStateFromString(CFG_STEP_STATE_LRSR), LSS_LRSR);
    assert_int_equal(getStepStateFromString(CFG_STEP_STATE_END), LSS_end);
    assert_int_equal(getStepStateFromString("END"), LSS_end);
    assert_int_equal(getStepStateFromString(CFG_STEP_STATE_DISABLE), LSS_disable);
    
    //fails
    assert_int_equal(getStepStateFromString("0"), LSS_unused);
    assert_int_equal(getStepStateFromString("END "), LSS_unused);
    assert_int_equal(getStepStateFromString(" END"), LSS_unused);
}

