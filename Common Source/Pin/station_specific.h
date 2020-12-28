/*
 * station_specific.h
 *
 *  Created on: 7 sty 2020
 *      Author: Mateusz Jurczak
 */

#ifndef INC_STATION_SPECIFIC_H_
#define INC_STATION_SPECIFIC_H_
#include "configuration.h"
#include "SpecialTypes.h"
#ifdef BOARD_SENSORTILE
#include "SensorTile.h"
#include "SensorTile_env_sensors.h"
#include "SensorTile_motion_sensors.h"
#include "SensorTile_motion_sensors_ex.h"
#include "lsm6dsm_settings.h"
#include "lsm303agr_settings.h"
#endif

#ifdef BOARD_N64_F4
void BLE_GET_DATA(DataDecode *Data);
#endif

#ifdef BOARD_N64_F4
#include "stm32f4xx_hal.h"
	HAL_StatusTypeDef InitStation(UART_HandleTypeDef *huart);
	void SendNamesToScreen(uint8_t *pData, uint8_t len, uint8_t Number);
	void SendConnectionFeedback(BLE_CONNECTION_STATUS Status);
	void DiscoveryCplt(void);
	uint8_t* STATION_UART_RxCpltCallback(UART_HandleTypeDef *huart);
	uint8_t* GetDataFromBUffer(void);

	/*
	 * BlueNRG-MS background task, should be called periodically outside of ISR
	 */
	void Station_MS_Process(void);

#endif
#ifdef BOARD_SENSORTILE

	void InitBoard(void);
	void UpdateCharacteristics(void);
	void ClkDependentInit(void);



#endif
#endif /* INC_STATION_SPECIFIC_H_ */
