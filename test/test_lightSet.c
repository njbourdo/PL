/***************************************************************************************
 * @file    test_lightSet.c
 * @date    January 23rd 2025
 *
 * @brief   
 *
 ****************************************************************************************/
#include "test_main.h"
#include "test_lightSet.h"
#include "config.h"
#include "lightSet.h"

//from lightSet.c
extern lightSet_t* lightSet1;
extern lightSet_t* lightSet2;
extern lightSetState_t clockLightSetStateMachine(lightSet_t* set, uint64_t millis);
extern lightSetState_t incrementLightSetStep(lightSet_t* set);
extern lightState_t getArrowState(lightSetState_t setState);
extern lightState_t getSolidGreenState(lightSetState_t setState);

//from config.c
extern lightSet_t lightConfigs[];


static void test_SET_assignLights(void **state);
static void test_SET_stateMachine(void **state);
static void test_clockLightSetStateMachine(void **state);
static void test_incrementLightSetStep(void **state);
static void test_getArrowState(void **state);
static void test_getSolidGreenState(void **state);

int test_lightSet(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_SET_assignLights),
        cmocka_unit_test(test_SET_stateMachine),
        cmocka_unit_test(test_clockLightSetStateMachine),
        cmocka_unit_test(test_incrementLightSetStep),
        cmocka_unit_test(test_getArrowState),
        cmocka_unit_test(test_getSolidGreenState),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

//error_t SET_assignLights(lightSet_t* set1, lightSet_t* set2, uint64_t startTime)
static void test_SET_assignLights(void **state)
{
    (void)state;
    
    lightSet_t set1, set2;
    
    //setting of new pointers
    assert_ptr_not_equal(lightSet1, &set1);
    assert_ptr_not_equal(lightSet2, &set2);
    assert_int_equal(SET_assignLights(&set1, &set2, 0), ERR_success);
    assert_ptr_equal(lightSet1, &set1);
    assert_ptr_equal(lightSet2, &set2);
    
    //null pointer tests
    assert_int_equal(SET_assignLights(NULL, &set2, 0), ERR_nullPtr);
    assert_int_equal(SET_assignLights(&set1, NULL, 0), ERR_nullPtr);
    assert_int_equal(SET_assignLights(NULL, NULL, 0), ERR_nullPtr);
    
    //updating cycle start times
    set1.cycleStartTime = 0;
    set2.cycleStartTime = 0;
    assert_int_equal(SET_assignLights(&set1, &set2, 13), ERR_success);
    assert_int_equal(set1.cycleStartTime, 13);
    assert_int_equal(set2.cycleStartTime, 13);
}


//lightSetState_t SET_stateMachine(uint64_t millis)
static void test_SET_stateMachine(void **state)
{
    (void)state;
    
    //setup system config
    assert_int_equal(CFG_init(TEST_CFG1_PATH), ERR_success);
    
    //clocking of both state machines
    assert_int_equal(SET_assignLights(&lightConfigs[ID_north], &lightConfigs[ID_south], 0), ERR_success);
    lightSet1->currentStep = 0;  //LPSR
    lightSet2->currentStep = 0;  //LPSR
    assert_int_equal(SET_stateMachine(2000), LSS_LUSR); //set 1&2, step 0 ends @ 2000mS
    assert_int_equal(lightSet1->currentStep, 1); //LUSR
    assert_int_equal(lightSet2->currentStep, 1); //LUSR
    
    //get overall state from set 1
    assert_int_equal(SET_stateMachine(3000), LSS_LUSG); //set 1, step 1 ends @ 3000mS
    assert_int_equal(lightSet1->currentStep, 2); //LUSG < LUSR
    assert_int_equal(lightSet2->currentStep, 1); //LUSR
    
    //set overall state from set 2
    assert_int_equal(SET_stateMachine(4000), LSS_LUSG); //set 2, step 1 ends @ 4000mS
    assert_int_equal(lightSet1->currentStep, 2); //LUSG
    assert_int_equal(lightSet2->currentStep, 2); //LUSG
    assert_int_equal(SET_stateMachine(4500), LSS_LUSG); //set 1, step 2 ends @ 4500mS
    assert_int_equal(lightSet1->currentStep, 3); //LYSY > LUSG
    assert_int_equal(lightSet2->currentStep, 2); //LUSG
    
    //both states @ end
    assert_int_equal(SET_stateMachine(5000), LSS_LYSY); //set 2, step 2 ends @ 5000mS
    assert_int_equal(lightSet1->currentStep, 3); //LYSY
    assert_int_equal(lightSet2->currentStep, 3); //LYSY
    assert_int_equal(SET_stateMachine(6000), LSS_LRSR); //set 1&2, step 3 ends @ 6000mS
    assert_int_equal(lightSet1->currentStep, 4); //LRSR
    assert_int_equal(lightSet2->currentStep, 4); //LRSR
    assert_int_equal(SET_stateMachine(7000), LSS_end); //sets 1&2 end @ 7000mS
    assert_int_equal(lightSet1->currentStep, 5); //end
    assert_int_equal(lightSet2->currentStep, 5); //end
}

