/*
 * timings.c
 *
 *  Created on: 2 sty 2021
 *      Author: Mateusz Jurczak
 */

#include "timings.h"
uint32_t uTickCount = 0;
TIM_HandleTypeDef *htim_base;
void InitTimer2(TIM_HandleTypeDef *htim)
{

	  uint32_t uCountUpPrescaler = 0;

	  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	  TIM_MasterConfigTypeDef sMasterConfig = {0};
	  TIM_OC_InitTypeDef sConfigOC = {0};

	  __HAL_RCC_TIM2_CLK_ENABLE();
	  htim_base = htim;
	  uCountUpPrescaler = (HAL_RCC_GetPCLK1Freq()/TIM2_FREQUENCY_FOR_TIMING) - 1;

	  htim->Instance = TIM2;
	  htim->Init.Prescaler = uCountUpPrescaler;
	  htim->Init.CounterMode = TIM_COUNTERMODE_UP;
#if defined(FREQUENCY_DEFINED_TIM2_TIMEBASE)
	  htim->Init.Period = 1;
#elif defined(TIM2_FREE_RUN)
	  htim->Init.Period = 0xffffffff;
#else
#warning("Tim period not set in InitTimer2")
#endif
	  htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	  if (HAL_TIM_Base_Init(htim) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	  if (HAL_TIM_ConfigClockSource(htim, &sClockSourceConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  if (HAL_TIM_OC_Init(htim) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sConfigOC.OCMode = TIM_OCMODE_TIMING;
	  sConfigOC.Pulse = 0;
	  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	  if (HAL_TIM_OC_ConfigChannel(htim, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	  {
	    Error_Handler();
	  }
//	  __HAL_TIM_ENABLE_OCxPRELOAD(htim, TIM_CHANNEL_1);
#ifdef FREQUENCY_DEFINED_TIM2_TIMEBASE
	  if(HAL_RCC_GetSysClockFreq() > 2000000)
	  {
		  /*
		   * We will not use interrupts for those lower frequencies as it takes a lot of processor time
		   * When checking timing critical tasks we should be checking the timer counter state
		   */
		  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
		  HAL_NVIC_EnableIRQ(TIM2_IRQn);
		  HAL_TIM_Base_Start_IT(htim);
	  }
	  else{
		  HAL_TIM_Base_Start(htim);
	  }
#endif
#ifdef TIM2_FREE_RUN
	  HAL_TIM_Base_Start(htim);
#endif
}
#ifndef TIM2_FREE_RUN
void InitTim2Tick(void)
{

	uTickCount = 0;
}

void TimingIncTick(void)
{
	uTickCount++;
}
#endif
void ResetTimCounter(void)
{
	__HAL_TIM_SET_COUNTER(htim_base, 0);
}
void StartTim(void)
{
	/*
	 * This function should not be used in timing critical functions, because if we ommit optimalization this will require at least a jump and a ret
	 */
	__HAL_TIM_ENABLE(htim_base);
}

void StopTim(void)
{
	/*
	 * This function should not be used in timing critical functions, because if we ommit optimalization this will require at least at least a jump and a ret
	 */
	__HAL_TIM_DISABLE(htim_base);
}
uint32_t GetTim2Tick(void)
{
#ifndef TIM2_FREE_RUN
	return uTickCount;
#else
	return __HAL_TIM_GET_COUNTER(htim_base);
#endif
}

