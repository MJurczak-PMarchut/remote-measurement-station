/*
 * configuration.h
 *
 *  Created on: Jan 1, 2020
 *      Author: Mateusz Jurczak
 */
#pragma once
#ifndef INC_CONFIGURATION_H_
#define INC_CONFIGURATION_H_

//#include "SpecialTypes.h"

/*File containing board definition*/
#include "BOARD_DEF.h"
/*Debugging options for algorithms (enables stop-whiles for unexpected cases)*/

//#define DEBUG_LW_AL
//#define DEBUG_BL
//#define DEBUG_BL_INIT


/* Number of slots for scheduler (DEFAULT 10)
 * Note: MAX number of slots defined in the respective c file (DEFAULT 80) */

#define NoOfFunctionSlots 10

/* Timing margin for power state in ms (uint) */

#define TIMING_MARGIN 20

#define TIMING_MARGIN_LP_RUN 30
#define TIMING_MARGIN_RUN 10

/*timing margin for recurrent procs, can be changed during the course of working*/

#define TIMING_MARGIN_RECURRENT 5

/*if you want to prioritize lower priority values over respective power state then disable this (not implemented)*/

#define POWER_FIRST

/* If priority is lower than lowest task is ignored (can be stuck, used to postpone a task)*/

#define LOWEST_PRIORITY 15

#define HIGHEST_PRIORITY 0

/*UART Options*/

#define MAX_TOKEN_NO 4
#define RX_BUFFER_SIZE 15
#define TX_BUFFER_SIZE 250
#define UART_TERM_CHAR '\n'

#define BAUD_RATE 115200

//#define FREQUENCY_DEFINED_TIM2_TIMEBASE
#define TIM2_FREE_RUN

#define TIM2_FREQUENCY_FOR_TIMING 1000000

#ifndef STM32_UUID
#ifdef BOARD_SENSORTILE
		#define STM32_UUID ((uint32_t *)0x1FFF7590)
#endif
#ifdef BOARD_N64_F4
		#define STM32_UUID ((uint32_t *)0x1FFF7A10)
#endif
//		#define UNDEF_STM32_UUID
#endif

#ifndef STLBLE_VERSION_MAJOR
	#define STLBLE_VERSION_MAJOR '2'
//	#define UNDEF_STLBLE_VERSION_MAJOR
#endif

#ifndef STLBLE_VERSION_MINOR
	#define STLBLE_VERSION_MINOR '1'
//	#define UNDEF_STLBLE_VERSION_MINOR
#endif

/*BLUETOOTH options*/

#define HAS_BLUETOOTH

#ifdef HAS_BLUETOOTH


	#define BLE_PIN 123456


	#define BDADDR_SIZE        6

	#define RAND_BLE_ADDRESS

	#ifndef RAND_BLE_ADDRESS
	#define BLE_BDADDR 0x21, 0x45, 0x21, 0x74, 0x65, 0x63
	#endif

	#define DEFAULT_SERV_DATA_LEN 0x6

	#define SYNC_LEN 1
	#define WRITE_DATA 0x05
	#define SEND_DATA_TO_NULL 0x42

	#define UUID_LENGTH 16


	#ifdef BOARD_N64_F4
		/*maximum number of devices we are able to discover, default 15, numbers greater than 20 not accepted*/
		#define MAX_NO_OF_DISCOVERABLE_DEVICES 10
		#define MAX_CHAR_NO_TO_READ 8
		#define MAX_LEN_OF_MULT_CHAR_READ (2*3*4+5)
		#define STATION_NAME "STACJA_POM"
		/*number of devices to connect to (default 4, numbers greater than 4 not accepted) */
		#define NoOfDevices 4

		/*if any service should be mandatory for a device uncomment respective line (default: all mandatory)*/
		#define SENS_SERVICE_MANDATORY
		#define SD_SERVICE_MANDATORY
		#define LED_SERVICE_MANDATORY


	#endif

	#define ADDR_TYPE STATIC_RANDOM_ADDR


	#define MAX_BLE_NAME_LENGTH 30

	#define MAX_CHAR_NUM_PER_SERV 2
	#define MAX_SERV_NUM_PER_DEV 5
	#define NO_OF_DEFAULT_SERVICES 2
	#define Sensortile_Datalog_SD
/*If data should be taken to sd or sent over bluetooth (compile priority, can be changed after connection is established) */

#define CUSTOM_BLE_SERVICE_UUIDs

