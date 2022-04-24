/*
 *
 */

/* Includes ------------------------------------------------------------------*/
//#define HAL_RTC_MODULE_ENABLED
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include "TargetFeatures.h"
#include "main.h"
#include "sensor_service.h"
#include "bluenrg_utils.h"
#include "ble_decode_callback.h"
#include "station_specific.h"
#include "pwr_control.h"
#include "test_payloads.h"
#include "timings.h"
#include "stm32l4xx_hal_rtc.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Imported Variables -------------------------------------------------------------*/
extern uint8_t set_connectable;
extern int connected;
#ifdef HAS_BLUETOOTH
BLE_DEV_DATA *DevData;
#endif
extern TIM_HandleTypeDef TimHandle;
extern volatile uint32_t HCI_ProcessEvent;
extern void CDC_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

/* Exported Variables -------------------------------------------------------------*/


uint32_t ConnectionBleStatus  =0;

uint8_t BufferToWrite[256];
int32_t BytesToWrite;

TIM_HandleTypeDef TimCCHandle;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;
RTC_HandleTypeDef hrtc;

uint32_t StartTime;
uint8_t bdaddr[6];

/* Private variables ---------------------------------------------------------*/
static volatile uint32_t SendEnv = 0;
WKUP_CONTEXT sWkupContext;
volatile uint8_t GPIO_STATE = GPIO_PIN_RESET;

/* Private function prototypes -----------------------------------------------*/
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc);
void Process_BLE_Conn(void);
void MX_RTC_Init(void);
void Error_Handler(void);
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc);
#ifdef FREQUENCY_SWEEP_TEST
void FreqSweepMain(void);
#endif

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM2)
  {
#ifndef TIM2_FREE_RUN
	  TimingIncTick();
#endif
  }
}


static void SystemClock_Config(void);

//static void Init_BlueNRG_Custom_Services(void);
//static void Init_BlueNRG_Stack(void);
static void InitTimers(void);
//static void SendEnvironmentalData(void);

//uint8_t CDC_Fill_Buffer(uint8_t* Buf, uint32_t TotalLen){}

/**
* @brief  Main program
* @param  None
* @retval None
*/
uint8_t everstarted_flag;
uint8_t __attribute__((section(".noinit"))) BLE_STANDBY;
int main(void)
{
	//	static volatile uint8_t sFlag_2;
	/* STM32L4xx HAL library initialization:
	  - Configure the Flash prefetch, instruction and Data caches
	  - Configure the Systick to generate an interrupt each 1 msec
	  - Set NVIC Group Priority to 4
	  - Global MSP (MCU Support Package) initialization
	 */
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	uint32_t start_time;
	if((BLE_STANDBY != 0) && (BLE_STANDBY != 1))
	{
		BLE_STANDBY = 0;
	}
	connected = 0;
	GPIO_InitStructure.Pin = __EVT_GPIO_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	//InitEvtGpioClock();
	//HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

	HAL_Init();
	InitEvtGpioClock();
	HAL_GPIO_Init(__EVT_GPIO_PORT, &GPIO_InitStructure);
	HAL_GPIO_WritePin(__EVT_GPIO__, GPIO_PIN_SET);
	InitBoard();
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	BSP_LED_Init(LED1);
//	ClkDependentInit();
	BLE_INIT_SPEC();
	BLE_ADD_SERVICES();
	/* initialize timers */
	InitTimers();
	StartTime = HAL_GetTick();
	HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 0);
	InitTimer2(&htim2);
	/* Infinite loop */
	SetWkupContextPointer(&sWkupContext);
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN5_HIGH);
	//TOGGLE_EVENT_PIN();
	start_time = HAL_GetTick();
	while (1) {
		if(connected ==1){
			if(CheckConnStatus() == HAL_ERROR)
			{
				BLE_INIT_SPEC();
				BLE_ADD_SERVICES();
				connected = 0;
			}
		}
		//TOGGLE_EVENT_PIN();
#if defined(HAS_BLUETOOTH)
		Process_BLE_Conn();
		CheckBufferAndSend();
#endif
		//TestPayload(); // Test power consumption
#if defined(HAS_BLUETOOTH)
		if (HCI_ProcessEvent) {
			HCI_ProcessEvent = 0;
			hci_user_evt_proc();
		}
		if((HAL_GetTick() - start_time) > 200)
		{
			if(GET_DECODER_STATE() == BLE_IDLE){
				UpdateCharacteristics();
				start_time = HAL_GetTick();
			}
		}
#endif
		TOGGLE_EVENT_PIN();
	}
}

/**
* @brief  Output Compare callback in non blocking mode 
* @param  htim : TIM OC handle
* @retval None
*/
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  uint32_t uhCapture=0;
  
  /* TIM1_CH1 toggling with frequency = 2Hz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
    uhCapture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
    /* Set the Capture Compare Register value */
    __HAL_TIM_SET_COMPARE(&TimCCHandle, TIM_CHANNEL_1, (uhCapture + uhCCR1_Val));
    SendEnv=1;
  }
}



