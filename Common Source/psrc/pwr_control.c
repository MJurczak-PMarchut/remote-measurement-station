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


WKUP_CONTEXT *psWkupContext;

RTC_HandleTypeDef *sphrtc;

void ResumeFromSleepModes(void);
void DisableLPRun(void);


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
			Default_CLK();
			break;
		}
		case NO_PLL_2MHz_CLK:
		{
			No_PLL_2MHz();
			break;
		}
		case NO_PLL_1MHz_CLK:
		{
			No_PLL_1MHz();
			break;
		}
		case NO_PLL_400kHz_CLK:
		{
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
//	uint32_t a,b;
	PrepareForSleep();
//	hci_tl_lowlevel_init();
#if defined(RTC_WKUP_INTERNAL)
	HAL_RTCEx_SetWakeUpTimer_IT(sphrtc, RTC_WKUP_CONTER, RTC_WKUP_CLK_DIV);
#endif
	HAL_SuspendTick();

	__WFI();

//	while(HAL_RTCEx_PollForWakeUpTimerEvent(sphrtc,0xFFFFF) != HAL_OK)
//	{
//
//	}

	HAL_ResumeTick();
#if defined(RTC_WKUP_INTERNAL)
	HAL_RTCEx_DeactivateWakeUpTimer(sphrtc);
#endif
//	a =HAL_GetTick();
	ResumeFromSleepModes();
//	b=HAL_GetTick();
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


