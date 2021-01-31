/*
 * ble_decode_callback.h
 *
 *  Created on: 3 sty 2020
 *      Author: Mateusz Jurczak
 */

#ifndef ST_BLUENRG_MS_INCLUDES_BLE_DECODE_CALLBACK_H_
#define ST_BLUENRG_MS_INCLUDES_BLE_DECODE_CALLBACK_H_


#include "bluenrg_gatt_aci.h"
#include "bluenrg_gap.h"
#include "SpecialTypes.h"
#include "configuration.h"
#include "bluenrg_gap_aci.h"
#ifdef HAS_BLUETOOTH
#ifdef BOARD_N64_F4
	#include "stm32f4xx.h"
#endif
#ifdef BOARD_SENSORTILE
	#include "stm32l4xx.h"
#endif
void BLE_EVNT_CALLBACK(void * pData);
void BLE_DECODE_GROUP_RESP(evt_att_read_by_group_resp *cc);
HAL_StatusTypeDef SET_DECODER_STATE(BLE_State eBLE_State);
BLE_State GET_DECODER_STATE(void);
void CLR_CALLBACK_FUNC(void);
#ifdef BOARD_N64_F4
void BLE_GET_DEV_DATA(uint8_t DevNo, BLE_DEV_DATA **DevStruct);
#ifdef BOARD_N64_F4
HAL_StatusTypeDef BLE_GET_DEV_NO_BY_HANDLE(uint16_t ConnHandle, uint8_t *__DevNo_BDH);
#endif
HAL_StatusTypeDef BLE_READ_CHAR(uint16_t ConnHandle, uint8_t *pData, uint8_t len, CharIDData *Characteristic, VoidFuncPointer Callback);
HAL_StatusTypeDef BLE_READ_MULTIPLE_CHAR(uint16_t ConnHandle, uint8_t *pData, uint8_t len,  uint8_t *Characteristic, uint8_t NoOfhandles, VoidFuncPointer Callback);
#endif
#ifdef BOARD_SENSORTILE
void BLE_GET_DEV_DATA(BLE_DEV_DATA **DevStruct);
#endif
//HAL_StatusTypeDef BLE_SET_DISC_DEV_NO(uint8_t DevNo);
HAL_StatusTypeDef BLE_START_DISC_ALL_DEV_SERVICES(void);
HAL_StatusTypeDef BLE_START_DISC_ALL_DEV_CHAR(void);
HAL_StatusTypeDef BLE_ADV_REPORT_CALLBACK(le_advertising_info *cc);
void BLE_CLEAR_ADV_DATA(void);
HAL_StatusTypeDef BLE_CONNECT_TO_DEVICE(uint8_t DevNo, BLE_DEVICE_LOCATION ble_dl);
HAL_StatusTypeDef BLE_START_SCAN(VoidFuncPointer Callback);
#ifdef BOARD_N64_F4
HAL_StatusTypeDef BLE_WRITE_CHAR(uint8_t DevNo, ServiceIDData *Service, CharIDData *Characteristic, uint8_t len, uint8_t *pData, VoidFuncPointer Callback);
#endif
#ifdef BOARD_SENSORTILE
HAL_StatusTypeDef BLE_UPDATE_CHAR(ServiceIDData *Service, CharIDData *Characteristic, uint8_t len, uint8_t *pData, VoidFuncPointer Callback);
#endif
void BLE_INIT_SPEC(void);
tBleStatus BLE_ADD_SERVICES(void);
HAL_StatusTypeDef BLE_WAIT_FOR_TX_POOL(void);

#endif
void InitBLEAndSetItToStandby(void);
#endif /* ST_BLUENRG_MS_INCLUDES_BLE_DECODE_CALLBACK_H_ */
