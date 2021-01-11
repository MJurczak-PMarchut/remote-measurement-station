/*
 * station_specific.c
 *
 *  Created on: Jan 16, 2020
 *      Author: OMEN
 */
#include "station_specific.h"
#include "stdlib.h"
#include "ble_decode_callback.h"
#include "stdio.h"


#include "hci.h"

uint8_t TxBuffer[TX_BUFFER_SIZE];
void* BLESendBuffer(void);
void PurgeSerialBuffer(void);


BufferStruct sBuffer = {
		.BufferEnd = 0,
		.BufferStart = 0,
		.pBuffer = TxBuffer
};
uint8_t CheckBufferSize(void);
uint8_t PutDataToBuffer(char *pData, uint8_t len);
uint8_t* GetDataFromBUffer(void);
void CheckBufferAndSend(void);
extern TIM_HandleTypeDef htim2;
#ifdef BOARD_N64_F4

#define bool uint8_t
#define true 1
#define false 0
#define TokenTypeNo sizeof(sToken)/sizeof(Token)




void AppendCommandsToBuffer(void);
void FindAndDecodeTokensStation(void);
void ExecuteCommands(void);
void SYNCHRONIZE();
void CONNECT_DEV(DataDecode *data, uint8_t len);

void Data_Recv_Single(void);
void Data_Recv_Multiple(void);

uint8_t TokenCounter = 0;
uint8_t CheckBufferSize(void);
#ifdef BOARD_N64_F4
uint8_t CharHandle[2];
uint8_t BLE_DATA_PTR[MAX_LEN_OF_MULT_CHAR_READ];
#endif

uint8_t GetDAta = 0;



Token sToken[] = {
				{SCAN, "SCAN"},
				{CONNECT, "CONNECT"},
				{GET_DATA, "GET_DATA"},
				{ABORT,"ABORT"},
				{SYNCHORONIZE, "SYNCHORONIZE"},
				{HARD_RESET, "HRD_RESET"}
		};



BLE_DEV_DATA *BLEDevData = NULL;

UART_HandleTypeDef *StationUart = NULL;
char Message[RX_BUFFER_SIZE] = {0};


DataDecode internalData[MAX_TOKEN_NO];

uint8_t UART_RX;

uint8_t AppendAndCheckMessage(uint8_t Char);

uint8_t CheckBufferSize(void)
{
	uint8_t ret;
	if(sBuffer.BufferStart < sBuffer.BufferEnd)
	{
		ret = TX_BUFFER_SIZE - (sBuffer.BufferEnd - sBuffer.BufferStart);
	}
	else if (sBuffer.BufferStart > sBuffer.BufferEnd)
	{
		ret = sBuffer.BufferStart -  sBuffer.BufferEnd - 1;
	}
	else
	{
		ret = TX_BUFFER_SIZE;
	}
	return ret;
}


uint8_t* GetDataFromBUffer(void)
{
	uint8_t* ret;
	ret = &sBuffer.pBuffer[sBuffer.BufferStart];
	if(sBuffer.BufferStart != sBuffer.BufferEnd)
	{
		sBuffer.BufferStart = (sBuffer.BufferStart < (TX_BUFFER_SIZE - 1))?sBuffer.BufferStart+1:0;
	}
	else
	{
		ret = NULL;
	}
	return ret;
}

uint8_t PutDataToBuffer(char *pData, int8_t len)
{

	uint8_t u8Iter;
	if(CheckBufferSize() >= len){
		for(u8Iter = 0; u8Iter < len; u8Iter++)
		{
			if(sBuffer.BufferEnd + u8Iter < TX_BUFFER_SIZE){
				sBuffer.pBuffer[sBuffer.BufferEnd + u8Iter] = pData[u8Iter];
			}
			else if(sBuffer.BufferEnd + u8Iter >= TX_BUFFER_SIZE)
			{
				sBuffer.pBuffer[sBuffer.BufferEnd + u8Iter - TX_BUFFER_SIZE] = pData[u8Iter];
			}
		}
		sBuffer.BufferEnd = ((sBuffer.BufferEnd + len) < TX_BUFFER_SIZE)? (sBuffer.BufferEnd + len) : (sBuffer.BufferEnd + len - TX_BUFFER_SIZE);
		if((HAL_UART_GetState(StationUart) & HAL_UART_STATE_BUSY_TX) != HAL_UART_STATE_BUSY_TX)
		{
			HAL_UART_Transmit_IT(StationUart, GetDataFromBUffer(), 1);
		}
		return 0;
	}
	return 1;
}

