/*
 * Timings.h
 *
 *  Created on: 2 sty 2021
 *      Author: Mateusz Jurczak
 */

#ifndef REMOTE_APP_PIN_TIMINGS_H_
#define REMOTE_APP_PIN_TIMINGS_H_
#include "configuration.h"
#include "BOARD_DEF.h"
#ifdef BOARD_SENSORTILE
 #include "stm32l4xx.h"
 #include "stm32l4xx_hal.h"
#endif
#ifdef BOARD_N64
 #include "stm32f4xx.h"
 #include "stm32f4xx_hal.h"
#endif

#ifdef BOARD_SENSORTILE
void InitTimer2(TIM_HandleTypeDef *htim);
#ifndef TIM2_FREE_RUN
void TimingIncTick(void);
#endif
uint32_t GetTim2Tick(void);
void ResetTimCounter(void);
void StartTim(void);
void StopTim(void);
#endif

#endif /* REMOTE_APP_PIN_TIMINGS_H_ */
