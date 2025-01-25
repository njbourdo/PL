/***************************************************************************************
 * @file    test_intersection.c
 * @date    January 22nd 2025
 *
 * @brief   
 *
 ****************************************************************************************/
#define _POSIX_C_SOURCE 199309L
#include <time.h>

#include "test_main.h"
#include "test_intersection.h"
#include "intersection.h"
#include "config.h"
#include "lightSet.h"

#define SIZE_STEP_ARRAY         (sizeof(lightSetStep_t) * MAX_STEPS_IN_PATTERN)

//from lightSet.c
extern lightSet_t* lightSet1;
extern lightSet_t* lightSet2;

//from config.c
extern lightSet_t lightConfigs[];

//from display.c
extern uint8_t printedSetSteps[];

//from intersection.c
extern intState_t intState;
extern const lightSetStep_t errorSteps[];
extern error_t (*changeActiveDirection_ptr)(intState_t, uint64_t);
extern uint64_t getMillis(void);
extern error_t toggleActiveDirection(uint64_t millis);
extern error_t changeActiveDirection(intState_t state, uint64_t millis);

static void test_INT_init(void **state);
static void test_INT_stateMachine(void **state);
static void test_getMillis(void **state);
static void test_toggleActiveDirection(void **state);
static void test_changeActiveDirection(void **state);

error_t MOCK_changeActiveDirection(intState_t state, uint64_t millis)
{
    (void)state;
    (void)millis;
    
    return ERR_other;
}

int test_intersection(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_INT_init),
        cmocka_unit_test(test_INT_stateMachine),
        cmocka_unit_test(test_getMillis),
        cmocka_unit_test(test_toggleActiveDirection),
        cmocka_unit_test(test_changeActiveDirection),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

static void test_INT_init(void **state)
{
    (void)state;
    
    //ensure expected config was received
    INT_init(TEST_CFG1_PATH);
    assert_int_equal(lightConfigs[ID_east].steps[4].expirationOffset, 7777);
    assert_int_equal(lightConfigs[ID_west].steps[4].expirationOffset, 7890);
}

static void test_INT_stateMachine(void **state)
{
    (void)state;
    
    //initialize system with the appropriate test configuration
    CFG_init(TEST_CFG1_PATH);
    
    //switch from off to ns
    intState = IS_off;
    INT_stateMachine();
    assert_int_equal(intState, IS_ns);
    //ensure correct sets have been loaded based on unique light type configs
    assert_int_equal(lightSet1->lights[2].type, LDT_unused);
    assert_int_equal(lightSet2->lights[2].type, LDT_solid);
    assert_int_equal(lightSet2->lights[3].type, LDT_unused);
    
    //switch from ns to ew
    //setup config to ensure state change
    lightSet1->currentStep = TEST_CFG1_OFF_STEP - 1;
    lightSet1->steps[TEST_CFG1_OFF_STEP - 1].expirationOffset = 0;
    lightSet1->cycleStartTime = 0;
    lightSet2->currentStep = TEST_CFG1_OFF_STEP - 1;
    lightSet2->steps[TEST_CFG1_OFF_STEP - 1].expirationOffset = 0;
    lightSet2->cycleStartTime = 0;
    INT_stateMachine();
    assert_int_equal(intState, IS_ew);
    
    //switch from ew to ns
    //setup config to ensure state change
    lightSet1->currentStep = TEST_CFG1_OFF_STEP - 1;
    lightSet1->steps[TEST_CFG1_OFF_STEP - 1].expirationOffset = 0;
    lightSet1->cycleStartTime = 0;
    lightSet2->currentStep = TEST_CFG1_OFF_STEP - 1;
    lightSet2->steps[TEST_CFG1_OFF_STEP - 1].expirationOffset = 0;
    lightSet2->cycleStartTime = 0;
    INT_stateMachine();
    assert_int_equal(intState, IS_ns);
    
    //confirm DISP_printLightStates is getting called
    printedSetSteps[ID_north] = MAX_STEPS_IN_PATTERN;
    printedSetSteps[ID_south] = MAX_STEPS_IN_PATTERN;
    INT_stateMachine();
    assert_int_equal(printedSetSteps[ID_north], 0);
    assert_int_equal(printedSetSteps[ID_south], 0);
    
    //check default case error check
    intState = IS_off;
    changeActiveDirection_ptr = MOCK_changeActiveDirection;
    assert_memory_not_equal(lightConfigs[ID_north].steps, &errorSteps, SIZE_STEP_ARRAY);
    INT_stateMachine();
    assert_memory_equal(lightConfigs[ID_north].steps, &errorSteps, SIZE_STEP_ARRAY);
    
    //reset configs
    CFG_init(TEST_CFG1_PATH);
    
    //check default case error check
    assert_int_equal(intState, IS_ew);
    assert_memory_not_equal(lightConfigs[ID_north].steps, &errorSteps, SIZE_STEP_ARRAY);
    //setup config to ensure state change
    lightSet1->currentStep = TEST_CFG1_OFF_STEP - 1;
    lightSet1->steps[TEST_CFG1_OFF_STEP - 1].expirationOffset = 0;
    lightSet1->cycleStartTime = 0;
    lightSet2->currentStep = TEST_CFG1_OFF_STEP - 1;
    lightSet2->steps[TEST_CFG1_OFF_STEP - 1].expirationOffset = 0;
    lightSet2->cycleStartTime = 0;
    INT_stateMachine();
    assert_memory_equal(lightConfigs[ID_north].steps, &errorSteps, SIZE_STEP_ARRAY);
    
    //reset function pointer
    changeActiveDirection_ptr = changeActiveDirection;
}

