/*
 * pwr_control.c
 *
 *  Created on: 26 gru 2020
 *      Author: Mateusz Jurczak
 */

#include "pwr_control.h"

HAL_StatusTypeDef CheckPowerLewerPrerequisite(PowerState ePowerState)
{
	uint8_t state;
	switch(ePowerState){
	case PS_DONT_CARE:
		//There are no prerequisites for "don't care", we should know what state we want to be in
		return HAL_ERROR;
		break;
	case PS_LOW_POWER_RUN:
		//We need to make sure that sysclk is less than 2M
		if(HAL_RCC_GetSysClockFreq() <= 2000000){
			return HAL_OK;
		}
		else{
			return HAL_ERROR;
		}
		break;
	}
	return HAL_ERROR;
}

void SetSysclk2MHz(void)
 {
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	//2MHz MSI
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_4;
	//We turn off the PLL
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_NONE;


	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		while (1);
	}

	/* Enable MSI Auto-calibration through LSE */
	HAL_RCCEx_EnableMSIPLLMode();

	//Disable USB
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
	PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_NONE;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

	/* Select MSI as system clock source and configure the HCLK, PCLK1 and PCLK2
	 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		while (1);
	}
}


HAL_StatusTypeDef Prepare_for_LPRun()
{
	//We must lower the sysclk to max 2MHz
	if(HAL_RCC_GetSysClockFreq() > 2000000){
		SetSysclk2MHz();
	}
	//We can also lower it further
	//TODO Prepare presets for other available frequencies

	//All clock-dependent peripherals need to be reinitialized
//	ClkDependentInit();


	return HAL_OK;

}
