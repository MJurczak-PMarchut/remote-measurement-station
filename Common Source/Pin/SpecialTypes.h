/*
 * SpecialTypes.h
 *
 *  Created on: Dec 31, 2019
 *      Author: Mateusz Jurczak
 */

#ifndef INC_SPECIALTYPES_H_
#define INC_SPECIALTYPES_H_

#include "configuration.h"
#include "BOARD_DEF.h"
#include "hci_const.h"

#ifdef BOARD_SENSORTILE
#include "SensorTile_motion_sensors.h"
#endif

typedef enum  {PS_LP_SLEEP = 0, PS_STOP0 = 1, PS_STOP1 = 2, PS_STOP2 = 3, PS_STANDBY = 4, PS_SHUTDOWN = 5, PS_RESET = 6, PS_DEFAULT, PS_LOW_POWER_RUN, PS_LOW_SPEED, PS_HIGH_SPEED, PS_BURST, PS_DONT_CARE} PowerState;
typedef uint8_t ProcessID ;
typedef enum {ST_RECURRENT, ST_SINGLE} SLOT_TYPE;
typedef enum  {TS_TIME_LEFT, TS_OVERTIME, TS_IN_MARGIN, TS_ERROR} TimingState;
typedef void* (*VoidFuncPointer) (void) ;
typedef uint8_t RunPriority;
typedef enum {BLE_DISC_SERV = 0, BLE_DISC_CHAR, BLE_IDLE, BLE_DISC_DESC, BLE_SCAN, BLE_READ, BLE_CONNECTING, BLE_RECONNECTING, BLE_WAIT_FOR_IND} BLE_State;

typedef enum {COMP_RESULT_OK, COMP_RESULT_NOK, COMP_ERROR} Comp_Result;
#if defined(HAS_BLUETOOTH)

typedef enum {
	DEVICE_LR = 0
#if NoOfDevices > 1
	,DEVICE_PR = 1
#endif
#if NoOfDevices > 2
	,DEVICE_LN = 2
#endif
#if NoOfDevices > 3
	,DEVICE_PN = 3
#endif
} BLE_DEVICE_LOCATION;

typedef enum {
	BLE_SENS_SERVICE = 0,
	BLE_LED_SERVICE = 1,
	BLE_SD_SERVICE = 2,
	BLE_CONF_SERVICE = 3,
	BLE_SERIAL_SERVICE = 4
} BLE_SERVICE_UUID;


typedef enum {
	BLE_COMPARE_SERVICE_UUID = 0,
	BLE_COMPARE_CHAR_UUID = 1
} BLE_COMPARE_UUID_TYPE;

typedef enum {
	BLE_GYRO_CHAR = 0,
	BLE_ACC_CHAR = 1,
	BLE_LED_CONF_CHAR = 2,
	BLE_LED_READ_CHAR = 3,
	BLE_SD_CONF_CHAR = 4,
	BLE_SD_READ_CHAR = 5,
	BLE_GYRO_CONF_CHAR = 6,
	BLE_ACCEL_CONF_CHAR = 7,
	BLE_SERIAL_RD_CHAR = 9,
	BLE_SERIAL_WR_CHAR = 8
} BLE_CHAR_UUID;


typedef union
{
	BLE_SERVICE_UUID	ServiceUUID;
	BLE_CHAR_UUID		CharUUID;
}BLE_UUID_MATCH_DATA;

typedef struct {

	BLE_UUID_MATCH_DATA uBLE_UUID_MATCH_DATA;
	Comp_Result			eCompResult;

}BLE_UUID_MATCH_DATA_STRUCT;


typedef struct {

	uint16_t CharHandle ;
	uint8_t  CharProperties ;
	uint16_t CharValueHandle ;

} CharIDData;

typedef struct {

	uint16_t 	ServiceHandle;
	uint16_t 	EndGroupHandle;


} ServiceIDData;


typedef struct {

	uint16_t 		ConnectionHandle ;
	uint8_t 		BdAddr[6] ;
	uint8_t			BdAddrType;
	uint8_t			ServicesPresent ;
	uint8_t			CharPresent ;
	CharIDData 		sCharIDData[MAX_CHAR_NUM_PER_SERV*MAX_SERV_NUM_PER_DEV];
	ServiceIDData 	sServiceIDData[MAX_SERV_NUM_PER_DEV];

} BLE_DEV_DATA;

typedef enum {
	BLE_CONNCETION_SUCCESFUL = 0,
	BLE_CONNECTION_FAILED
} BLE_CONNECTION_STATUS;

#endif
typedef enum {SCAN, CONNECT, GET_DATA, SYNCHORONIZE, ABORT, HARD_RESET} Keyword;

typedef struct {
	Keyword eKeyword;
	char* 	pcKeyword;
}Token;






typedef enum  {EMPTY = 0, FLOAT, INT, KEYWORD, STRING} DataTypes;

typedef union {
	int iData;
	uint32_t ui32Data;
	float fData;
	char* cData;
	Keyword eData;
} vData;


typedef struct
{
	DataTypes eDataTypes;
	vData 	  vData;
}DataDecode;

typedef struct
{
	uint8_t *pData;
	uint8_t len;
}BLE_READ_DATA_STRUCT;

typedef enum{NO_WKUP = 0, RTC_IT, BLE_IT, GEN_EXTI, SENS_IT, TIMER_IT, SYSTICK_IT} WKUP_REASON;

typedef volatile struct
{
	WKUP_REASON eWkupReason;
	uint8_t u8WkupContext; // used for TIMER_IT to decide freq settings

} WKUP_CONTEXT;

typedef struct {
	uint8_t BufferStart;
	uint8_t BufferEnd;
	uint8_t *pBuffer;
}BufferStruct;

#ifdef BOARD_SENSORTILE
typedef struct
{
  uint32_t ms_counter;
  float pressure;
  float humidity;
  float temperature;
  BSP_MOTION_SENSOR_Axes_t acc;
  BSP_MOTION_SENSOR_Axes_t gyro;
  BSP_MOTION_SENSOR_Axes_t mag;
} T_SensorsData;



#if defined(BOARD_SENSORTILE)
	typedef enum{DEFAULT_CLK, NO_PLL_48MHz_CLK, NO_PLL_32MHz_CLK, NO_PLL_24MHz_CLK, NO_PLL_16MHz_CLK, NO_PLL_8MHz_CLK, NO_PLL_4MHz_CLK, NO_PLL_2MHz_CLK, NO_PLL_1MHz_CLK, NO_PLL_800kHz_CLK, NO_PLL_400kHz_CLK, NO_PLL_200kHz_CLK, NO_PLL_100kHz_CLK} CLK_SPEED;
#endif

#endif
#endif /* INC_SPECIALTYPES_H_ */
