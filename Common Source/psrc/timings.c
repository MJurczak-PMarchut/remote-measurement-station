/*
 * timings.c
 *
 *  Created on: 2 sty 2021
 *      Author: Mateusz Jurczak
 */

//#include "timings.h"
//void InitTimer(void)
//{
//	  /* USER CODE BEGIN TIM2_Init 0 */
//
//	  /* USER CODE END TIM2_Init 0 */
//
//	  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
//	  TIM_MasterConfigTypeDef sMasterConfig = {0};
//	  TIM_OC_InitTypeDef sConfigOC = {0};
//
//	  /* USER CODE BEGIN TIM2_Init 1 */
//
//	  /* USER CODE END TIM2_Init 1 */
//	  htim2.Instance = TIM2;
//	  htim2.Init.Prescaler = 800;
//	  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
//	  htim2.Init.Period = 10;
//	  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//	  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
//	  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
//	  {
//	    Error_Handler();
//	  }
//	  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
//	  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
//	  {
//	    Error_Handler();
//	  }
//	  if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
//	  {
//	    Error_Handler();
//	  }
//	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//	  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
//	  {
//	    Error_Handler();
//	  }
//	  sConfigOC.OCMode = TIM_OCMODE_TIMING;
//	  sConfigOC.Pulse = 10;
//	  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
//	  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
//	  if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
//	  {
//	    Error_Handler();
//	  }
//	  __HAL_TIM_ENABLE_OCxPRELOAD(&htim2, TIM_CHANNEL_1);
//	  /* USER CODE BEGIN TIM2_Init 2 */
//
//	  /* USER CODE END TIM2_Init 2 */
//
//	  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
//	  HAL_NVIC_EnableIRQ(TIM2_IRQn);
//	  HAL_TIM_Base_Start_IT(&TimCCHandle);
//}
//
//
//