HAL_StatusTypeDef InitStation(UART_HandleTypeDef *huart)
{

	StationUart = huart;
	BLE_INIT_SPEC();
	return HAL_UART_Receive_IT(StationUart, &UART_RX, 1);
}

uint8_t* STATION_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(StationUart != NULL)
	{
		if(StationUart->Instance == huart->Instance)
		{
			if(AppendAndCheckMessage(UART_RX) != 0)
			{
				FindAndDecodeTokensStation();
				ExecuteCommands();
			}
			HAL_UART_Receive_IT(StationUart, &UART_RX, 1);
			return(&UART_RX);
		}
	}
	return 0;
}


uint8_t AppendAndCheckMessage(uint8_t Char)
{
	uint8_t ucRet = 0;
	static uint8_t uCharCount;
	if((Char == UART_TERM_CHAR) || (Char == '\r') || (RX_BUFFER_SIZE - 1 <= uCharCount))
	{
		Message[uCharCount] = 0;
		ucRet = uCharCount;
		uCharCount = 0;
		return ucRet;
	}
	else
	{
		Message[uCharCount] = Char;
		uCharCount++;
	}
	return 0;
}

void Station_MS_Process(void)
{
	hci_user_evt_proc();
}



void FindAndDecodeTokensStation(void)
{
	uint8_t uTokenIter = 0;
	uint8_t uDecodeIter = 0;
	uint8_t KeywordIter = 0;
	bool 	DataFound = false;
	int temp1 = 0;

	internalData[uTokenIter].vData.cData = strtok(Message, " ");
	internalData[uTokenIter].eDataTypes = STRING;
	while(internalData[uTokenIter].vData.cData != NULL){
		uTokenIter++;
		internalData[uTokenIter].eDataTypes = STRING;
		internalData[uTokenIter].vData.cData = strtok(NULL, " ");
	}
	internalData[uTokenIter].eDataTypes= EMPTY;

	TokenCounter = uTokenIter;

	for(uDecodeIter = 0; uDecodeIter < uTokenIter; uDecodeIter++)
	{
		DataFound = false;
		for(KeywordIter = 0; KeywordIter < TokenTypeNo;KeywordIter++)
		{
			if(strcmp(internalData[uDecodeIter].vData.cData, sToken[KeywordIter].pcKeyword) == 0)
			{
				internalData[uDecodeIter].eDataTypes = KEYWORD;
				internalData[uDecodeIter].vData.eData = sToken[KeywordIter].eKeyword;
				DataFound = true;
			}
		}

		if (strspn(internalData[uDecodeIter].vData.cData, "1234567890-") == strlen(internalData[uDecodeIter].vData.cData)){
			temp1 = (int)(atoi(internalData[uDecodeIter].vData.cData));
			if(DataFound == false)
			{
				internalData[uDecodeIter].eDataTypes = INT;
				internalData[uDecodeIter].vData.iData = temp1;
				DataFound = true;
			}
		}
	}
}

void SendNamesToScreen(uint8_t *pData, uint8_t len, uint8_t Number)
{
	Number = Number + 48;
	if(CheckBufferSize() > len + 3){ //we don't wait for a place in buffer because we will most likely use this in an interrupt
		PutDataToBuffer((char*)&Number, 1);
		PutDataToBuffer(" ", 1);
		PutDataToBuffer((char*)pData, len);
		PutDataToBuffer("\n", 1);
	}
}


void SendConnectionFeedback(BLE_CONNECTION_STATUS Status)
{
	if(CheckBufferSize() > 8)
	{
		if(Status == BLE_CONNCETION_SUCCESFUL){
			PutDataToBuffer("CONN_OK\n",8);
		}
		else{
			PutDataToBuffer("FAIL_CN\n",10);
		}
	}
}

void DiscoveryCplt(void)
{
	if(CheckBufferSize() > 10)
	{
		PutDataToBuffer("DISC_DONE\n", 10);
	}
	CLR_CALLBACK_FUNC();
}

