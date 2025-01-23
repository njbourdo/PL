/***************************************************************************************
 * @file    test_intersection.c
 * @date    January 22nd 2025
 *
 * @brief   
 *
 ****************************************************************************************/

#include "test_main.h"
#include "test_intersection.h"
#include "intersection.h"
#include "config.h"
#include "lightSet.h"

#define TEST_CFG1_PATH          "test/test_config1.json"
#define TEST_CFG1_OFF_STEP      5

//from lightSet.c
extern lightSet_t* lightSet1;
extern lightSet_t* lightSet2;

//from config.c
extern lightSet_t lightConfigs[];

//from display.c
extern uint8_t printedSetSteps[];

//from intersection.c
extern intState_t intState;

void test_INT_stateMachine(void **state)
{   
/* 
    switch(intState)
    {
        case IS_ns:
        case IS_ew:
            if(SET_stateMachine(millis) == LSS_off)
            {
                toggleActiveDirection(millis);
            }
            break;
        default:
            changeActiveDirection(IS_ns, millis);
            break;
    }
    
    DISP_printLightStates();
    */
    (void)state;
    
    //initialize system with the appropriate test configuration
    CFG_init(TEST_CFG1_PATH);

    
    //switch from off to ns
    intState = IS_off;
    INT_stateMachine();
    assert_int_equal(intState, IS_ns);
    //ensure correct set have been loaded based on unique light type configs
    assert_int_equal(lightSet1->lights[2].type, LDT_unused);
    assert_int_equal(lightSet2->lights[2].type, LDT_solid);
    assert_int_equal(lightSet2->lights[3].type, LDT_unused);
    
    //switch from ns to ew
    lightSet1->currentStep = TEST_CFG1_OFF_STEP - 1;
    lightSet1->steps[TEST_CFG1_OFF_STEP - 1].expirationOffset = 0;
    lightSet1->cycleStartTime = 0;
    lightSet2->currentStep = TEST_CFG1_OFF_STEP - 1;
    lightSet2->steps[TEST_CFG1_OFF_STEP - 1].expirationOffset = 0;
    lightSet2->cycleStartTime = 0;
    INT_stateMachine();
    assert_int_equal(intState, IS_ew);
    
    //switch from ew to ns
    lightSet1->currentStep = TEST_CFG1_OFF_STEP - 1;
    lightSet1->steps[TEST_CFG1_OFF_STEP - 1].expirationOffset = 0;
    lightSet1->cycleStartTime = 0;
    lightSet2->currentStep = TEST_CFG1_OFF_STEP - 1;
    lightSet2->steps[TEST_CFG1_OFF_STEP - 1].expirationOffset = 0;
    lightSet2->cycleStartTime = 0;
    INT_stateMachine();
    assert_int_equal(intState, IS_ns);
    
    //confirm state changes take effect
    printedSetSteps[ID_north] = MAX_STEPS_IN_PATTERN;
    printedSetSteps[ID_south] = MAX_STEPS_IN_PATTERN;
    INT_stateMachine();
    assert_int_equal(printedSetSteps[ID_north], 0);
    assert_int_equal(printedSetSteps[ID_south], 0);
    
}

extern void toggleActiveDirection(uint64_t millis);
static void test_toggleActiveDirection(void **state)
{
    (void)state;

    //Change from NS to EW
    intState = IS_ns;
    toggleActiveDirection(0);
    assert_int_equal(intState, IS_ew);
    
    //Change from EW to NS
    toggleActiveDirection(0);
    assert_int_equal(intState, IS_ns);
    
    //Stay in any state other than NS or EW
    intState = IS_off;
    toggleActiveDirection(0);
    assert_int_equal(intState, IS_off);
}


extern void changeActiveDirection(intState_t state, uint64_t millis);
static void test_changeActiveDirection(void **state)
{
    (void)state;
    
    //state validity check
    intState = IS_ns;
    changeActiveDirection(IS_off, 0);
    assert_int_equal(intState, IS_ns);
    
    //state difference check
    intState = IS_ns;
    lightSet1->cycleStartTime = 0;
    changeActiveDirection(IS_ns, 1);
    assert_int_equal(intState, IS_ns);
    assert_int_equal(lightSet1->cycleStartTime, 0); //confirms function returned where expected
    
    //ns to ew change
    intState = IS_ns;
    lightSet1 = NULL;
    lightSet2 = NULL;
    changeActiveDirection(IS_ew, 1);
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
    changeActiveDirection(IS_ns, 5);
    assert_int_equal(intState, IS_ns);
    assert_non_null(lightSet1);
    assert_non_null(lightSet2);
    assert_int_equal(lightSet1->cycleStartTime, 5);
    assert_int_equal(lightSet2->cycleStartTime, 5);
    assert_ptr_equal(lightSet1, &lightConfigs[ID_north]);
    assert_ptr_equal(lightSet2, &lightConfigs[ID_south]);
}


int test_intersection(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_INT_stateMachine),
        cmocka_unit_test(test_toggleActiveDirection),
        cmocka_unit_test(test_changeActiveDirection),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

