/*
 * pwr_control.c
 *
 *  Created on: 26 gru 2020
 *      Author: Mateusz Jurczak
 */

#include "pwr_control.h"
extern volatile uint32_t HCI_ProcessEvent;

WKUP_CONTEXT *psWkupContext;

void ResumeFromSleepModes(void);

HAL_StatusTypeDef CheckPowerLevelPrerequisites(PowerState ePowerState)
{
//	uint8_t state;
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
	default:
		return HAL_ERROR;
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



HAL_StatusTypeDef Prepare_for_LPRun(void)
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

void EnableLPRun(void)
{
	if(CheckPowerLevelPrerequisites(PS_LOW_POWER_RUN) != HAL_OK)
	{
		Prepare_for_LPRun();
	}
	HAL_PWREx_EnableLowPowerRunMode();
}

void DisableLPRun(void)
{
	HAL_PWREx_DisableLowPowerRunMode();
}

void PrepareForSleep(void)
{
	Prepare_for_LPRun();
//	HAL_PWR_EnterSLEEPMode(Regulator, SLEEPEntry)
	//We must be in LPRun mode, clock below2MHz
    if (HAL_IS_BIT_SET(PWR->SR2, PWR_SR2_REGLPF) == RESET)
    {
      HAL_PWREx_EnableLowPowerRunMode();
    }
	CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
}


/*
 * Wake up management functions
 */


void SetWkupContextPointer(WKUP_CONTEXT *psWkupPointer){
	psWkupContext = psWkupPointer;
}

void SleepAndWaitForWkup(void){
	/*
	 * We come here after every period of run mode, first we stop and wait for interrupt
	 */
	PrepareForSleep();
	__WFI();
	ResumeFromSleepModes();
	//Then we set clocks up depending on context
}

void ResumeFromSleepModes(void)
{

	/*
	 * Now we need to decide what to do
	 * We process what was the cause of the wake up
	 * and we decide at what frequency we will work
	 */
	switch(psWkupContext->eWkupReason)
	{
	case NO_WKUP:
		// There is no known reason for the wake up, we need to check it
		break;
	case TIMER_IT:
		//What to do when woke up from timer
		break;
	case RTC_IT:
		//What to do if woken up by rtc
		break;
	case GEN_EXTI:
		break;
	case SENS_IT:
		break;
	default:
	    if(HCI_ProcessEvent)
	    {
	    	psWkupContext->eWkupReason= BLE_IT;
//	    	SetSysclk2MHz(); //We can't really get lower, there are problems with spi to ble chip below that
	    	EnableLPRun();
	    	ClkDependentInit();
	    	//Now we hand off control to the calling function
	    	return;
	    }
		break;
	}

}
