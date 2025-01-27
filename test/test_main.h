/***************************************************************************************
 * @file    test_main.h
 * @date    January 22nd 2025
 *
 * @brief   
 *
 ****************************************************************************************/

#ifndef _TEST_MAIN_H_
#define _TEST_MAIN_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "main.h"

#define SIZE_LIGHT_ARRAY        (sizeof(light_t) * MAX_LIGHTS_IN_SET)
#define SIZE_STEP_ARRAY         (sizeof(lightSetStep_t) * MAX_STEPS_IN_PATTERN)

#define TEST_CFG1_PATH          "test/test_config1.json"
#define TEST_CFG1_OFF_STEP      5
#define TEST_CFG1_SIZE          3182

#define TEST_CFG2_PATH          "test/test_config2.json"
//#define TEST_CFG2_SIZE          3182
#define TEST_CFG3_PATH          "test/test_config3.json"

#define TEST_CFG_INV1_PATH      "test/test_config_invalid1.json"
#define TEST_CFG_INV2_PATH      "test/test_config_invalid2.json"
#define TEST_CFG_INV3_PATH      "test/test_config_invalid3.json"
#define TEST_CFG_INV4_PATH      "test/test_config_invalid4.json"
#define TEST_CFG_INV5_PATH      "test/test_config_invalid5.json"
#define TEST_CFG_INV6_PATH      "test/test_config_invalid6.json"
#define TEST_CFG_INV7_PATH      "test/test_config_invalid7.json"
#define TEST_CFG_INV8_PATH      "test/test_config_invalid8.json"
#define TEST_CFG_INV9_PATH      "test/test_config_invalid9.json"
#define TEST_CFG_INV10_PATH     "test/test_config_invalid10.json"
#define TEST_CFG_INV11_PATH     "test/test_config_invalid11.json"
#define TEST_CFG_INV12_PATH     "test/test_config_invalid12.json"
#define TEST_CFG_INV13_PATH     "test/test_config_invalid13.json"
#define TEST_CFG_INV14_PATH     "test/test_config_invalid14.json"


#endif //_TEST_MAIN_H_