/**
* @brief  Function for initializing timers for sending the information to BLE:
*  - 1 for sending MotionFX/AR/CP and Acc/Gyro/Mag
*  - 1 for sending the Environmental info
* @param  None
* @retval None
*/
static void InitTimers(void)
{
  uint32_t uwPrescalerValue;
  
  /* Timer Output Compare Configuration Structure declaration */
  TIM_OC_InitTypeDef sConfig;
  
  /* Compute the prescaler value to have TIM3 counter clock equal to 10 KHz */
  uwPrescalerValue = (uint32_t) ((SystemCoreClock / 10000) - 1); 
  
  /* Set TIM1 instance (Motion)*/
  /* Set TIM1 instance */
  TimCCHandle.Instance = TIM1;
  TimCCHandle.Init.Period        = 65535;
  TimCCHandle.Init.Prescaler     = uwPrescalerValue;
  TimCCHandle.Init.ClockDivision = 0;
  TimCCHandle.Init.CounterMode   = TIM_COUNTERMODE_UP;
  if(HAL_TIM_OC_Init(&TimCCHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /* Configure the Output Compare channels */
  /* Common configuration for all channels */
  sConfig.OCMode     = TIM_OCMODE_TOGGLE;
  sConfig.OCPolarity = TIM_OCPOLARITY_LOW;
  
  /* Output Compare Toggle Mode configuration: Channel1 */
  sConfig.Pulse = uhCCR1_Val;
  if(HAL_TIM_OC_ConfigChannel(&TimCCHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }  
  
}

#if defined(HAS_BLUETOOTH)
void Process_BLE_Conn(void)
{
	/* Led Blinking when there is not a client connected */
    if (connected != 1)
    {
      if(!TargetBoardFeatures.LedStatus)
      {
        if(HAL_GetTick()-StartTime > 1000)
        {
          LedOnTargetPlatform();
          TargetBoardFeatures.LedStatus =1;
          StartTime = HAL_GetTick();
        }
      }
      else
      {
        if(HAL_GetTick()-StartTime > 50)
        {
          LedOffTargetPlatform();
          TargetBoardFeatures.LedStatus =0;
          StartTime = HAL_GetTick();
        }
      }
      if(HCI_ProcessEvent){
      // Handle Ble event
		  HCI_ProcessEvent=0;
		  hci_user_evt_proc();
		  if(connected == 0x10){
			  connected = 1;
//			  SET_DECODER_STATE(BLE_IDLE);
			  if(HCI_ProcessEvent){
				  StartTime = HAL_GetTick();
				  HCI_ProcessEvent=0;
				  hci_user_evt_proc();
			  	}
		  }
      }

    }
    else{
    	if(TargetBoardFeatures.LedStatus != 0)
    	{
    		LedOffTargetPlatform();
    		TargetBoardFeatures.LedStatus = 0;
    	}
    }
}
#endif

/**
* @brief  System Clock Configuration
* @param  None
* @retval None
*/
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  
  __HAL_RCC_PWR_CLK_ENABLE();
  
#ifdef USE_RTC
  /* Enable the LSE Oscilator */
  RCC_OscInitStruct.OscillatorType = RTC_CLK_SOURCE;
  if(RTC_CLK_SOURCE == RCC_RTCCLKSOURCE_LSE){
	  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  }
  else if(RTC_CLK_SOURCE == RCC_RTCCLKSOURCE_LSI){
	  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  }
#endif
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1);
  }
  
  /* Enable the CSS interrupt in case LSE signal is corrupted or not present */
  HAL_RCCEx_DisableLSECSS();
  
  /* Enable MSI Oscillator and activate PLL with MSI as source */
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
  
  /* Enable MSI Auto-calibration through LSE */
  HAL_RCCEx_EnableMSIPLLMode();
  
  /* Select MSI output as USB clock source */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_MSI;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
  clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    while(1);
  }
}


/**
* @brief This function provides accurate delay (in milliseconds) based 
*        on variable incremented.
* @note This is a user implementation using WFI state
* @param Delay: specifies the delay time length, in milliseconds.
* @retval None
*/
void HAL_Delay(__IO uint32_t Delay)
{
  uint32_t tickstart = 0;
  tickstart = HAL_GetTick();
  while((HAL_GetTick() - tickstart) < Delay)
  {
    __WFI();
  }
}


/**
* @brief  This function is executed in case of error occurrence.
* @param  None
* @retval None
*/
void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
    
  }
}


