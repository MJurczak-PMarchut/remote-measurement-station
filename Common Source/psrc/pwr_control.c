/*
 * pwr_control.c
 *
 *  Created on: 26 gru 2020
 *      Author: Mateusz Jurczak
 */

#include "pwr_control.h"
#include "clk_speed_definitions.h"
//#include "stm32l4xx_hal_rtc.h"
//#include "stm32l4xx_hal_rtc_ex.h"
//#include "rtc_util.h"
extern volatile uint32_t HCI_ProcessEvent;

#ifdef BOARD_SENSORTILE
WKUP_CONTEXT *psWkupContext;

RTC_HandleTypeDef *sphrtc;

void ResumeFromSleepModes(PowerState ePowerState);
void DisableLPRun(void);
void EnableLPRun(void);

static CLK_SPEED __attribute((section(".noinit"))) egclk;

void DisableGPIOs(void)
{
//	   HAL_PWREx_EnablePullUpPullDownConfig();
//	   HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A, GPIO_PIN_All);
//	   HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_C, GPIO_PIN_All);

//	HAL_PWREx_DisablePullUpPullDownConfig();
//	   GPIO_InitTypeDef GPIO_InitStructure1 = {0};
	   GPIO_InitTypeDef GPIO_InitStructure = {0};
	   GPIO_InitStructure.Pin = GPIO_PIN_All;
	   GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	   GPIO_InitStructure.Pull = GPIO_NOPULL;

//	   GPIO_InitStructure1.Pin = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 |\
//			   GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 |\
//			   GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_15);
//	   GPIO_InitStructure1.Mode = GPIO_MODE_ANALOG;
//	   GPIO_InitStructure1.Pull = GPIO_NOPULL;

	   // GPIO_B and GPIO_C are disabled
//	   HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
//	   __HAL_RCC_GPIOA_CLK_DISABLE();

	   HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	   __HAL_RCC_GPIOB_CLK_DISABLE();
//	   HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
//	   __HAL_RCC_GPIOC_CLK_DISABLE();
	   HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
	   __HAL_RCC_GPIOG_CLK_DISABLE();
	   GPIO_InitStructure.Pin = GPIO_PIN_All;
	   GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	   GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	   HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);
	   __HAL_RCC_GPIOH_CLK_DISABLE();

}
void SetNextClkPreset(CLK_SPEED eclk)
{
	egclk = eclk;
}


void SetHrtcPointer(RTC_HandleTypeDef *phrtc){
	sphrtc = phrtc;
}



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
	No_PLL_2MHz();
}


void SetClkPreset(CLK_SPEED eclk)
{
	switch(eclk)
	{
		case DEFAULT_CLK:
		{
			DisableLPRun();
//			Default_CLK();
			  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
			  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
			  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
			  __HAL_RCC_PWR_CLK_ENABLE();
				RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
							| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
			RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
			RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
			RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
			RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
			if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
				while (1);
			}
			  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_MSI;
			  RCC_OscInitStruct.MSIState            = RCC_MSI_ON;
			  RCC_OscInitStruct.HSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
			  RCC_OscInitStruct.MSIClockRange       = RCC_MSIRANGE_11;
			  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
			  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_MSI;
			  RCC_OscInitStruct.PLL.PLLM            = 6;
			  RCC_OscInitStruct.PLL.PLLN            = 40;
			  RCC_OscInitStruct.PLL.PLLP            = 7;
			  RCC_OscInitStruct.PLL.PLLQ            = 4;
			  RCC_OscInitStruct.PLL.PLLR            = 4;
			  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
			  {
			    while(1);
			  }
			  HAL_RCCEx_EnableMSIPLLMode();
			  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
			  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_MSI;
			  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
			  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
			  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
			  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
			  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
			  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
			  {
			    while(1);
			  }
			break;
		}
		case NO_PLL_2MHz_CLK:
		{
			EnableLPRun();
			No_PLL_2MHz();
			break;
		}
		case NO_PLL_1MHz_CLK:
		{
			EnableLPRun();
			No_PLL_1MHz();
			break;
		}
		case NO_PLL_400kHz_CLK:
		{
			EnableLPRun();
			No_PLL_400kHz();
			break;
		}
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
	CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
}


//void EnterStopMode(uint8_t stop_mode)
//{
//	if(stop_mode)
//}
/*
 * Wake up management functions
 */


//HAL_PWREx_EnterSTOP1Mode(uint8_t STOPEntry)

void SetWkupContextPointer(WKUP_CONTEXT *psWkupPointer){
	psWkupContext = psWkupPointer;
}

void SleepAndWaitForWkup(void){
	/*
	 * We come here after every period of run mode, first we stop and wait for interrupt
	 */
	HAL_SuspendTick();
	__WFI();
	HAL_ResumeTick();
}

//	HAL_PWREx_EnableSRAM2ContentRetention();
//	DisableGPIOs();
//	HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
//	HAL_DeInit();
//	HAL_PWR_EnterSTANDBYMode();