//lightSetState_t clockLightSetStateMachine(lightSet_t* set, uint64_t millis);
static void test_clockLightSetStateMachine(void **state)
{
    (void)state;
    
    //setup system config
    assert_int_equal(CFG_init(TEST_CFG1_PATH), ERR_success);
    assert_int_equal(SET_assignLights(&lightConfigs[ID_north], &lightConfigs[ID_south], 0), ERR_success);
    lightSet1->currentStep = 0;
    lightSet2->currentStep = 0;
    
    //invalid ptr check
    assert_int_equal(clockLightSetStateMachine(NULL, 0), LSS_end);
    
    //unused set check
    assert_int_not_equal(clockLightSetStateMachine(lightSet1, 0), LSS_end);
    lightSet1->steps[0].state = LSS_unused;
    assert_int_equal(clockLightSetStateMachine(lightSet1, 0), LSS_end);
    
    //state not yet expired
    assert_int_equal(lightSet2->currentStep, 0);
    assert_int_equal(lightSet2->steps[0].state, LSS_LPSR);
    assert_int_equal(lightSet2->steps[0].expirationOffset, 2000);
    assert_int_equal(clockLightSetStateMachine(lightSet2, 0), LSS_LPSR);
    assert_int_equal(clockLightSetStateMachine(lightSet2, 1000), LSS_LPSR);
    assert_int_equal(clockLightSetStateMachine(lightSet2, 1999), LSS_LPSR);
    assert_int_equal(lightSet2->currentStep, 0);
    
    //state just expired
    assert_int_equal(lightSet2->steps[1].state, LSS_LUSR);
    assert_int_equal(clockLightSetStateMachine(lightSet2, 2000), LSS_LUSR);
    assert_int_equal(lightSet2->currentStep, 1);
    
    //state long past expired
    assert_int_equal(lightSet2->steps[2].state, LSS_LUSG);
    assert_int_equal(lightSet2->steps[1].expirationOffset, 4000);
    assert_int_equal(clockLightSetStateMachine(lightSet2, 9000), LSS_LUSG);
    assert_int_equal(lightSet2->currentStep, 2);
}