#ifdef CUSTOM_BLE_SERVICE_UUIDs


	#define CUSTOM_BLE_SENS_SERVICE_UUID  	{0,0,0,0,0,0,0,0,0x15,0x2B,0x54,0x15,0xB4,0xA2,0x74,0xDA}
	#define	CUSTOM_BLE_LED_SERVICE_UUID  	{0,0,0,0,0,0,0,0,0x25,0x2A,0x52,0x05,0x45,0xA2,0x32,0xFA}
	#define	CUSTOM_BLE_CONFIG_SERVICE_UUID  {0,0,0,0,0,0,0,0,0x2A,0xCA,0xEA,0xBC,0x12,0x02,0x34,0xB6}
	#define	CUSTOM_BLE_SD_SERVICE_UUID  	{0,0,0,0,0,0,0,0,0x45,0x2C,0x57,0xFF,0x45,0x1A,0x78,0xC6}
	#define CUSTOM_BLE_SERIAL_SERVICE_UUID  {0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x01,0x00,0x40,0x6e}
#endif


#define CUSTOM_BLE_CHARACTERISCTIC_UUIDs

#ifdef CUSTOM_BLE_CHARACTERISCTIC_UUIDs



	#define	CUSTOM_BLE_GYRO_CHAR_UUID  		{0,0,0,0,0,0,0,0,0xDA,0xCF,0x48,0x11,0x05,0xE4,0x91,0x28}
	#define	CUSTOM_BLE_ACC_CHAR_UUID  		{0,0,0,0,0,0,0,0,0x40,0x36,0x14,0x97,0x65,0xCD,0x80,0x45}
	#define	CUSTOM_BLE_LED_CONF_CHAR_UUID  	{0,0,0,0,0,0,0,0,0xAD,0x27,0x02,0x49,0xA8,0x92,0x87,0xCA}
	#define	CUSTOM_BLE_LED_READ_CHAR_UUID  	{0,0,0,0,0,0,0,0,0x09,0xFC,0x35,0x42,0xAF,0x14,0x98,0xF6}
	#define	CUSTOM_BLE_SD_CONF_CHAR_UUID  	{0,0,0,0,0,0,0,0,0xF4,0x82,0x95,0xF2,0xCA,0x27,0x28,0xA6}
	#define	CUSTOM_BLE_SD_READ_CHAR_UUID  	{0,0,0,0,0,0,0,0,0xFB,0x54,0x32,0x94,0xCF,0x41,0x08,0x86}
	#define CUSTOM_BLE_GYRO_CONF_CHAR_UUID 	{0,0,0,0,0,0,0,0,0xFA,0x64,0x12,0xFA,0x87,0x55,0xAA,0xE3}
	#define CUSTOM_BLE_ACCEL_CONF_CHAR_UUID {0,0,0,0,0,0,0,0,0xFA,0x74,0x12,0xFA,0x87,0x65,0xAA,0xA3}
	#define CUSTOM_BLE_SERIAL_RD_CHAR_UUID		{0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x02,0x00,0x40,0x6e}
	#define CUSTOM_BLE_SERIAL_WR_CHAR_UUID		{0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x03,0x00,0x40,0x6e}

#endif


	#ifdef BOARD_SENSORTILE
		/*Name for bl periph, needs recompiling for each, better way needed  TODO*/
		#define ST_BL_NAME "SENS0"
		/*which characteristics should be visible for data transfer*/
		#define GYRO_CHAR
		#define ACCEL_CHAR
		#define BL_PIN_CODE 854124
	#endif
#endif
#if defined(BOARD_SENSORTILE)
/*
 * RTC settings
 */
#define USE_RTC

#if defined(USE_RTC)

	#define RTC_WKUP_INTERNAL  //RTC uses EXTI line 20 for wake up

	/* RTC clock prescaler, should be one of below
	 * RTC_WAKEUPCLOCK_RTCCLK_DIV16
	 * RTC_WAKEUPCLOCK_RTCCLK_DIV8
	 * RTC_WAKEUPCLOCK_RTCCLK_DIV4
	 * RTC_WAKEUPCLOCK_RTCCLK_DIV2
	 */
	#define RTC_WKUP_CLK_DIV RTC_WAKEUPCLOCK_RTCCLK_DIV16

/*
 * RTC CLOCK source selection, should be one of the below
 * RCC_RTCCLKSOURCE_LSI - 32kHz
 * RCC_RTCCLKSOURCE_LSE - 32.768 kHz
 */
	#define RTC_CLK_SOURCE RCC_RTCCLKSOURCE_LSE

/*
 * RTC counter
 * wake up period is = RTC_WKUP_COUNTER  * (RTC_WKUP_CLK_DIV / RTC_CLK_SOURCE)
 */
	#define RTC_WKUP_COUNTER 2048

#endif

#endif


#endif /* INC_CONFIGURATION_H_ */
