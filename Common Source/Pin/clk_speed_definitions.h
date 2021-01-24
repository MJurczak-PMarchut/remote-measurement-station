/*
 * clk_speed_definitions.h
 *
 *  Created on: 1 sty 2020
 *      Author: OMEN
 */

#ifndef INC_CLK_SPEED_DEFINITIONS_H_
#define INC_CLK_SPEED_DEFINITIONS_H_

#include "configuration.h"

#ifdef BOARD_SENSORTILE
#endif


#ifdef BOARD_N64_F4
#define DEFAULT_CONF do {\
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};\
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};\
   __HAL_RCC_PWR_CLK_ENABLE();\
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);\
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;\
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;\
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;\
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;\
  RCC_OscInitStruct.PLL.PLLM = 8;\
  RCC_OscInitStruct.PLL.PLLN = 336;\
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;\
  RCC_OscInitStruct.PLL.PLLQ = 7;\
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)\
  {\
    Error_Handler();\
  }\
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK\
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;\
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;\
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;\
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;\
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;\
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)\
  {\
    Error_Handler();\
  }\
  HAL_RCC_EnableCSS();\
}while(0)

#define DEFAULT_HALF_SPEED do {\
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};\
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};\
  __HAL_RCC_PWR_CLK_ENABLE();\
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);\
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;\
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;\
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;\
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;\
  RCC_OscInitStruct.PLL.PLLM = 4;\
  RCC_OscInitStruct.PLL.PLLN = 84;\
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;\
  RCC_OscInitStruct.PLL.PLLQ = 7;\
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)\
  {\
    Error_Handler();\
  }\
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK\
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;\
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;\
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;\
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;\
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;\
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)\
  {\
    Error_Handler();\
  }\
}while(0)

#define DEFAULT_NO_PLL do {\
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};\
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};\
  __HAL_RCC_PWR_CLK_ENABLE();\
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);\
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;\
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;\
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;\
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)\
  {\
    Error_Handler();\
  }\
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK\
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;\
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;\
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;\
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;\
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;\
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)\
  {\
    Error_Handler();\
  }\
}\
while(0)

#define DEFAULT_NO_PLL_VS1 do {\
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};\
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};\
  __HAL_RCC_PWR_CLK_ENABLE();\
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);\
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;\
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;\
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;\
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)\
  {\
    Error_Handler();\
  }\
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK\
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;\
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;\
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;\
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;\
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;\
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)\
  {\
    Error_Handler();\
  }\
}\
while(0)

#define DEFAULT_NO_PLL_VS1_HALF_SPEED do {\
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};\
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};\
  __HAL_RCC_PWR_CLK_ENABLE();\
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);\
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;\
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;\
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;\
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)\
  {\
    Error_Handler();\
  }\
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK\
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;\
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;\
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;\
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;\
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;\
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)\
  {\
    Error_Handler();\
  }\
}\
while(0)

#define DEFAULT_NO_PLL_VS1_HSI do{\
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};\
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};\
  __HAL_RCC_PWR_CLK_ENABLE();\
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);\
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;\
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;\
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;\
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;\
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)\
  {\
    Error_Handler();\
  }\
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK\
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;\
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;\
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;\
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;\
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;\
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)\
  {\
    Error_Handler();\
  }\
}\
while(0)

#endif
#ifdef BOARD_SENSORTILE
/*
 * Power states definitions
 */



#define Default_CLK() do{\
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};\
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};\
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};\
  __HAL_RCC_PWR_CLK_ENABLE();\
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_MSI;\
  RCC_OscInitStruct.MSIState            = RCC_MSI_ON;\
  RCC_OscInitStruct.HSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;\
  RCC_OscInitStruct.MSIClockRange       = RCC_MSIRANGE_11;\
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;\
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_MSI;\
  RCC_OscInitStruct.PLL.PLLM            = 6;\
  RCC_OscInitStruct.PLL.PLLN            = 40;\
  RCC_OscInitStruct.PLL.PLLP            = 7;\
  RCC_OscInitStruct.PLL.PLLQ            = 4;\
  RCC_OscInitStruct.PLL.PLLR            = 4;\
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)\
  {\
    while(1);\
  }\
  HAL_RCCEx_EnableMSIPLLMode();\
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;\
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_MSI;\
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);\
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);\
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;\
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;\
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;\
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;\
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)\
  {\
    while(1);\
  }\
}while(0)


#define No_PLL_2MHz() do{\
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };\
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };\
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK\
				| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);\
		RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;\
		RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;\
		RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;\
		RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;\
		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {\
			while (1);\
		}\
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;\
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;\
	RCC_OscInitStruct.HSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;\
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;\
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;\
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_NONE;\
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {\
		while (1);\
	}\
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK\
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);\
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;\
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;\
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;\
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;\
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {\
		while (1);\
	}\
}while(0)
#define No_PLL_1MHz() do{\
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };\
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };\
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};\
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK\
				| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);\
		RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;\
		RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;\
		RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;\
		RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;\
		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {\
			while (1);\
		}\
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;\
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;\
	RCC_OscInitStruct.HSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;\
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_3;\
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;\
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_NONE;\
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {\
		while (1);\
	}\
	HAL_RCCEx_EnableMSIPLLMode();\
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;\
	PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_NONE;\
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);\
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK\
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);\
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;\
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;\
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;\
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;\
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {\
		while (1);\
	}\
}while(0)

#define No_PLL_400kHz() do{\
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };\
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };\
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};\
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK\
				| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);\
		RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;\
		RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;\
		RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;\
		RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;\
		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {\
			while (1);\
		}\
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;\
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;\
	RCC_OscInitStruct.HSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;\
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_2;\
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;\
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_NONE;\
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {\
		while (1);\
	}\
	HAL_RCCEx_EnableMSIPLLMode();\
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;\
	PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_NONE;\
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);\
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK\
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);\
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;\
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;\
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;\
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;\
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {\
		while (1);\
	}\
}while(0)
#endif

#endif /* INC_CLK_SPEED_DEFINITIONS_H_ */