//lightSetState_t incrementLightSetStep(lightSet_t* set);
static void test_incrementLightSetStep(void **state)
{
    (void)state;
    
    //setup system config
    assert_int_equal(CFG_init(TEST_CFG1_PATH), ERR_success);
    assert_int_equal(SET_assignLights(&lightConfigs[ID_north], &lightConfigs[ID_south], 0), ERR_success);
    lightSet1->currentStep = MAX_STEPS_IN_PATTERN - 1;
    lightSet2->currentStep = 0;
    
    //increment step number (wrap-around and not)
    assert_int_equal(lightSet1->steps[0].state, LSS_LPSR);
    assert_int_equal(incrementLightSetStep(lightSet1), LSS_LPSR);
    assert_int_equal(lightSet1->currentStep, 0);
    assert_int_equal(lightSet1->steps[1].state, LSS_LUSR);
    assert_int_equal(incrementLightSetStep(lightSet1), LSS_LUSR);
    assert_int_equal(lightSet1->currentStep, 1);
    
    //skip unused steps
    lightSet1->currentStep = 5;
    assert_int_equal(lightSet1->steps[6].state, LSS_unused);
    assert_int_equal(lightSet1->steps[7].state, LSS_unused);
    assert_int_equal(lightSet1->steps[8].state, LSS_unused);
    assert_int_equal(lightSet1->steps[9].state, LSS_unused);
    assert_int_equal(MAX_STEPS_IN_PATTERN, 10);
    assert_int_equal(incrementLightSetStep(lightSet1), LSS_LPSR);
    assert_int_equal(lightSet1->currentStep, 0);
    
    //setting appropriate states for different light types and skipping all lights after an unused one
    assert_int_equal(SET_assignLights(&lightConfigs[ID_east], &lightConfigs[ID_west], 0), ERR_success);
    lightSet1->currentStep = 2;
    lightSet2->currentStep = 2;
    assert_int_equal(lightSet1->steps[3].state, LSS_LUSY);  //state with different values for different light types
    assert_int_equal(lightSet1->lights[0].type, LDT_arrow); //arrow light
    lightSet1->lights[0].state = LS_off;
    assert_int_equal(lightSet1->lights[1].type, LDT_solid); //solid light
    lightSet1->lights[1].state = LS_off;
    lightSet1->lights[2].type = LDT_unused; //set as unused light
    lightSet1->lights[2].state = LS_off;
    lightSet1->lights[3].type = LDT_solid;  //set a dummy light other than unused which should be skipped
    lightSet1->lights[3].state = LS_off;
    assert_int_equal(incrementLightSetStep(lightSet1), LSS_LUSY);   //switched to expected state
    assert_int_equal(lightSet1->lights[0].state, LS_yellowArrow);   //arrow light
    assert_int_equal(lightSet1->lights[1].state, LS_yellow);        //arrow light
    assert_int_equal(lightSet1->lights[2].state, LS_off);           //unused light
    assert_int_equal(lightSet1->lights[3].state, LS_off);           //skipped arrow light
}

//lightState_t getArrowState(lightSetState_t setState);
static void test_getArrowState(void **state)
{
    (void)state;
    
    assert_int_equal(getArrowState(LSS_LPSG), LS_green);
    assert_int_equal(getArrowState(LSS_LPSY), LS_green);
    assert_int_equal(getArrowState(LSS_LPSR), LS_green);
    
    assert_int_equal(getArrowState(LSS_LUSG), LS_yellowArrow);
    assert_int_equal(getArrowState(LSS_LUSY), LS_yellowArrow);
    assert_int_equal(getArrowState(LSS_LUSR), LS_yellowArrow);
    
    assert_int_equal(getArrowState(LSS_LYSG), LS_yellow);
    assert_int_equal(getArrowState(LSS_LYSY), LS_yellow);
    assert_int_equal(getArrowState(LSS_LYSR), LS_yellow);
    
    assert_int_equal(getArrowState(LSS_LRSG), LS_red);
    assert_int_equal(getArrowState(LSS_LRSY), LS_red);
    assert_int_equal(getArrowState(LSS_LRSR), LS_red);
    assert_int_equal(getArrowState(LSS_end), LS_red);
    
    assert_int_equal(getArrowState(LSS_disable), LS_off);
    assert_int_equal(getArrowState(LSS_unused), LS_off);
}

//lightState_t getSolidGreenState(lightSetState_t setState);
static void test_getSolidGreenState(void **state)
{
    (void)state;
    
    assert_int_equal(getSolidGreenState(LSS_LPSG), LS_green);
    assert_int_equal(getSolidGreenState(LSS_LUSG), LS_green);
    assert_int_equal(getSolidGreenState(LSS_LYSG), LS_green);
    assert_int_equal(getSolidGreenState(LSS_LRSG), LS_green);
    
    assert_int_equal(getSolidGreenState(LSS_LPSY), LS_yellow);
    assert_int_equal(getSolidGreenState(LSS_LUSY), LS_yellow);
    assert_int_equal(getSolidGreenState(LSS_LYSY), LS_yellow);
    assert_int_equal(getSolidGreenState(LSS_LRSY), LS_yellow);
    
    assert_int_equal(getSolidGreenState(LSS_LPSR), LS_red);
    assert_int_equal(getSolidGreenState(LSS_LUSR), LS_red);
    assert_int_equal(getSolidGreenState(LSS_LYSR), LS_red);
    assert_int_equal(getSolidGreenState(LSS_LRSR), LS_red);
    assert_int_equal(getSolidGreenState(LSS_end), LS_red);
    
    assert_int_equal(getSolidGreenState(LSS_disable), LS_off);
    assert_int_equal(getSolidGreenState(LSS_unused), LS_off);
}

