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


//void InitTimer(void);

#endif /* REMOTE_APP_PIN_TIMINGS_H_ */