void ExecuteCommands(void)
{
	if(TokenCounter > 0){
		switch(internalData[0].eDataTypes)
		{
			case KEYWORD:
				switch(internalData[0].vData.eData)
				{
					case SCAN:
						if(BLE_START_SCAN(DiscoveryCplt) == HAL_OK){
							SET_DECODER_STATE(BLE_SCAN);
							PutDataToBuffer("SCAN_START\n", 11);
						}
						else
						{
							PutDataToBuffer("SCAN_FAIL\n",10);
						}
						break;
					case SYNCHORONIZE:
						SYNCHRONIZE();
						break;
					case CONNECT:
						CONNECT_DEV(&internalData[1], TokenCounter - 1);
						break;
					case HARD_RESET:
						NVIC_SystemReset();
						break;
					case GET_DATA:
						BLE_GET_DATA(&internalData[1]);
						break;

					default:
						break;
				}
				break;
				default:
					break;
		}
	}
}


void CONNECT_DEV(DataDecode *data, uint8_t len)
{
	if(len != 0)
		{
			switch(data[0].eDataTypes)
			{
			case INT:
				switch(data[1].eDataTypes)
				{
					case INT:
						if(BLE_CONNECT_TO_DEVICE(data[0].vData.iData, data[1].vData.iData) != HAL_OK){
							PutDataToBuffer("Conn_start_error\n", 17);
						}
						else{
							PutDataToBuffer("Conn_START\n", 11);
						}

						break;
					default:
						break;
				}
				break;
				case KEYWORD:
				{
					if(data[0].vData.eData == ABORT)
					{
						aci_gap_terminate_gap_procedure(GAP_DIRECT_CONNECTION_ESTABLISHMENT_PROC);
					}
				}
				default:
					break;
			}
		}
}

void SYNCHRONIZE(void)
{
	static uint8_t Dev_No = 0;
	uint8_t VAL = WRITE_DATA;
	BLE_DEV_DATA *data;
	BLE_GET_DEV_DATA(Dev_No,&data);
	if(Dev_No < NoOfDevices)
	{
		if(BLE_WRITE_CHAR(Dev_No, data->sServiceIDData, &data->sCharIDData[BLE_SD_CONF_CHAR], SYNC_LEN,&VAL, SYNCHRONIZE) == HAL_OK){
			Dev_No++;
		}

	}
	else
	{
		Dev_No = 0;
	}
	CLR_CALLBACK_FUNC();
}
#endif

#ifdef BOARD_N64_F4


void BLE_GET_DATA(DataDecode *Data)
{
//	BLE_DEV_DATA *BLEDevData;
//	GetDAta = 1;
	static uint8_t state = 0;
	switch(Data->eDataTypes){
		case INT:
			if(Data->vData.iData < NoOfDevices){
				BLE_GET_DEV_DATA(Data->vData.iData, &BLEDevData);
				if(((BLEDevData->CharPresent) & ((1<<BLE_ACC_CHAR) | (1<<BLE_GYRO_CHAR))) == ((1<<BLE_ACC_CHAR) | (1<<BLE_GYRO_CHAR)))
				{
	//				CharHandle[0] = BLEDevData->sCharIDData[BLE_ACC_CHAR].CharHandle;
	//				CharHandle[1] = BLEDevData->sCharIDData[BLE_GYRO_CHAR].CharHandle;
 					if (BLE_READ_CHAR(BLEDevData->ConnectionHandle, BLE_DATA_PTR, 4*3,  &BLEDevData->sCharIDData[BLE_ACC_CHAR], Data_Recv_Multiple) == HAL_OK){
 						GetDAta = 1;
 					}
				}
			}
			break;
		case KEYWORD:
			if(Data->vData.eData == ABORT)
			{
				GetDAta = 0;
			}
	}

 }
void Data_Recv_Single(void)
{
	char Message[50];
	//FIXME wywalic to
	uint8_t len;
	typedef struct
	{
		int32_t x;
		int32_t y;
		int32_t z;
	}axisData;
	axisData *paData = (axisData*)BLE_DATA_PTR;

	len = sprintf(Message,"%d %d %d\n", paData->x, paData->y,paData->z);
	PutDataToBuffer(Message, len);
	CLR_CALLBACK_FUNC();
}


