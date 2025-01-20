/***************************************************************************************
 * @file     main.c
 * @date     January 19th 2025
 *
 * Main.c for test suite of traffic lights application
 *
 ****************************************************************************************/


#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>


//#include "lights.h"

/*****************************************************************************
 ** @brief null test
 **     a test that does nothing and succeeds
 **
 ** @param state
 **
 ** @return none
******************************************************************************/
static void null_test_success(void **state) 
{
    (void) state;
}

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

    //test functions
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(null_test_success),
    };


    //run tests
    return cmocka_run_group_tests(tests, NULL, NULL);
}