static void test_getMillis(void **state)
{
    (void)state;
    uint64_t msTime;
    struct timespec ts = {2, 0};
    
    //confirm time is working
    msTime = getMillis();
    nanosleep(&ts, NULL);
    assert_in_range(getMillis(), msTime+2000, msTime+2002);
}

static void test_toggleActiveDirection(void **state)
{
    (void)state;

    //Change from NS to EW
    intState = IS_ns;
    assert_int_equal(toggleActiveDirection(0), ERR_success);
    assert_int_equal(intState, IS_ew);
    
    //Change from EW to NS
    assert_int_equal(toggleActiveDirection(0), ERR_success);
    assert_int_equal(intState, IS_ns);
    
    //Stay in any state other than NS or EW
    intState = IS_off;
    assert_int_equal(toggleActiveDirection(0), ERR_success);
    assert_int_equal(intState, IS_off);
    
    //check other return val
    changeActiveDirection_ptr = MOCK_changeActiveDirection;
    intState = IS_ns;
    assert_int_equal(toggleActiveDirection(0), ERR_other);
    assert_int_equal(intState, IS_ns);
    changeActiveDirection_ptr = changeActiveDirection;
}


static void test_changeActiveDirection(void **state)
{
    (void)state;
    
    //initialize system with the appropriate test configuration
    CFG_init(TEST_CFG1_PATH);
    
    //state validity check
    intState = IS_ns;
    assert_int_equal(changeActiveDirection(IS_off, 0), ERR_value);
    assert_int_equal(intState, IS_ns);
    
    //state difference check
    intState = IS_ns;
    lightSet1->cycleStartTime = 0;
    assert_int_equal(changeActiveDirection(IS_ns, 1), ERR_success);
    assert_int_equal(intState, IS_ns);
    assert_int_equal(lightSet1->cycleStartTime, 0); //confirms function returned where expected
    
    //ns to ew change
    intState = IS_ns;
    lightSet1 = NULL;
    lightSet2 = NULL;
    assert_int_equal(changeActiveDirection(IS_ew, 1), ERR_success);
    assert_int_equal(intState, IS_ew);
    assert_non_null(lightSet1);
    assert_non_null(lightSet2);
    assert_int_equal(lightSet1->cycleStartTime, 1);
    assert_int_equal(lightSet2->cycleStartTime, 1);
    assert_ptr_equal(lightSet1, &lightConfigs[ID_east]);
    assert_ptr_equal(lightSet2, &lightConfigs[ID_west]);
    
    //ew to ns change
    lightSet1 = NULL;
    lightSet2 = NULL;
    assert_int_equal(changeActiveDirection(IS_ns, 5), ERR_success);
    assert_int_equal(intState, IS_ns);
    assert_non_null(lightSet1);
    assert_non_null(lightSet2);
    assert_int_equal(lightSet1->cycleStartTime, 5);
    assert_int_equal(lightSet2->cycleStartTime, 5);
    assert_ptr_equal(lightSet1, &lightConfigs[ID_north]);
    assert_ptr_equal(lightSet2, &lightConfigs[ID_south]);
    
    //ns to error change
    intState = IS_ns;
    lightSet1 = NULL;
    lightSet2 = NULL;
    assert_memory_not_equal(lightConfigs[ID_north].steps, &errorSteps, SIZE_STEP_ARRAY);
    assert_memory_not_equal(lightConfigs[ID_south].steps, &errorSteps, SIZE_STEP_ARRAY);
    assert_memory_not_equal(lightConfigs[ID_east].steps, &errorSteps, SIZE_STEP_ARRAY);
    assert_memory_not_equal(lightConfigs[ID_west].steps, &errorSteps, SIZE_STEP_ARRAY);
    assert_int_equal(changeActiveDirection(IS_error, 1), ERR_success);
    assert_int_equal(intState, IS_ew);
    assert_non_null(lightSet1);
    assert_non_null(lightSet2);
    assert_memory_equal(lightConfigs[ID_north].steps, &errorSteps, SIZE_STEP_ARRAY);
    assert_memory_equal(lightConfigs[ID_south].steps, &errorSteps, SIZE_STEP_ARRAY);
    assert_memory_equal(lightConfigs[ID_east].steps, &errorSteps, SIZE_STEP_ARRAY);
    assert_memory_equal(lightConfigs[ID_west].steps, &errorSteps, SIZE_STEP_ARRAY);
    
}

