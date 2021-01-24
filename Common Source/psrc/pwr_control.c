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

static CLK_SPEED egclk = DEFAULT_CLK;

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
	   HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
//	   __HAL_RCC_GPIOA_CLK_DISABLE();

	   HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	   __HAL_RCC_GPIOB_CLK_DISABLE();
	   HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	   __HAL_RCC_GPIOC_CLK_DISABLE();
	   HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
	   __HAL_RCC_GPIOG_CLK_DISABLE();
	   GPIO_InitStructure.Pin = GPIO_PIN_All;
	   GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	   GPIO_InitStructure.Pull = GPIO_PULLUP;
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
			Default_CLK();
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
	psWkupContext->eWkupReason = NO_WKUP;
#if defined(RTC_WKUP_INTERNAL)
	HAL_RTCEx_SetWakeUpTimer_IT(sphrtc, RTC_WKUP_COUNTER, RTC_WKUP_CLK_DIV);
#endif
	switch(ePowerState)
	{
		case PS_LP_SLEEP:
			EnableLPRun();
			TOGGLE_EVENT_PIN();
			HAL_Delay(10);
			SleepAndWaitForWkup();
			break;
		case PS_RESET:
			TOGGLE_EVENT_PIN();
			HAL_Delay(15);
			HAL_NVIC_SystemReset();
			break; //only decorative
		case PS_STOP0:
			TOGGLE_EVENT_PIN();
			HAL_Delay(20);
			HAL_PWREx_EnterSTOP0Mode(PWR_STOPENTRY_WFI);
			break;
		case PS_STOP1:
			EnableLPRun();
			TOGGLE_EVENT_PIN();
			HAL_Delay(25);
			HAL_PWREx_EnterSTOP1Mode(PWR_STOPENTRY_WFI);
			break;
		case PS_STOP2:
			EnableLPRun();
			TOGGLE_EVENT_PIN();
			HAL_Delay(30);
			HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
			break;
		default:
			break;
	}
#if defined(RTC_WKUP_INTERNAL)
	HAL_RTCEx_DeactivateWakeUpTimer(sphrtc);
#endif
	ResumeFromSleepModes(ePowerState);
}



void ResumeFromSleepModes(PowerState ePowerState)
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
    	SetClkPreset(egclk);
    	ClkDependentInit();
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
	    	ClkDependentInit();
	    	//Now we hand off control to the calling function
	    	return;
	    }
		break;
	}

}
#endif