void EnterLowPoweMode(PowerState ePowerState)
{
	extern uint8_t BLE_STANDBY;
	psWkupContext->eWkupReason = NO_WKUP;

//	DisableGPIOs();
	 __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	switch(ePowerState)
	{
		case PS_LP_SLEEP:
			EnableLPRun();
#if defined(RTC_WKUP_INTERNAL)
			HAL_RTCEx_SetWakeUpTimer_IT(sphrtc, RTC_WKUP_COUNTER, RTC_WKUP_CLK_DIV);
#endif
			SleepAndWaitForWkup();
			break;
		case PS_STOP0:
			TOGGLE_EVENT_PIN();
			HAL_Delay(10);
			TOGGLE_EVENT_PIN();
			HAL_PWREx_DisableLowPowerRunMode();
			HAL_SuspendTick();
#if defined(RTC_WKUP_INTERNAL)
			HAL_RTCEx_SetWakeUpTimer_IT(sphrtc, RTC_WKUP_COUNTER, RTC_WKUP_CLK_DIV);
#endif
			HAL_PWREx_EnterSTOP0Mode(PWR_STOPENTRY_WFI);
			break; //only decorative
		case PS_STOP1:
			TOGGLE_EVENT_PIN();
			HAL_Delay(15);
			TOGGLE_EVENT_PIN();
			HAL_SuspendTick();
#if defined(RTC_WKUP_INTERNAL)
			HAL_RTCEx_SetWakeUpTimer_IT(sphrtc, RTC_WKUP_COUNTER, RTC_WKUP_CLK_DIV);
#endif
			HAL_PWREx_EnterSTOP1Mode(PWR_STOPENTRY_WFI);
			break;
		case PS_STOP2:
			EnableLPRun();
			TOGGLE_EVENT_PIN();
			HAL_Delay(20);
			TOGGLE_EVENT_PIN();
			HAL_SuspendTick();
			HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_H, GPIO_PIN_All);
		    HAL_PWREx_EnablePullUpPullDownConfig();
#if defined(RTC_WKUP_INTERNAL)
			HAL_RTCEx_SetWakeUpTimer_IT(sphrtc, RTC_WKUP_COUNTER, RTC_WKUP_CLK_DIV);
#endif
			HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
			break;
		case PS_STANDBY:
			EnableLPRun();
			HAL_PWREx_EnableSRAM2ContentRetention();
			HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_H, GPIO_PIN_All);
		    HAL_PWREx_EnablePullUpPullDownConfig();
			HAL_SuspendTick();
#if defined(RTC_WKUP_INTERNAL)
			HAL_RTCEx_SetWakeUpTimer_IT(sphrtc, RTC_WKUP_COUNTER, RTC_WKUP_CLK_DIV);
#endif
			HAL_PWR_EnterSTANDBYMode();
			break;
		case PS_SHUTDOWN:
//			EnableLPRun();
			HAL_PWREx_EnableSRAM2ContentRetention();
			HAL_SuspendTick();
			HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_H, GPIO_PIN_All);
		    HAL_PWREx_EnablePullUpPullDownConfig();
#if defined(RTC_WKUP_INTERNAL)
			HAL_RTCEx_SetWakeUpTimer_IT(sphrtc, RTC_WKUP_COUNTER, RTC_WKUP_CLK_DIV);
#endif
			HAL_PWREx_EnterSHUTDOWNMode();
			break;
		default:
			break;

	}
#if defined(RTC_WKUP_INTERNAL)
//	__HAL_RCC_RTC_ENABLE();
//	HAL_PWR_EnableBkUpAccess();
	HAL_RTCEx_DeactivateWakeUpTimer(sphrtc);
//	HAL_PWR_DisableBkUpAccess();
#endif
	HAL_ResumeTick();
	ResumeFromSleepModes(ePowerState);
}



void ResumeFromSleepModes(PowerState ePowerState)
{

	/*
	 * Now we need to decide what to do
	 * We process what was the cause of the wake up
	 * and we decide at what frequency we will work
	 */
	GPIO_InitTypeDef GPIO_InitStructure = {0};

	//ENABLE_EVENT_PORT_CLOCK();

	GPIO_InitStructure.Pin = __EVT_GPIO_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	InitEvtGpioClock();
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	switch(psWkupContext->eWkupReason)
	{
	case TIMER_IT:
		//What to do when woke up from timer
		break;
	case RTC_IT:
		//What to do if woken up by rtc
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
		TOGGLE_EVENT_PIN();
    	SetClkPreset(egclk);
    	ClkDependentInit();
    	TOGGLE_EVENT_PIN();
		break;
	case GEN_EXTI:
		break;
	case SENS_IT:
		break;
	default:
	    if(HCI_ProcessEvent)
	    {
	    	psWkupContext->eWkupReason = BLE_IT;
	    	SetClkPreset(egclk);
//	    	ClkDependentInit();
	    	//Now we hand off control to the calling function
	    	return;
	    }
		break;
	}

}
#endif

