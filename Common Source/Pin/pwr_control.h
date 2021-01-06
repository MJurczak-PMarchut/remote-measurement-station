/*
 * pwr_control.h
 *
 *  Created on: 27 gru 2020
 *      Author: Ciućmok
 */

#ifndef PIN_PWR_CONTROL_H_
#define PIN_PWR_CONTROL_H_
#include "SpecialTypes.h"
#ifdef BOARD_N64_F4
	#include "stm32f4xx.h"
#else
	#include "stm32l4xx.h"
#endif
#include "station_specific.h"

HAL_StatusTypeDef Prepare_for_LPRun(void);
void SleepAndWaitForWkup(void);
void SetWkupContextPointer(WKUP_CONTEXT *psWkupPointer);

#endif /* PIN_PWR_CONTROL_H_ */