void Data_Recv_Multiple(void)
{
	char Message[50];
	//FIXME wywalic to
	uint8_t len;
	typedef struct
	{
		int32_t x;
		int32_t y;
		int32_t z;
	}axisData;
	axisData *paData = (axisData*)BLE_DATA_PTR;

	len = sprintf(Message,"%d %d %d\n", paData->x, paData->y,paData->z);
	PutDataToBuffer(Message, len);
	if(GetDAta != 0){
		CLR_CALLBACK_FUNC();
		if (BLE_READ_CHAR(BLEDevData->ConnectionHandle, BLE_DATA_PTR, 4*3,  &BLEDevData->sCharIDData[BLE_ACC_CHAR], Data_Recv_Multiple) == HAL_OK){
			//TODO Add something here
		}
	}
}

#endif

#ifdef BOARD_SENSORTILE

T_SensorsData mptr;
T_SensorsData *rptr = &mptr;
int32_t getSensorsData( T_SensorsData *mptr);

void InitBoard(void)
{


	  BSP_MOTION_SENSOR_Init(LSM6DSM_0, MOTION_ACCELERO | MOTION_GYRO);
	  BSP_MOTION_SENSOR_SetOutputDataRate(LSM6DSM_0, MOTION_ACCELERO, LSM6DSM_ACC_ODR);
	  BSP_MOTION_SENSOR_SetFullScale(LSM6DSM_0, MOTION_ACCELERO, LSM6DSM_ACC_FS);
	  BSP_MOTION_SENSOR_SetOutputDataRate(LSM6DSM_0, MOTION_GYRO, LSM6DSM_GYRO_ODR);
	  BSP_MOTION_SENSOR_SetFullScale(LSM6DSM_0, MOTION_GYRO, LSM6DSM_GYRO_FS);



}

int32_t getSensorsData( T_SensorsData *mptr)
{
  int32_t ret = BSP_ERROR_NONE;
  mptr->ms_counter = HAL_GetTick();

  /* Get Data from Sensors */
  if ( BSP_MOTION_SENSOR_GetAxes(LSM6DSM_0, MOTION_ACCELERO, &mptr->acc ) == BSP_ERROR_COMPONENT_FAILURE )
  {
    mptr->acc.x = 0;
    mptr->acc.y = 0;
    mptr->acc.z = 0;
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }

  if ( BSP_MOTION_SENSOR_GetAxes(LSM6DSM_0, MOTION_GYRO, &mptr->gyro ) == BSP_ERROR_COMPONENT_FAILURE )
  {
    mptr->gyro.x = 0;
    mptr->gyro.y = 0;
    mptr->gyro.z = 0;
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }

  if ( BSP_MOTION_SENSOR_GetAxes(LSM303AGR_MAG_0, MOTION_MAGNETO, &mptr->mag ) == BSP_ERROR_COMPONENT_FAILURE )
  {
    mptr->mag.x = 0;
    mptr->mag.y = 0;
    mptr->mag.z = 0;
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }


  return ret;
}


void UpdateCharacteristics(void)
{
	uint8_t Message[50] = "aaa\n";
	BLE_DEV_DATA *DevData = NULL;
	uint8_t len;
	typedef struct
	{
		BSP_MOTION_SENSOR_Axes_t acc;
		BSP_MOTION_SENSOR_Axes_t gyro;

	}sDat;
	typedef union
	{
		sDat sData;
		uint8_t pData;
	}uDat;

//	uDat vDat;

//	uint8_t Data[4*3*2] = 0;

//	if(getSensorsData(rptr) != BSP_ERROR_NONE)
	if(0)
	{
		return;
	}
	else
	{
//		vDat.sData.acc.x = rptr->acc.x;
//		vDat.sData.acc.y = rptr->acc.y;


//		BLE_GET_DEV_DATA(&DevData);

//		len = sprintf(Message,"%d %d %d\n", rptr->acc.x, rptr->acc.y, rptr->acc.z);
		len = 4;
		BLE_UPDATE_CHAR(&DevData->sServiceIDData[BLE_SERIAL_SERVICE], &DevData->sCharIDData[BLE_SERIAL_RD_CHAR], len, Message, NULL);

	}
}

uint32_t *pHCI_ProcessEvent;

void setHCI_Event_var(uint32_t* puin)
{
	pHCI_ProcessEvent = pHCI_ProcessEvent;
}

