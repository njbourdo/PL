/***************************************************************************************
 * @file     main.c
 * @date     January 19th 2025
 *
 * Main.c for test suite of traffic lights application
 *
 ****************************************************************************************/

#include "test_main.h"
#include "test_intersection.h"
#include "test_lightSet.h"
//#include "config.h"
//#include "lightSet.h"
//#include "display.h"

/*****************************************************************************
 ** @brief dummy test
 **     a test that does nothing and succeeds
 **
 ** @param state
 **
 ** @return none
******************************************************************************/
/*static void dummy_test(void **state) 
{
    (void) state;
}*/

/*****************************************************************************
 ** @brief test runner main
 **     Main file for the test suite
 **
 ** @param none
 **
 ** @return test result
******************************************************************************/
int main(void) 
{
    int result = 0;

    /*//test functions
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(dummy_test),
    };


    //run tests
    return cmocka_run_group_tests(tests, NULL, NULL);*/
    
    result += test_intersection();
    result += test_lightSet();
    
    return result;
}