HAL_StatusTypeDef MX_SPI1_Init(SPI_HandleTypeDef* hspi)
{
//TODO check if this function is called instead of weak one
  HAL_StatusTypeDef ret = HAL_OK;
  hspi->Instance = SPI1;
  hspi->Init.Mode = SPI_MODE_MASTER;
  hspi->Init.Direction = SPI_DIRECTION_2LINES;
  hspi->Init.DataSize = SPI_DATASIZE_8BIT;
  hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi->Init.NSS = SPI_NSS_SOFT;
  //SPI is stable at around 20MHz, above that we lose communication with BLE chip
//  if(HAL_RCC_GetSysClockFreq() <= 40000000){
//       hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
//  }
//  else{
//	  hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
//  }
  if(HAL_RCC_GetSysClockFreq() > 64000000){
       hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  }
  else if (HAL_RCC_GetSysClockFreq() > 32000000){
	  hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  }
  else if (HAL_RCC_GetSysClockFreq() > 16000000){
	  hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  }
  else{
	  hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  }
  hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi->Init.TIMode = SPI_TIMODE_DISABLE;
  hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi->Init.CRCPolynomial = 7;
  hspi->Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi->Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(hspi) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

void MX_RTC_Init(void)
{
	  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
	  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//	  __HAL_RCC_RTC_ENABLE();
	  __HAL_RCC_SYSCFG_CLK_ENABLE();
	  __HAL_RCC_PWR_CLK_ENABLE();

//	  HAL_PWR_EnableBkUpAccess();
#ifdef USE_RTC
	  if(RTC_CLK_SOURCE == RCC_RTCCLKSOURCE_LSE){
		  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
		  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	  }
	  else if(RTC_CLK_SOURCE == RCC_RTCCLKSOURCE_LSI){
		  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI;
		  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	  }
#endif
	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
	    while(1);
	  }
	  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
#ifdef USE_RTC
	  PeriphClkInit.RTCClockSelection = RTC_CLK_SOURCE;
#endif
	  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  hrtc.Instance = RTC;
	  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	  hrtc.Init.AsynchPrediv = 127;
	  hrtc.Init.SynchPrediv = 255;
	  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
	  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	  if (HAL_RTC_Init(&hrtc) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  HAL_PWR_EnableBkUpAccess();
	  HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
	  HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);


}

void SetSysFrequency(void)
{
	  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//	  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	  __HAL_RCC_PWR_CLK_ENABLE();
	  RCC_OscInitStruct.OscillatorType = RCC_RTCCLKSOURCE_NO_CLK;
#ifdef USE_RTC
	  if(RTC_CLK_SOURCE == RCC_RTCCLKSOURCE_LSE){
		  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
	  }
	  else if(RTC_CLK_SOURCE == RCC_RTCCLKSOURCE_LSI){
		  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
	  }
#endif
	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
	    while(1);
	  }
	  /* Enable MSI Oscillator and activate PLL with MSI as source */
	  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_MSI;
	  RCC_OscInitStruct.MSIState            = RCC_MSI_ON;
	  RCC_OscInitStruct.HSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	  RCC_OscInitStruct.MSIClockRange       = MSI_CLOCK_RANGE;
	  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_OFF;
	  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_NONE;
	  RCC_OscInitStruct.PLL.PLLM            = 6;
	  RCC_OscInitStruct.PLL.PLLN            = 40;
	  RCC_OscInitStruct.PLL.PLLP            = 7;
	  RCC_OscInitStruct.PLL.PLLQ            = 4;
	  RCC_OscInitStruct.PLL.PLLR            = 4;
	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
	    while(1);
	  }

	  /* Enable MSI Auto-calibration through LSE */
//	  HAL_RCCEx_EnableMSIPLLMode();

	  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	  clocks dividers */
	  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FlashLatencyWS) != HAL_OK)
	  {
	    while(1);
	  }
	  HAL_PWREx_ControlVoltageScaling(VOLTAGE_RANGE);
#if defined(USE_ART)
	   __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
	   __HAL_FLASH_DATA_CACHE_ENABLE();
	   __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
#else
	   __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
	   __HAL_FLASH_DATA_CACHE_DISABLE();
	   __HAL_FLASH_PREFETCH_BUFFER_DISABLE();
#endif

#if defined(ENABLE_LP_RUN)
	  HAL_PWREx_EnableLowPowerRunMode();
#endif
}

void FreqSweepMain(void)
{
	SetSysFrequency();
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.Pin = __EVT_GPIO_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	InitEvtGpioClock();
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	HAL_GPIO_WritePin(__EVT_GPIO__, GPIO_PIN_SET);
	HAL_Init();
	InitBLEAndSetItToStandby();
	while(1)
	{
		TOGGLE_EVENT_PIN();
		TestPayload();
		TOGGLE_EVENT_PIN();
		TestPayload();
	}
}



void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	sWkupContext.eWkupReason = RTC_IT;
}

void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
  if(hrtc->Instance==RTC)
  {
    __HAL_RCC_RTC_ENABLE();
  }

}

#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: STLBLE_PRINTF("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1){
  }
}
#endif


/******************* (C) COPYRIGHT 2016 STMicroelectronics *****END OF FILE****/
