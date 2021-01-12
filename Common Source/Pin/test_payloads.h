/*
 * test_payloads.h
 *
 *  Created on: 29 gru 2020
 *      Author: Mateusz Jurczak
 */

#ifndef REMOTE_APP_PIN_TEST_PAYLOADS_H_
#define REMOTE_APP_PIN_TEST_PAYLOADS_H_
#include "BOARD_DEF.h"
#ifdef BOARD_SENSORTILE
#include "stm32l4xx.h"
#endif
#ifdef BOARD_N64_F4
#include "stm32f4xx.h"
#endif
#include "station_specific.h"
#include "timings.h"
void TestPayload(void);
//void MemoryCopyTest(void);

#endif /* REMOTE_APP_PIN_TEST_PAYLOADS_H_ */