void SendToBLESerial(unsigned char *string, unsigned char len)
{
static uint32_t past, present;

//TODO it will probably be better to send max 15 characters each time, as notification has a data limit of around 20
//	while(GET_DECODER_STATE() != BLE_IDLE){
//		if(*pHCI_ProcessEvent)
//			{
//			  *pHCI_ProcessEvent=0;
//			  hci_user_evt_proc();
//			}
//	}
	present = htim2.Instance->CNT;
	while(PutDataToBuffer(string, len) == 1){
		if(*pHCI_ProcessEvent)
			{
			  *pHCI_ProcessEvent=0;
			  hci_user_evt_proc();
			}
	}
	past  = htim2.Instance->CNT;

}
void ClkDependentInit(void)
{
	  InitTimer2(&htim2);
#if defined(HAS_BLUETOOTH)
	  hci_tl_lowlevel_init();
#endif
//	  InitBoard();

}


uint8_t CheckBufferSize(void)
{
	uint8_t ret;
	if(sBuffer.BufferStart < sBuffer.BufferEnd)
	{
		ret = TX_BUFFER_SIZE - (sBuffer.BufferEnd - sBuffer.BufferStart);
	}
	else if (sBuffer.BufferStart > sBuffer.BufferEnd)
	{
		ret = sBuffer.BufferStart -  sBuffer.BufferEnd - 1;
	}
	else
	{
		ret = TX_BUFFER_SIZE;
	}
	return ret;
}

uint8_t* GetDataFromBUffer(void)
{
	uint8_t* ret;
	ret = &sBuffer.pBuffer[sBuffer.BufferStart];
	if(sBuffer.BufferStart != sBuffer.BufferEnd)
	{
		sBuffer.BufferStart = (sBuffer.BufferStart < (TX_BUFFER_SIZE - 1))?sBuffer.BufferStart+1:0;
	}
	else
	{
		ret = NULL;
	}
	return ret;
}

uint8_t PutDataToBuffer(char *pData, uint8_t len)
{

	uint8_t u8Iter;
	uint8_t buffer_size_left = 0;
	buffer_size_left = CheckBufferSize();
	if(buffer_size_left > len){
		for(u8Iter = 0; u8Iter < len; u8Iter++)
		{
			if(sBuffer.BufferEnd + u8Iter < TX_BUFFER_SIZE){
				sBuffer.pBuffer[sBuffer.BufferEnd + u8Iter] = pData[u8Iter];
			}
			else if(sBuffer.BufferEnd + u8Iter >= TX_BUFFER_SIZE)
			{
				sBuffer.pBuffer[sBuffer.BufferEnd + u8Iter - TX_BUFFER_SIZE] = pData[u8Iter];
			}
		}
		sBuffer.BufferEnd = ((sBuffer.BufferEnd + len) < TX_BUFFER_SIZE)? (sBuffer.BufferEnd + len) : (sBuffer.BufferEnd + len - TX_BUFFER_SIZE);
		if(GET_DECODER_STATE() == BLE_IDLE)
		{
			BLESendBuffer();
		}
		return 0;
	}
	if(GET_DECODER_STATE() == BLE_IDLE)
	{
		BLESendBuffer();
	}
	return 1;
}

void CheckBufferAndSend(void)
{
	if((CheckBufferSize() != TX_BUFFER_SIZE) && (GET_DECODER_STATE() == BLE_IDLE))
	{
		BLESendBuffer();
	}
}

void* BLESendBuffer(void){
// static uint32_t past, present;
 uint8_t uIter = 0;
 uint8_t message[21];
 uint8_t *pu8char = 1;
 BLE_DEV_DATA *DevData = NULL;
 BLE_GET_DEV_DATA(&DevData);
// pu8char = GetDataFromBUffer();
 for(uIter = 0; (uIter < 20) && (pu8char != NULL);  uIter++)
 {
	 pu8char = GetDataFromBUffer();
	 message[uIter] = *pu8char;
	 if(pu8char == NULL)
	 {
		 break;
	 }

 };
 if((uIter > 0) && (message[0] != NULL))
 {
	 BLE_UPDATE_CHAR(&DevData->sServiceIDData[BLE_SERIAL_SERVICE], &DevData->sCharIDData[BLE_SERIAL_RD_CHAR], uIter, message, BLESendBuffer);
 }
 if(CheckBufferSize() < TX_BUFFER_SIZE){
	 return BLESendBuffer;
 }
 else return NULL;
}

void PurgeSerialBuffer(void)
{
	sBuffer.BufferStart = 0;
	sBuffer.BufferEnd = 0;
}
#endif
