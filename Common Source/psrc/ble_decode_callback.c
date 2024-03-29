/*
 * ble_decode_callback.c
 *
 *  Created on: 4 sty 2020
 *      Author: Mateusz Jurczak
 */

/*
 * A better context based control would be useful here
 * I'am inching ever closer to finishing of this project, however each time i add 1 new functionality
 * 4 other get broken or need rewriting, good programming practices should be implemented, however they are hard and take time
 * so I'll keep up with my questionable practices in hopes that no soul will ever be forced to read or support this monstrosity
 *
 * The more time I spend here the more I think that the entire decision and event handling tree
 * needs to be implemented to just get basic functionality working reliably
 *
 * Hours wasted here : 24
 *
 *
 */


#include "ble_decode_callback.h"

#include "station_specific.h"
#include "bluenrg_aci_const.h"
#include "bluenrg_hal_aci.h"
//#include "bluenrg_conf.h"
#include "bluenrg_gap.h"
#include "hci.h"
#include "bluenrg_utils.h"
#include "hci_le.h"
#include "sm.h"
#include "pwr_control.h"
#ifdef HAS_BLUETOOTH

#ifdef BOARD_SENSORTILE
#include "TargetFeatures.h"

extern int connected;
extern TargetFeatures_t TargetBoardFeatures;
//static uint16_t SensorServiceHandle, SDServiceHandle, LEDServiceHandle;
//static uint16_t ACC_char_handle, GYRO_char_handle, SD_char_handle, LED_char_handle, LED_conf_handle, SD_conf_handle;


#endif

uint8_t CharDisc = 0;

#ifdef BOARD_SENSORTILE
#define ADV_DATA_TYPE      ADV_IND
/*---------- Minimum Advertising Interval (for a number N, Time = N x 0.625 msec) -----------*/
#define ADV_INTERV_MIN      0x0020
/*---------- Maximum Advertising Interval (for a number N, Time = N x 0.625 msec) -----------*/
#define ADV_INTERV_MAX      0x1000
#endif

#ifdef BOARD_N64_F4

BLE_READ_DATA_STRUCT sBLE_DATA_READ_INFO;

void BLE_DISC_SERVICES(uint8_t DevNo);

void DISC_CPLT(uint16_t handle);

void ATT_READ_MULTIPLE_RESP(evt_att_read_mult_resp *cc);
void ATT_READ_RESP(evt_att_read_resp *cc);

void GATT_DISC_CPLT(uint16_t handle);
#endif

void BLE_READ_REQUEST_CB(uint16_t handle);
void BLE_ATTR_MODIFIED_CB(uint16_t attr_handle, uint8_t * att_data, uint8_t data_length);

#ifdef BOARD_SENSORTILE

void BLE_SET_CONNECTABLE(void);

#endif

BLE_State MasterState = BLE_IDLE;
uint8_t MasterDevNo = 0;

uint8_t Service2Discover = 0;
#ifdef BOARD_N64_F4
BLE_DEV_DATA BLEDeviceData[NoOfDevices];
#endif
#ifdef BOARD_SENSORTILE
BLE_DEV_DATA __attribute__((section(".noinit"))) BLEDeviceData;
#endif
void DisconnectionComplete_CB(evt_disconn_complete *evt_data);
HAL_StatusTypeDef BLE_WAIT_FOR_TX_POOL(void);

void ConnectionComplete_CB(uint8_t addr[6], uint16_t handle,uint8_t Status, uint8_t addr_type);

HAL_StatusTypeDef BLE_SET_DISC_DEV_NO(uint8_t DevNo);

#ifdef BOARD_N64_F4
uint8_t TempBdAddr[MAX_NO_OF_DISCOVERABLE_DEVICES][6];
uint8_t TempBdAddrType[MAX_NO_OF_DISCOVERABLE_DEVICES];
uint8_t BdAddrBufferCounter = 0;

#endif

VoidFuncPointer CpltCallback = NULL;

typedef enum{BLE_DISC_ALL_SERVICES_OFF, BLE_DISC_ALL_SERVICES_ON}  BLE_DISC_ALL_SERVICES_STATE ;
#ifdef BOARD_N64_F4
	typedef enum{BLE_STATION_DEVICE_BONDING, BLE_STATION_IDLE} BLE_STATION_STATE;
	BLE_STATION_STATE BLE_STATION_MASTER_STATE = BLE_STATION_IDLE;
#endif
BLE_DISC_ALL_SERVICES_STATE  BLE_DISC_ALL_SERVICES_MASTER_STATE =  BLE_DISC_ALL_SERVICES_OFF;

Comp_Result BLE_CHECK_IF_ADDR_PRESENT (uint8_t *bdaddr);

#ifndef CUSTOM_BLE_SERVICE_UUIDs

uint8_t UUID_SERVICE_DATA[MAX_SERV_NUM_PER_DEV][UUID_LENGTH] ={
	[BLE_SENS_SERVICE] = {0,0,0,0,0,0,0,0,0x15,0x2B,0x54,0x15,0xB4,0xA2,0x74,0xDA},
	[BLE_LED_SERVICE] = {0,0,0,0,0,0,0,0,0x25,0x2A,0x52,0x05,0x45,0xA2,0x32,0xFA},
	[BLE_SD_SERVICE] = {0,0,0,0,0,0,0,0,0x45,0x2C,0x57,0xFF,0x45,0x1A,0x78,0xC6}
};
#else


uint8_t UUID_SERVICE_DATA[MAX_SERV_NUM_PER_DEV][UUID_LENGTH] ={
	[BLE_SENS_SERVICE] = CUSTOM_BLE_SENS_SERVICE_UUID,
	[BLE_LED_SERVICE] = CUSTOM_BLE_LED_SERVICE_UUID,
	[BLE_SD_SERVICE] = CUSTOM_BLE_SD_SERVICE_UUID,
	[BLE_CONF_SERVICE] = CUSTOM_BLE_CONFIG_SERVICE_UUID,
	[BLE_SERIAL_SERVICE] = CUSTOM_BLE_SERIAL_SERVICE_UUID
};

#endif


#ifndef CUSTOM_BLE_CHARACTERISCTIC_UUIDs

uint8_t UUID_CHAR_DATA[MAX_CHAR_NUM_PER_SERV*MAX_SERV_NUM_PER_DEV][UUID_LENGTH] ={
	[BLE_GYRO_CHAR] = {0,0,0,0,0,0,0,0,0xDA,0xCF,0x48,0x11,0x05,0xE4,0x91,0x28},
	[BLE_ACC_CHAR] = {0,0,0,0,0,0,0,0,0x40,0x36,0x14,0x97,0x65,0xCD,0x80,0x45},
	[BLE_LED_CONF_CHAR] = {0,0,0,0,0,0,0,0,0xAD,0x27,0x02,0x49,0xA8,0x92,0x87,0xCA},
	[BLE_LED_READ_CHAR] = {0,0,0,0,0,0,0,0,0x09,0xFC,0x35,0x42,0xAF,0x14,0x98,0xF6},
	[BLE_SD_CONF_CHAR] = {0,0,0,0,0,0,0,0,0xF4,0x82,0x95,0xF2,0xCA,0x27,0x28,0xA6},
	[BLE_SD_READ_CHAR] = {0,0,0,0,0,0,0,0,0xFB,0x54,0x32,0x94,0xCF,0x41,0x08,0x86}
};
#else
uint8_t UUID_CHAR_DATA[MAX_CHAR_NUM_PER_SERV*MAX_SERV_NUM_PER_DEV][UUID_LENGTH] ={
	[BLE_GYRO_CHAR] = CUSTOM_BLE_GYRO_CHAR_UUID,
	[BLE_ACC_CHAR] = CUSTOM_BLE_ACC_CHAR_UUID,
	[BLE_LED_CONF_CHAR] = CUSTOM_BLE_LED_CONF_CHAR_UUID,
	[BLE_LED_READ_CHAR] = CUSTOM_BLE_LED_READ_CHAR_UUID,
	[BLE_SD_CONF_CHAR] = CUSTOM_BLE_SD_CONF_CHAR_UUID,
	[BLE_SD_READ_CHAR] = CUSTOM_BLE_SD_READ_CHAR_UUID,
	[BLE_GYRO_CONF_CHAR] = CUSTOM_BLE_GYRO_CONF_CHAR_UUID,
	[BLE_ACCEL_CONF_CHAR] = CUSTOM_BLE_ACCEL_CONF_CHAR_UUID,
	[BLE_SERIAL_RD_CHAR] = CUSTOM_BLE_SERIAL_RD_CHAR_UUID,
	[BLE_SERIAL_WR_CHAR] = CUSTOM_BLE_SERIAL_WR_CHAR_UUID

};
#endif

uint8_t BLE_TX_POOL_FREE = 0;
uint8_t SendEnv = 0;

#ifdef BOARD_N64_F4

void ATT_READ_MULTIPLE_RESP(evt_att_read_mult_resp *cc)
{
	if(cc->event_data_length >= sBLE_DATA_READ_INFO.len)
	{
		sBLE_DATA_READ_INFO.pData = cc->set_of_values;
	}
}

void ATT_READ_RESP(evt_att_read_resp *cc)
{
	uint8_t u8Iter;
	if(cc->event_data_length >= sBLE_DATA_READ_INFO.len)
	{
		for(u8Iter = 0; u8Iter < sBLE_DATA_READ_INFO.len; u8Iter++)
		{
			sBLE_DATA_READ_INFO.pData[u8Iter] = cc->attribute_value[u8Iter];
		}
	}
}

Comp_Result BLE_CHECK_IF_ADDR_PRESENT (uint8_t *bdaddr)
{
	uint8_t u8Iter, u8OctetIter, ret;
	ret = 1;
	for(u8Iter = 0; u8Iter < BdAddrBufferCounter; u8Iter++)
	{
		for(u8OctetIter= 0; u8OctetIter < BDADDR_SIZE; u8OctetIter++)
		{
			if(bdaddr[u8OctetIter] != TempBdAddr[u8Iter][u8OctetIter])
			{
				ret = 0;
			}
		}
		if(ret == 1){
			return COMP_ERROR;
		}
		ret = 1;
	}
	return COMP_RESULT_OK;
}

#endif
#ifdef BOARD_N64_F4

Comp_Result BLE_COMPARE_UUID(uint8_t *UUID, uint8_t *UUID2)
{

	uint8_t u8OctetIter = 0;
	Comp_Result ret = COMP_RESULT_OK;
	for(u8OctetIter = 0; u8OctetIter < UUID_LENGTH; u8OctetIter++)
	{
		if(UUID[u8OctetIter] != UUID2[u8OctetIter])
		{
			ret = COMP_RESULT_NOK;
			return ret;
		}
	}
	return ret;

}
#endif

#ifdef BOARD_N64_F4

BLE_UUID_MATCH_DATA_STRUCT BLE_FIND_MATCHING_UUID(uint8_t *UUID )
{

	BLE_UUID_MATCH_DATA_STRUCT ret;
	uint8_t u8UUIDIter = 0;
	ret.eCompResult = COMP_ERROR;
	switch(MasterState)
	{
		case BLE_DISC_SERV:
			for(u8UUIDIter = 0; u8UUIDIter < MAX_SERV_NUM_PER_DEV; u8UUIDIter++)
			{
				if(BLE_COMPARE_UUID(UUID_SERVICE_DATA[u8UUIDIter],UUID) == COMP_RESULT_OK)
				{
					ret.eCompResult = COMP_RESULT_OK;
					ret.uBLE_UUID_MATCH_DATA.ServiceUUID = (BLE_SERVICE_UUID) u8UUIDIter;
					return ret;
				}
			}
		break;
		case BLE_DISC_CHAR:
			for(u8UUIDIter = 0; u8UUIDIter < MAX_CHAR_NUM_PER_SERV*MAX_SERV_NUM_PER_DEV; u8UUIDIter++)
			{
				if(BLE_COMPARE_UUID(UUID_CHAR_DATA[u8UUIDIter],UUID) == COMP_RESULT_OK)
				{
					ret.eCompResult = COMP_RESULT_OK;
					ret.uBLE_UUID_MATCH_DATA.CharUUID = (BLE_CHAR_UUID) u8UUIDIter;
					return ret;
				}
			}
		break;
		default:
			return ret;
	}
	return ret;
}

#endif

void BLE_DECODE_GROUP_RESP(evt_att_read_by_group_resp *cc)
{
#ifdef DEBUG_BL
	uint8_t whiledebug = 1;
#endif
#ifdef BOARD_N64_F4
	uint8_t *ptrData;
	uint8_t BLE_UUID_DATA;
	BLE_UUID_MATCH_DATA_STRUCT ret;
	evt_att_read_by_type_resp *c2;
#endif
	switch(MasterState)
	{

#ifdef BOARD_N64_F4
	case BLE_DISC_SERV:
		if(BLEDeviceData[MasterDevNo].ConnectionHandle == cc->conn_handle)
		{
			if(cc->attribute_data_length > DEFAULT_SERV_DATA_LEN)
			{	// Not a default BlueNRG service
				ptrData = cc->attribute_data_list;					// A pointer to move around
				ptrData = ptrData + 4;
				ret = BLE_FIND_MATCHING_UUID(ptrData);			// 4 is an offset from the beginning to start of UUID
				ptrData = ptrData - 4;
				if(ret.eCompResult == COMP_RESULT_OK)
				{
//					ptrData = ptrData + 16;
					BLEDeviceData[MasterDevNo].ServicesPresent = BLEDeviceData[MasterDevNo].ServicesPresent | (1 << ret.uBLE_UUID_MATCH_DATA.ServiceUUID); // Set Service Bit
					BLEDeviceData[MasterDevNo].sServiceIDData[ret.uBLE_UUID_MATCH_DATA.ServiceUUID].ServiceHandle  =   	ptrData[1]<<8 | ptrData[0] ; // First two octets are service handle
					BLEDeviceData[MasterDevNo].sServiceIDData[ret.uBLE_UUID_MATCH_DATA.ServiceUUID].EndGroupHandle = 	ptrData[3]<<8 | ptrData[2] ; // Next two octets are End Group Handle
				}
			}
		}

		break;
#endif

#ifdef BOARD_N64_F4
	case BLE_DISC_CHAR:
		c2 = cc;
		ptrData = c2->handle_value_pair;
		if(BLEDeviceData[MasterDevNo].ConnectionHandle == cc->conn_handle)
			{
				if(c2->event_data_length > 0)
				{ //anything here ?
//					ptrData = cc->attribute_data_list;	// A pointer to move around
					ptrData = ptrData + (c2->handle_value_pair_length - UUID_LENGTH);
					ret = BLE_FIND_MATCHING_UUID(ptrData);			// 6 is an offset from the beginning to start of UUID
					ptrData = ptrData - (c2->handle_value_pair_length - UUID_LENGTH);
					if(ret.eCompResult == COMP_RESULT_OK)
					{
//						ptrData = ptrData + 15;
						BLEDeviceData[MasterDevNo].CharPresent = BLEDeviceData[MasterDevNo].CharPresent | (1 << ret.uBLE_UUID_MATCH_DATA.CharUUID); // Set characteristic bit
						BLEDeviceData[MasterDevNo].sCharIDData[ret.uBLE_UUID_MATCH_DATA.CharUUID].CharHandle  =   	ptrData[1]<<8 | ptrData[0] ; // First two octets are characteristic handle
						BLEDeviceData[MasterDevNo].sCharIDData[ret.uBLE_UUID_MATCH_DATA.CharUUID].CharProperties = 	ptrData[2] ; // Next octet are characteristic properties
						BLEDeviceData[MasterDevNo].sCharIDData[ret.uBLE_UUID_MATCH_DATA.CharUUID].CharValueHandle = 	ptrData[4]<<8 | ptrData[3] ; // Next two octets are value handle
					}
				}
			}

		break;
#endif

#ifdef BOARD_N64_F4
	case BLE_DISC_DESC:

		//we are not using descriptors
#ifdef DEBUG_BL
		while(whiledebug){} //pause to debug
#endif
		break;
#endif
	case BLE_IDLE:
		//we shouldn't be here
#ifdef DEBUG_BL
		while(whiledebug){} //pause to debug
#endif
			break;
	default:
		break;
	}
}
HAL_StatusTypeDef SET_DECODER_STATE(BLE_State eBLE_State)
{
	if(eBLE_State <= 8)
	{
		MasterState = eBLE_State;
		return HAL_OK;
	}
	return HAL_ERROR;
}
BLE_State GET_DECODER_STATE(void)
{
	return MasterState;
}

#ifdef BOARD_N64_F4
HAL_StatusTypeDef BLE_SET_DISC_DEV_NO(uint8_t DevNo)
{
	if(DevNo < NoOfDevices)
	{
		MasterDevNo = DevNo;
		return HAL_OK;
	}
	return HAL_ERROR;
}

#endif

#ifdef BOARD_N64_F4

void BLE_CLEAR_ADV_DATA(void)
{
	BdAddrBufferCounter = 0;
}

#endif

#ifdef BOARD_N64_F4

void BLE_BUFFER_ADDRESSES(uint8_t *pAddr, uint8_t BdAddrType)
{
	uint8_t u8Iter = 0;

	if((BdAddrBufferCounter + 1)  < MAX_NO_OF_DISCOVERABLE_DEVICES)
	{
		for(u8Iter = 0; u8Iter < 6; u8Iter++){
			TempBdAddr[BdAddrBufferCounter][u8Iter] = pAddr[u8Iter];
		}
		TempBdAddrType[BdAddrBufferCounter] = BdAddrType;
	}
}

#endif

#ifdef BOARD_N64_F4

void BLE_GET_DEV_DATA(uint8_t DevNo, BLE_DEV_DATA **DevStruct)
{

	if(DevNo < NoOfDevices){
		*DevStruct = &BLEDeviceData[DevNo];
	}
}

#endif

#ifdef BOARD_SENSORTILE
void BLE_GET_DEV_DATA(BLE_DEV_DATA **DevStruct)
{
	*DevStruct = &BLEDeviceData;
}

#endif
#ifdef BOARD_N64_F4
HAL_StatusTypeDef BLE_CONNECT_TO_DEVICE(uint8_t DevNo, BLE_DEVICE_LOCATION ble_dl)
{
	uint8_t ret;
	if(DevNo < BdAddrBufferCounter){
		ret = aci_gap_create_connection(0x4000, 0x4000, TempBdAddrType[DevNo], TempBdAddr[DevNo],ADDR_TYPE, 9, 9, 0, 60, 1000 , 1000);
		BLE_SET_DISC_DEV_NO(ble_dl);
		if(ret == 0){
				SET_DECODER_STATE(BLE_CONNECTING);
		}
		return (ret == 0)? HAL_OK: HAL_ERROR;
	}
	return HAL_ERROR;
}

#endif


#ifdef BOARD_N64_F4 //we are not scanning on sensortile
HAL_StatusTypeDef BLE_ADV_REPORT_CALLBACK(le_advertising_info *cc)
{
#ifdef DEBUG_BL
	uint8_t whiledebug = 1;
#endif
	uint8_t *Temp_BdAddr;
	uint8_t Temp_BdAddrType;

	uint8_t len;
	uint8_t *ptrData;
	Temp_BdAddr = &cc->bdaddr;
	Temp_BdAddrType = cc->bdaddr_type;
	len = cc->data_length;
	ptrData = cc->data_RSSI;
	if(BLE_CHECK_IF_ADDR_PRESENT(Temp_BdAddr) == COMP_ERROR){
		return HAL_OK;
	}
	switch(cc->evt_type){
	case ADV_IND:
		len = *ptrData;
		ptrData++;
		while (len > 0 && *ptrData != 0 )
		{

			if((*ptrData == 0x8) || (*ptrData == 0x9)){
				ptrData++;
				if((len < MAX_BLE_NAME_LENGTH) && ((BdAddrBufferCounter + 1) < MAX_NO_OF_DISCOVERABLE_DEVICES)){

					BLE_BUFFER_ADDRESSES(Temp_BdAddr, Temp_BdAddrType);
					SendNamesToScreen(ptrData, len - 1, BdAddrBufferCounter);
					BdAddrBufferCounter++;

				}
#ifdef DEBUG_BL
		while(whiledebug){} //pause to debug
#endif
			}
			ptrData = ptrData + len;
			len = *ptrData;
			ptrData++;
		}
		break;
	case SCAN_RSP:
	{
		len = *ptrData;
		ptrData++;
		while(len > 0 && *ptrData != 0)
		{

			if((*ptrData == 0x8) || (*ptrData == 0x9)){
				ptrData++;
				if((len < MAX_BLE_NAME_LENGTH) && ((BdAddrBufferCounter + 1) < MAX_NO_OF_DISCOVERABLE_DEVICES)){

					BLE_BUFFER_ADDRESSES(Temp_BdAddr, Temp_BdAddrType);
					SendNamesToScreen(ptrData, len - 1, BdAddrBufferCounter);
					BdAddrBufferCounter++;

				}
			}

			ptrData = ptrData + len;
			len = *ptrData;
			ptrData++;


		}
		break;
	}
	//
	default:
		//Due to discrepancy in ST documentation other cases will not be implemented yet, though BLE specification makes use of this clear i have no way of testing this now so it will be omitted
		break;
	}
	return HAL_OK;
}

#endif

#ifdef BOARD_N64_F4

tBleStatus BLE_START_DISC_CHAR_OF_SERV(uint16_t ConnHandle)
{
	uint8_t u8Iter = 0;
	tBleStatus ret;
	if(GET_DECODER_STATE() == BLE_IDLE)
	{
		if(BLEDeviceData[MasterDevNo].ConnectionHandle != ConnHandle) /*because we need to operate on a single device, why was it changed during a discovery proc ? (just a precaution)*/
		{
			for(u8Iter = 0; u8Iter < NoOfDevices; u8Iter++)
			{
				if(BLEDeviceData[u8Iter].ConnectionHandle == ConnHandle)
				{
					BLE_SET_DISC_DEV_NO(u8Iter);
				}
			}
		}
		if((BLEDeviceData[MasterDevNo].ConnectionHandle == ConnHandle) &&
			(BLEDeviceData[MasterDevNo].ServicesPresent != 0)
		){
			for(u8Iter = 0; u8Iter<8;u8Iter++)
			{
				if(BLEDeviceData[MasterDevNo].ServicesPresent & (1 << u8Iter)){
					break;
				}
			}
			SET_DECODER_STATE(BLE_DISC_CHAR);
			CharDisc = (1 << u8Iter);
			Service2Discover = u8Iter;
			ret = aci_gatt_disc_all_charac_of_serv(BLEDeviceData[MasterDevNo].ConnectionHandle, BLEDeviceData[MasterDevNo].sServiceIDData[u8Iter].ServiceHandle, BLEDeviceData[MasterDevNo].sServiceIDData[u8Iter].EndGroupHandle);
			return ret;
		}
	}
 return ERR_COMMAND_DISALLOWED;
}

#endif

#ifdef BOARD_N64_F4
void BLE_DISC_SERVICES(uint8_t DevNo)
{
	if(DevNo < NoOfDevices)
	{
		if((GET_DECODER_STATE() == BLE_DISC_SERV) ||(GET_DECODER_STATE() == BLE_IDLE)){
			SET_DECODER_STATE(BLE_DISC_SERV);
			BLE_SET_DISC_DEV_NO(DevNo);
			aci_gatt_disc_all_prim_services(BLEDeviceData[DevNo].ConnectionHandle);
		}
	}
}

#endif

#ifdef BOARD_N64_F4

void BLE_DISC_CONN_SERVICES(uint16_t ConnHandle)
{
	if((GET_DECODER_STATE() == BLE_DISC_SERV) ||(GET_DECODER_STATE() == BLE_IDLE)){
		SET_DECODER_STATE(BLE_DISC_SERV);
		aci_gatt_disc_all_prim_services(ConnHandle);
	}
}

#endif

void BLE_EVNT_CALLBACK(void * pData)
{
	  hci_uart_pckt *hci_pckt = pData;
	  /* obtain event packet */
	  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

	  if(hci_pckt->type != HCI_EVENT_PKT)
	    return;

	  switch(event_pckt->evt){

	  case EVT_DISCONN_COMPLETE:
	    {
	      evt_disconn_complete *cc = (void *)event_pckt->data;
	      //Who disconnected ? if it was initiated by us it most likely was a timeout, if so lets try to reconnect
	      DisconnectionComplete_CB(cc);
	    }
	    break;

	  case EVT_LE_META_EVENT:
	    {
	      evt_le_meta_event *evt = (void *)event_pckt->data;

	      switch(evt->subevent){
	      case EVT_LE_CONN_COMPLETE:
	        {
	          evt_le_connection_complete *cc = (void *)evt->data;
	          SET_DECODER_STATE(BLE_CONNECTING);
	          ConnectionComplete_CB(cc->peer_bdaddr, cc->handle,cc->status, cc->peer_bdaddr_type);
	        }
	        break;
	      case EVT_LE_ADVERTISING_REPORT:
	      {
	    	  le_advertising_info *cc = (void *)evt->data+1;
#ifdef BOARD_N64_F4
	    	  BLE_ADV_REPORT_CALLBACK(cc);
#endif
#ifdef BOARD_SENSORTILE
	    	  UNUSED(cc);
#endif
//			  HAL_Delay(10);
	      }
		  break;

	      }
	    }
	    break;

	  case EVT_VENDOR:
	    {
	      evt_blue_aci *blue_evt = (void*)event_pckt->data;
	      switch(blue_evt->ecode){
	      case EVT_BLUE_GATT_TX_POOL_AVAILABLE:
	      {
	    	  BLE_TX_POOL_FREE = 0;
	    	  break;
	      }
	      case EVT_BLUE_GATT_PROCEDURE_TIMEOUT:
	      {
	    	  // Basically dead, we can disconnect and reconnect later
	    	  //TODO check if there is a better way to do this, maybe update link ?
	    	  evt_gatt_procedure_timeout *cc = (void*)blue_evt->data;
	    	  aci_gap_terminate(cc->conn_handle, ERR_CONNECTION_TIMEOUT);
	    	  break;
	      }
	      case EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP:
	       {
	    	   evt_att_read_by_group_resp *cc = (void*)blue_evt->data;
	    	   BLE_DECODE_GROUP_RESP(cc);
	       }
	       break;
	      case EVT_BLUE_ATT_READ_BY_TYPE_RESP:
	      {
	    	  evt_att_read_by_type_resp *cc = (void*)blue_evt->data;
	    	  BLE_DECODE_GROUP_RESP((evt_att_read_by_group_resp*)cc);
	      }
	      break;
#ifdef BOARD_SENSORTILE
	      case EVT_BLUE_GATT_READ_PERMIT_REQ:
	        {
	          evt_gatt_read_permit_req *pr = (void*)blue_evt->data;
	          UNUSED(pr);
//	          BLE_READ_REQUEST_CB(pr->attr_handle);
	          //TODO Implement this
	        }
	        break;
	      case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
	        {
	          evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*)blue_evt->data;
	          BLE_ATTR_MODIFIED_CB(evt->attr_handle, evt->att_data,evt->data_length);
	          //TODO Implement this
	        }
	        break;
#endif
	      case EVT_BLUE_GATT_ERROR_RESP:
	      {
	    	  evt_gatt_error_resp *cc = (void*)blue_evt->data;
	    	  UNUSED(cc);
//	    	  aci_gap_terminate_gap_procedure();
//	    	  SET_DECODER_STATE(BLE_IDLE); // 0x10
#ifdef BOARD_N64_F4
	    	  /*
	    	   * Perhaps someday i'll try to handle errors, but today is not that day
	    	   */
//	    	  if((GET_DECODER_STATE() == BLE_DISC_SERV) && (cc->req_opcode == 0x10) && (cc->error_code == 0xA)){
//					if(BLE_DISC_ALL_SERVICES_MASTER_STATE == BLE_DISC_ALL_SERVICES_ON){
//						if(MasterDevNo < NoOfDevices - 1){
//							BLE_SET_DISC_DEV_NO(MasterDevNo + 1);
//							BLE_DISC_SERVICES(MasterDevNo);
//						}
//						else{
//							BLE_DISC_ALL_SERVICES_MASTER_STATE = BLE_DISC_ALL_SERVICES_OFF;
//  //		    				SET_DECODER_STATE(BLE_IDLE);
//  //		    				TODO check this
////							GATT_DISC_CPLT(cc->conn_handle);
//						}
//					}
//					else{
////							GATT_DISC_CPLT(cc->conn_handle);
//						}
//				}
#endif

	      }
	      break;
	      case EVT_BLUE_ATT_READ_MULTIPLE_RESP:
	      {
	    	  evt_att_read_mult_resp *cc = (void*)blue_evt->data;
	    	  UNUSED(cc);

//TODO I think this should be implemented, it could greatly increase speed
	      }
	      break;
	      case EVT_BLUE_ATT_READ_RESP:
	      {
	    	  evt_att_read_resp *cc = (void*)blue_evt->data;
#ifdef BOARD_N64_F4
	    	  ATT_READ_RESP(cc);
#endif
#ifdef BOARD_SENSORTILE
	    	  UNUSED(cc);
#endif
	      }
	      break;
	      case	EVT_BLUE_GATT_NOTIFICATION:
	      {
	    	  evt_gatt_attr_notification *cc = (void*)blue_evt->data;
	    	  UNUSED(cc);
	      }
	      break;
	      case EVT_BLUE_GATT_PROCEDURE_COMPLETE:
		    {
		    	evt_gatt_procedure_complete *pr = (void*)blue_evt->data;
#ifdef BOARD_N64_F4
		    	if(GET_DECODER_STATE() == BLE_DISC_SERV){
		    		if(BLE_DISC_ALL_SERVICES_MASTER_STATE == BLE_DISC_ALL_SERVICES_ON){
		    			if(MasterDevNo < NoOfDevices - 1){
		    				BLE_SET_DISC_DEV_NO(MasterDevNo + 1);
		    				BLE_DISC_SERVICES(MasterDevNo);
		    			}
		    			else{
		    				BLE_DISC_ALL_SERVICES_MASTER_STATE = BLE_DISC_ALL_SERVICES_OFF;
//		    				SET_DECODER_STATE(BLE_IDLE);
//		    				TODO check this
		    				GATT_DISC_CPLT(pr->conn_handle);
		    			}

		    		}
		    		else{
		    			GATT_DISC_CPLT(pr->conn_handle);
		    		}
		    	}
		    	else if(GET_DECODER_STATE() == BLE_DISC_CHAR)
		    	{
		    		GATT_DISC_CPLT(pr->conn_handle);
		    	}

#endif
		    	UNUSED(pr);
		    	if(CpltCallback != NULL){
		    		SET_DECODER_STATE(BLE_IDLE);
		    		CpltCallback();
		    		CpltCallback = NULL;
			  }
	        }
	    	break;
		   case EVT_BLUE_GAP_PROCEDURE_COMPLETE:
			{
			  evt_gap_procedure_complete *pr = (void*)blue_evt->data;
			  switch(pr->procedure_code) {
			  case GAP_LIMITED_DISCOVERY_PROC:
			  case GAP_GENERAL_DISCOVERY_PROC:
				  SET_DECODER_STATE(BLE_IDLE);
				  if(CpltCallback != NULL){
					  CpltCallback();
					  CpltCallback = NULL;
					  SET_DECODER_STATE(BLE_IDLE);
				  }
				  break;
			  case GAP_DIRECT_CONNECTION_ESTABLISHMENT_PROC:
				  SET_DECODER_STATE(BLE_IDLE);
//				  ConnectionComplete_CB(NULL,pr->status);
				  }
				}
				break;
		   case EVT_BLUE_GATT_SERVER_CONFIRMATION_EVENT:
		   {
			   SET_DECODER_STATE(BLE_IDLE);
			   if(CpltCallback != NULL){
				  CpltCallback = (VoidFuncPointer) CpltCallback();
			   }
		   }
	      }

	    }
	    break;
	  }
}

void CLR_CALLBACK_FUNC(void)
{
	CpltCallback = NULL;
}
#ifdef BOARD_N64_F4



void GATT_DISC_CPLT(uint16_t handle)
{
	//TODO I need to add BdAddrType to BLEDeviceData. it is needed during reconnection
	uint8_t u8Iter = 0;
	if(BLEDeviceData[MasterDevNo].ConnectionHandle == handle){
		switch(GET_DECODER_STATE())
		{
		case BLE_DISC_SERV:
			SET_DECODER_STATE(BLE_IDLE);
			if(BLE_START_DISC_CHAR_OF_SERV(handle) == ERR_COMMAND_DISALLOWED)
			{
				DISC_CPLT(handle);
			}
			break;
		case BLE_DISC_CHAR:
			if(CharDisc == BLEDeviceData[MasterDevNo].ServicesPresent){
				SET_DECODER_STATE(BLE_IDLE);
				DISC_CPLT(handle);
			}
			else{
				for(u8Iter = 7; u8Iter>=0;u8Iter--)
				{
					if (CharDisc & (1<<u8Iter))
					{
						u8Iter++;
						break;
					}
				}
				for(; u8Iter<8; u8Iter++){
					if(BLEDeviceData[MasterDevNo].ServicesPresent & (1 << u8Iter)){
						break;
					}
				}
				CharDisc = CharDisc | (1 << u8Iter);
				aci_gatt_disc_all_charac_of_serv(BLEDeviceData[MasterDevNo].ConnectionHandle, BLEDeviceData[MasterDevNo].sServiceIDData[u8Iter].ServiceHandle, BLEDeviceData[MasterDevNo].sServiceIDData[u8Iter].EndGroupHandle);
			}
			break;
		default:
			break;
		}

	}

}

#endif

#ifdef BOARD_N64_F4
void DISC_CPLT(uint16_t handle)
{
	uint8_t u8AllMand_present = 0;
	if(BLEDeviceData[MasterDevNo].ConnectionHandle != handle) /*because we need to operate on a single device, why was it changed during a discovery proc ? (just a precaution)*/
	{
		for(u8AllMand_present = 0; u8AllMand_present < NoOfDevices; u8AllMand_present++)
		{
			if(BLEDeviceData[u8AllMand_present].ConnectionHandle == handle)
			{
				BLE_SET_DISC_DEV_NO(u8AllMand_present);
			}
		}
	}
	u8AllMand_present = 0;
#ifdef SENS_SERVICE_MANDATORY
	if (!(BLEDeviceData[MasterDevNo].ServicesPresent & (1<<BLE_SENS_SERVICE))){ u8AllMand_present = 1;}
#endif
#ifdef LED_SERVICE_MANDATORY
	if (!(BLEDeviceData[MasterDevNo].ServicesPresent & (1<<BLE_LED_SERVICE))){ u8AllMand_present = 1;}
#endif
#ifdef SD_SERVICE_MANDATORY
	if (!(BLEDeviceData[MasterDevNo].ServicesPresent & (1<<BLE_SD_SERVICE))){ u8AllMand_present = 1;}
#endif


	if(u8AllMand_present == 0){
		SendConnectionFeedback(BLE_CONNCETION_SUCCESFUL);
	}
	else{
		aci_gap_terminate(handle, 0x16);
		SendConnectionFeedback(BLE_CONNECTION_FAILED);
	}
}

#endif
void DisconnectionComplete_CB(evt_disconn_complete *evt_data)
{
#if defined(USE_RTC)
	RTC_HandleTypeDef RtcHandle;
	uint32_t BLE_RTC_STATUS = 0;
#endif
#ifdef BOARD_N64_F4
	uint8_t __DevNo_DCCB;
#endif
	//Did we disconnect on purpose ?
	if(evt_data->reason == ERR_CONNECTION_TIMEOUT){
#ifdef BOARD_N64_F4
		//We should initiate a connection to that device
		//TODO This seems stupid, there has got to be be a better way
		//To be done on a central device
		SET_DECODER_STATE(BLE_IDLE); //Set State to idle because we need to change it later to reconnection
		if (BLE_GET_DEV_NO_BY_HANDLE(evt_data->handle, &__DevNo_DCCB) == HAL_OK){
			aci_gap_create_connection(0x4000, 0x4000, BLEDeviceData[__DevNo_DCCB].BdAddrType, BLEDeviceData[__DevNo_DCCB].BdAddr, ADDR_TYPE, 9, 9, 0, 60, 1000 , 1000);
			SET_DECODER_STATE(BLE_RECONNECTING);
			return 0;
		}
		else{
			/* TODO Debug code here
			 * possibly handle the error
			 */
		}
#endif

	}
	SET_DECODER_STATE(BLE_IDLE);
#ifdef BOARD_SENSORTILE
	connected = 0;
	BLE_SET_CONNECTABLE();
	PurgeSerialBuffer();
#if defined(USE_RTC)
	RtcHandle.Instance = RTC;
	BLE_RTC_STATUS = HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BACKUP_BLE_STATE_REG);
	HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BACKUP_BLE_STATE_REG, ( BLE_RTC_STATUS & ~RTC_BACKUP_BLE_CONNECTED ));
#endif
#endif
}

#ifdef BOARD_N64_F4
HAL_StatusTypeDef BLE_GET_DEV_NO_BY_HANDLE(uint16_t ConnHandle, uint8_t *__DevNo_BDH)
{
	uint8_t u8iter = 0;
	for(u8iter=0; u8iter<NoOfDevices; u8iter++)
	{
		if(BLEDeviceData[u8iter].ConnectionHandle == ConnHandle){
			*__DevNo_BDH = u8iter;
			return HAL_OK;
		}
	}
	return HAL_ERROR;
}
#endif

void ConnectionComplete_CB(uint8_t addr[6], uint16_t handle,uint8_t Status, uint8_t addr_type)
{
#if defined(USE_RTC)
	RTC_HandleTypeDef RtcHandle;
	uint32_t BLE_RTC_STATUS = 0;
#endif
#ifdef BOARD_N64_F4
	if(Status == 0){
		if(GET_DECODER_STATE() != BLE_RECONNECTING){
			BLEDeviceData[MasterDevNo].BdAddr[0] = addr[0];
			BLEDeviceData[MasterDevNo].BdAddr[1] = addr[1];
			BLEDeviceData[MasterDevNo].BdAddr[2] = addr[2];
			BLEDeviceData[MasterDevNo].BdAddr[3] = addr[3];
			BLEDeviceData[MasterDevNo].BdAddr[4] = addr[4];
			BLEDeviceData[MasterDevNo].BdAddr[5] = addr[5];
			BLEDeviceData[MasterDevNo].ConnectionHandle = handle;
			BLEDeviceData[MasterDevNo].BdAddrType = addr_type;
			BLE_DISC_CONN_SERVICES(handle);
		}
	}
	else{
		SendConnectionFeedback(BLE_CONNECTION_FAILED);
	}
#endif
#ifdef BOARD_SENSORTILE
//	BLEDeviceData.BdAddr[0] = addr[0];
//	BLEDeviceData.BdAddr[1] = addr[1];
//	BLEDeviceData.BdAddr[2] = addr[2];
//	BLEDeviceData.BdAddr[3] = addr[3];
//	BLEDeviceData.BdAddr[4] = addr[4];
//	BLEDeviceData.BdAddr[5] = addr[5];
#endif

	UNUSED(addr);
	UNUSED(handle);
	UNUSED(Status);
#ifdef BOARD_SENSORTILE
	connected = 0x10;
#endif
	CLR_CALLBACK_FUNC();
	aci_gap_set_non_discoverable();
#if defined(USE_RTC)
	RtcHandle.Instance = RTC;
	BLE_RTC_STATUS = HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BACKUP_BLE_STATE_REG);
	HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BACKUP_BLE_STATE_REG, ( BLE_RTC_STATUS | RTC_BACKUP_BLE_CONNECTED ));
#endif
//	SET_DECODER_STATE(BLE_IDLE);
}

#ifdef BOARD_N64_F4

HAL_StatusTypeDef BLE_START_SCAN(VoidFuncPointer Callback)
{

 	if(GET_DECODER_STATE() == BLE_IDLE){
		BLE_CLEAR_ADV_DATA();
		CpltCallback = Callback;
		return (aci_gap_start_general_discovery_proc(0x4000,0x4000,ADDR_TYPE,1) == BLE_STATUS_SUCCESS)? HAL_OK:HAL_ERROR;
	}
	return HAL_ERROR;

}

#endif

HAL_StatusTypeDef BLE_WRITE_CHAR(uint8_t DevNo, ServiceIDData *Service, CharIDData *Characteristic, uint8_t len, uint8_t *pData, VoidFuncPointer Callback)
{

	if(GET_DECODER_STATE() == BLE_IDLE){
		CpltCallback = Callback;
#ifdef BOARD_N64_F4
		return (aci_gatt_write_charac_value(BLEDeviceData[DevNo].ConnectionHandle , Characteristic->CharValueHandle, len, pData) == BLE_STATUS_SUCCESS)? HAL_OK:HAL_ERROR;
#else

#endif
	}
	return HAL_ERROR;

}

void BLE_INIT_SPEC(void)
{
	uint8_t  hwVersion;
	uint16_t fwVersion;
#if defined(USE_RTC)
	RTC_HandleTypeDef RtcHandle;
	uint32_t BLE_RTC_STATUS = 0;
#endif

#ifdef RAND_BLE_ADDRESS
	uint8_t bdaddr[BDADDR_SIZE];
#else
	uint8_t bdaddr[BDADDR_SIZE] = {BLE_BDADDR};
#endif
#ifdef DEBUG_BL_INIT
	int ret;
#endif

	//TODO Debug this, little or big endian ? Check at runtime

#ifdef BOARD_N64_F4
	typedef const struct {
		char name[sizeof(STATION_NAME)];
		char local;
	}lname;
	lname sname = {.local = AD_TYPE_COMPLETE_LOCAL_NAME, .name = STATION_NAME};

	union {
		lname sname;
		char cname;
	}uname = {.sname = sname};

#else
	typedef const struct {
		char local;
		char name[sizeof(ST_BL_NAME)];
	}lname;
	lname sname = {.local = AD_TYPE_COMPLETE_LOCAL_NAME, .name = ST_BL_NAME};

	union {
		lname sname;
		char cname;
	}uname = {.sname = sname};

//	const char *name = ST_BL_NAME;

#endif


	uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
	memset(&BLEDeviceData, 0, sizeof(BLE_DEV_DATA));
	hci_init(BLE_EVNT_CALLBACK, NULL);
	hci_gpio_reset();
	getBlueNRGVersion(&hwVersion, &fwVersion);
	hci_reset();
	HAL_Delay(100);

#ifdef RAND_BLE_ADDRESS
	bdaddr[0] = (STM32_UUID[1]>>24)&0xFF;
	bdaddr[1] = (STM32_UUID[0]    )&0xFF;
	bdaddr[2] = (STM32_UUID[2] >>8)&0xFF;
	bdaddr[3] = (STM32_UUID[0]>>16)&0xFF;
	bdaddr[4] = ((((fwVersion & 0xff00) >> 8)*10) + ((fwVersion & 0xf0) >> 4)+100)&0xFF;
	bdaddr[5] = 0xC0;
#endif
#ifdef BOARD_SENSORTILE
	BLEDeviceData.BdAddr[0] = bdaddr[0];
	BLEDeviceData.BdAddr[1] = bdaddr[1];
	BLEDeviceData.BdAddr[2] = bdaddr[2];
	BLEDeviceData.BdAddr[3] = bdaddr[3];
	BLEDeviceData.BdAddr[4] = bdaddr[4];
	BLEDeviceData.BdAddr[5] = bdaddr[5];
#endif
//	HAL_Delay(100);
#ifdef DEBUG_BL_INIT
	ret =
#endif
#ifdef BOARD_N64_F4
	aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
	                                  CONFIG_DATA_PUBADDR_LEN,
	                                  bdaddr);
#endif


#ifdef DEBUG_BL_INIT
		ret =
#endif
	aci_gatt_init();

#ifdef BOARD_N64_F4
	#ifdef DEBUG_BL_INIT
		ret =
	#endif
	 aci_gap_init_IDB05A1(GAP_CENTRAL_ROLE_IDB05A1, 0, 0x07, &service_handle, &dev_name_char_handle, &appearance_char_handle);
#else
	#ifdef DEBUG_BL_INIT
		ret =
	#endif
	aci_gap_init_IDB05A1(GAP_PERIPHERAL_ROLE_IDB05A1, 0, 0x07, &service_handle, &dev_name_char_handle, &appearance_char_handle);
#endif


#ifdef BOARD_SENSORTILE
	hci_le_set_random_address(bdaddr);
	aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0,
	                                   strlen(&uname.cname + 1), (uint8_t *)(&uname.cname + 1));
#endif

#ifdef DEBUG_BL_INIT
	ret =
#endif
	/*TODO Check if the following code is enough to enable secure connection */
	aci_gap_set_auth_requirement(MITM_PROTECTION_REQUIRED,
	                                     OOB_AUTH_DATA_ABSENT,
	                                     NULL,
	                                     7,
	                                     16,
	                                     USE_FIXED_PIN_FOR_PAIRING,
	                                     BLE_PIN,
	                                     BONDING);
#ifdef DEBUG_BL_INIT
	ret =
#endif
	aci_hal_set_tx_power_level(0,2);

#ifdef BOARD_SENSORTILE


#endif

#ifdef DEBUG_BL_INIT
	while(ret){};
#endif

#ifdef BOARD_SENSORTILE
	BLE_SET_CONNECTABLE();
#endif
#if defined(USE_RTC)
	RtcHandle.Instance = RTC;
	BLE_RTC_STATUS = HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BACKUP_BLE_STATE_REG);
	BLE_RTC_STATUS |=  RTC_BACKUP_BLE_INITIALIZED;
	BLE_RTC_STATUS &= ~RTC_BACKUP_BLE_CONNECTED;
	HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BACKUP_BLE_STATE_REG, BLE_RTC_STATUS);
#endif
}

#ifdef BOARD_N64_F4
HAL_StatusTypeDef BLE_READ_CHAR(uint16_t ConnHandle, uint8_t *pData, uint8_t len, CharIDData *Characteristic, VoidFuncPointer Callback)
{
	if((GET_DECODER_STATE() == BLE_IDLE) && (CpltCallback == NULL) )
	{
		CpltCallback = Callback;
		sBLE_DATA_READ_INFO.pData  = pData;
		sBLE_DATA_READ_INFO.len = len;
		SET_DECODER_STATE(BLE_READ);
		aci_gatt_read_charac_val(ConnHandle, Characteristic->CharValueHandle);
		return HAL_OK;
	}
	else if ((GET_DECODER_STATE() == BLE_IDLE) && (Callback == NULL))
	{
		sBLE_DATA_READ_INFO.pData  = pData;
		sBLE_DATA_READ_INFO.len = len;
		SET_DECODER_STATE(BLE_READ);
		aci_gatt_read_charac_val(ConnHandle, Characteristic->CharValueHandle);
		return HAL_OK;
	}
	return HAL_ERROR;
}

HAL_StatusTypeDef BLE_READ_MULTIPLE_CHAR(uint16_t ConnHandle, uint8_t *pData, uint8_t len,  uint8_t *Characteristic, uint8_t NoOfhandles, VoidFuncPointer Callback)
{
	if((GET_DECODER_STATE() == BLE_IDLE) && (CpltCallback == NULL) )
	{
		if(NoOfhandles <= MAX_CHAR_NO_TO_READ){
			CpltCallback = Callback;
			sBLE_DATA_READ_INFO.pData  = pData;
			sBLE_DATA_READ_INFO.len = len;
			SET_DECODER_STATE(BLE_READ);
			aci_gatt_read_multiple_charac_val(ConnHandle, NoOfhandles, Characteristic);
			return HAL_OK;
		}

	}
	else if ((GET_DECODER_STATE() == BLE_IDLE) && (Callback == NULL))
	{
		if(NoOfhandles <= MAX_CHAR_NO_TO_READ){
			sBLE_DATA_READ_INFO.pData  = pData;
			sBLE_DATA_READ_INFO.len = len;
			SET_DECODER_STATE(BLE_READ);
			aci_gatt_read_multiple_charac_val(ConnHandle, NoOfhandles, Characteristic);
			return HAL_OK;
		}
	}
	return HAL_ERROR;
}
#endif

#ifdef BOARD_SENSORTILE

tBleStatus BLE_ADD_SERVICES(void)
{
	tBleStatus ret;
	uint16_t char_desc;
	uint16_t temp = CHAR_USER_DESC_UUID;

/*Sens service*/

	ret =  aci_gatt_add_serv(UUID_TYPE_128,  UUID_SERVICE_DATA[BLE_SENS_SERVICE], PRIMARY_SERVICE,
								1+(2 + 1 + 2) * 2,                                     /*service attr + (desc + inc + val)*char_no*/
								&BLEDeviceData.sServiceIDData[BLE_SENS_SERVICE].ServiceHandle);

#ifdef DEBUG_BL_INIT
	if(ret != 0)
	{
		while(1)
	}
#endif
//TODO check if this will stand


	ret =  aci_gatt_add_char(BLEDeviceData.sServiceIDData[BLE_SENS_SERVICE].ServiceHandle, UUID_TYPE_128, UUID_CHAR_DATA[BLE_ACC_CHAR], 18,
								CHAR_PROP_NOTIFY|CHAR_PROP_READ,
								ATTR_PERMISSION_NONE,
								0,
								16, 1, &BLEDeviceData.sCharIDData[BLE_ACC_CHAR].CharHandle);
	BLEDeviceData.sCharIDData[BLE_ACC_CHAR].CharValueHandle = BLEDeviceData.sCharIDData[BLE_ACC_CHAR].CharHandle + 2;
	ret = aci_gatt_add_char_desc(
								 BLEDeviceData.sServiceIDData[BLE_SENS_SERVICE].ServiceHandle,
								 BLEDeviceData.sCharIDData[BLE_ACC_CHAR].CharHandle,
								 UUID_TYPE_16,
								 (uint8_t*)&temp,
							     10,
								 7,
								 "ACC_RD",
								 ATTR_PERMISSION_NONE,
								 ATTR_ACCESS_READ_ONLY,
								 0,
								 16,
								 0,
								 &char_desc);
#ifdef DEBUG_BL_INIT
	if(ret != 0)
	{
		while(1)
	}
#endif
	ret =  aci_gatt_add_char(BLEDeviceData.sServiceIDData[BLE_SENS_SERVICE].ServiceHandle, UUID_TYPE_128, UUID_CHAR_DATA[BLE_GYRO_CHAR], (3*4),
								CHAR_PROP_NOTIFY|CHAR_PROP_READ,
								ATTR_PERMISSION_NONE,
								0,
								16, 0, &BLEDeviceData.sCharIDData[BLE_GYRO_CHAR].CharHandle);
	BLEDeviceData.sCharIDData[BLE_GYRO_CHAR].CharValueHandle = BLEDeviceData.sCharIDData[BLE_GYRO_CHAR].CharHandle + 2;
#ifdef DEBUG_BL_INIT
	if(ret != 0)
	{
		while(1)
	}
#endif
/*SD service*/
	ret = aci_gatt_add_serv(UUID_TYPE_128,  UUID_SERVICE_DATA[BLE_SD_SERVICE], PRIMARY_SERVICE,
			1+(1 + 1 +2) * 2,
								&BLEDeviceData.sServiceIDData[BLE_SD_SERVICE].ServiceHandle);
#ifdef DEBUG_BL_INIT
	if(ret != 0)
	{
		while(1)
	}
#endif
	ret =  aci_gatt_add_char(BLEDeviceData.sServiceIDData[BLE_SD_SERVICE].ServiceHandle, UUID_TYPE_128, UUID_CHAR_DATA[BLE_SD_READ_CHAR], 1,
								CHAR_PROP_NOTIFY|CHAR_PROP_READ,
								ATTR_PERMISSION_NONE,
								0,
								16, 0, &BLEDeviceData.sCharIDData[BLE_SD_READ_CHAR].CharHandle);
#ifdef DEBUG_BL_INIT
	if(ret != 0)
	{
		while(1)
	}
#endif
	ret =  aci_gatt_add_char(BLEDeviceData.sServiceIDData[BLE_SD_SERVICE].ServiceHandle, UUID_TYPE_128, UUID_CHAR_DATA[BLE_SD_CONF_CHAR], 1,
								CHAR_PROP_NOTIFY| CHAR_PROP_WRITE_WITHOUT_RESP,
								ATTR_PERMISSION_NONE,
								GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
								16, 0, &BLEDeviceData.sCharIDData[BLE_SD_CONF_CHAR].CharHandle);
#ifdef DEBUG_BL_INIT
	if(ret != 0)
	{
		while(1)
	}
#endif

/* LED service*/

	ret = aci_gatt_add_serv(UUID_TYPE_128,  UUID_SERVICE_DATA[BLE_LED_SERVICE], PRIMARY_SERVICE,
			1+(1 + 1 +2) * 2,
								&BLEDeviceData.sServiceIDData[BLE_LED_SERVICE].ServiceHandle);
#ifdef DEBUG_BL_INIT
	if(ret != 0)
	{
		while(1)
	}
#endif

	ret =  aci_gatt_add_char(BLEDeviceData.sServiceIDData[BLE_LED_SERVICE].ServiceHandle, UUID_TYPE_128, UUID_CHAR_DATA[BLE_LED_READ_CHAR], 1,
								CHAR_PROP_NOTIFY|CHAR_PROP_READ,
								ATTR_PERMISSION_NONE,
								0,
								16, 0, &BLEDeviceData.sCharIDData[BLE_LED_READ_CHAR].CharHandle);
#ifdef DEBUG_BL_INIT
	if(ret != 0)
	{
		while(1)
	}
#endif

	ret =  aci_gatt_add_char(BLEDeviceData.sServiceIDData[BLE_LED_SERVICE].ServiceHandle, UUID_TYPE_128, UUID_CHAR_DATA[BLE_LED_CONF_CHAR], 1,
								CHAR_PROP_NOTIFY| CHAR_PROP_WRITE_WITHOUT_RESP,
								ATTR_PERMISSION_NONE,
								GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
								16, 0, &BLEDeviceData.sCharIDData[BLE_LED_CONF_CHAR].CharHandle);
#ifdef DEBUG_BL_INIT
	if(ret != 0)
	{
		while(1)
	}
#endif
/* SENSOR config service */
	ret = aci_gatt_add_serv(UUID_TYPE_128,  UUID_SERVICE_DATA[BLE_CONF_SERVICE], PRIMARY_SERVICE,
			1+(1 + 1 +2) * 2,
								&BLEDeviceData.sServiceIDData[BLE_LED_SERVICE].ServiceHandle);
#ifdef DEBUG_BL_INIT
	if(ret != 0)
	{
		while(1)
	}
#endif
	ret =  aci_gatt_add_char(BLEDeviceData.sServiceIDData[BLE_LED_SERVICE].ServiceHandle, UUID_TYPE_128, UUID_CHAR_DATA[BLE_LED_READ_CHAR], 1,
								CHAR_PROP_NOTIFY|CHAR_PROP_READ,
								ATTR_PERMISSION_NONE,
								0,
								16, 0, &BLEDeviceData.sCharIDData[BLE_LED_READ_CHAR].CharHandle);
#ifdef DEBUG_BL_INIT
	if(ret != 0)
	{
		while(1)
	}
#endif
	ret =  aci_gatt_add_char(BLEDeviceData.sServiceIDData[BLE_LED_SERVICE].ServiceHandle, UUID_TYPE_128, UUID_CHAR_DATA[BLE_LED_CONF_CHAR], 1,
								CHAR_PROP_NOTIFY| CHAR_PROP_WRITE_WITHOUT_RESP,
								ATTR_PERMISSION_NONE,
								GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
								16, 0, &BLEDeviceData.sCharIDData[BLE_LED_CONF_CHAR].CharHandle);
#ifdef DEBUG_BL_INIT
	if(ret != 0)
	{
		while(1)
	}
#endif
	/* Serial service */
		ret = aci_gatt_add_serv(UUID_TYPE_128,  UUID_SERVICE_DATA[BLE_SERIAL_SERVICE], PRIMARY_SERVICE,
				1 + (1 + 2 + 1) * 2,
									&BLEDeviceData.sServiceIDData[BLE_SERIAL_SERVICE].ServiceHandle);
	#ifdef DEBUG_BL_INIT
		if(ret != 0)
		{
			while(1)
		}
	#endif
		ret =  aci_gatt_add_char(BLEDeviceData.sServiceIDData[BLE_SERIAL_SERVICE].ServiceHandle, UUID_TYPE_128, UUID_CHAR_DATA[BLE_SERIAL_RD_CHAR], 35,
//									CHAR_PROP_NOTIFY,
									CHAR_PROP_INDICATE,
									ATTR_PERMISSION_NONE,
									0,
									16, 1, &BLEDeviceData.sCharIDData[BLE_SERIAL_RD_CHAR].CharHandle);
		BLEDeviceData.sCharIDData[BLE_SERIAL_RD_CHAR].CharValueHandle = BLEDeviceData.sCharIDData[BLE_SERIAL_RD_CHAR].CharHandle + 1;
		ret =  aci_gatt_add_char(BLEDeviceData.sServiceIDData[BLE_SERIAL_SERVICE].ServiceHandle, UUID_TYPE_128, UUID_CHAR_DATA[BLE_SERIAL_WR_CHAR], 35,
									CHAR_PROP_WRITE_WITHOUT_RESP|CHAR_PROP_WRITE,
									ATTR_PERMISSION_NONE,
									GATT_NOTIFY_ATTRIBUTE_WRITE,
									16, 1, &BLEDeviceData.sCharIDData[BLE_SERIAL_WR_CHAR].CharHandle);
		BLEDeviceData.sCharIDData[BLE_SERIAL_WR_CHAR].CharValueHandle = BLEDeviceData.sCharIDData[BLE_SERIAL_WR_CHAR].CharHandle + 1;
	#ifdef DEBUG_BL_INIT
		if(ret != 0)
		{
			while(1)
		}
	#endif
		BLEDeviceData.sCharIDData[BLE_ACC_CHAR].CharValueHandle = BLEDeviceData.sCharIDData[BLE_ACC_CHAR].CharHandle + 2;
//		ret = aci_gatt_add_char_desc(
//									 BLEDeviceData.sServiceIDData[BLE_SENS_SERVICE].ServiceHandle,
//									 BLEDeviceData.sCharIDData[BLE_ACC_CHAR].CharHandle,
//									 UUID_TYPE_16,
//									 (uint8_t*)&temp,
//								     10,
//									 7,
//									 "ACC_RD",
//									 ATTR_PERMISSION_NONE,
//									 ATTR_ACCESS_READ_ONLY,
//									 0,
//									 16,
//									 0,
//									 &char_desc);
	#ifdef DEBUG_BL_INIT
		if(ret != 0)
		{
			while(1)
		}
	#endif
return ret;
}

void BLE_SET_CONNECTABLE(void)
{
	typedef const struct {
		char local;
		char name[sizeof(ST_BL_NAME)];
	}lname;
	lname sname = {.local = AD_TYPE_COMPLETE_LOCAL_NAME, .name = ST_BL_NAME};

	union {
		lname sname;
		char cname;
	}uname = {.sname = sname};

	  uint8_t manuf_data[26] = {
	    2,0x0A,0x00 /* 0 dBm */, // Trasmission Power
//	    8,0x09,NAME_STLBLE, // Complete Name
	    13,0xFF,0x01/*SKD version */,
	    0x02,
	    0x00, /* */
	    0xE0, /* ACC+Gyro+Mag*/
	    0x00, /*  */
	    0x00, /*  */
	    0x00, /* BLE MAC start */
	    0x00,
	    0x00,
	    0x00,
	    0x00,
	    0x00, /* BLE MAC stop */
	  };

	  /* BLE MAC */
	  manuf_data[20] = BLEDeviceData.BdAddr[5];
	  manuf_data[21] = BLEDeviceData.BdAddr[4];
	  manuf_data[22] = BLEDeviceData.BdAddr[3];
	  manuf_data[23] = BLEDeviceData.BdAddr[2];
	  manuf_data[24] = BLEDeviceData.BdAddr[1];
	  manuf_data[25] = BLEDeviceData.BdAddr[0];



	hci_le_set_scan_resp_data(0,NULL);
	#ifdef DEBUG_BL_INIT
		ret =
	#endif


	aci_gap_set_discoverable(ADV_DATA_TYPE, ADV_INTERV_MIN, ADV_INTERV_MAX, ADDR_TYPE,
		                                 NO_WHITE_LIST_USE, sizeof(uname), &uname.cname, 0, NULL, 0xFFFF, 0xFFFF);
	aci_gap_update_adv_data(26, manuf_data);
}

HAL_StatusTypeDef BLE_WAIT_FOR_TX_POOL(void)
{
	BLE_TX_POOL_FREE = 1;
	while(BLE_TX_POOL_FREE == 1)
	{
	    if(HCI_ProcessEvent)
	    {
	      HCI_ProcessEvent=0;
	      hci_user_evt_proc();
	    }
	}
	return HAL_OK;
}

HAL_StatusTypeDef BLE_UPDATE_CHAR(ServiceIDData *Service, CharIDData *Characteristic, uint8_t len, uint8_t *pData, VoidFuncPointer Callback)
{
	uint8_t ret;
	if(CpltCallback == NULL)
	{
		CpltCallback = Callback;
	}
	else if (CpltCallback == Callback)
	{
		CpltCallback = Callback;
	}
	else if(Callback != NULL)
	{
		return HAL_ERROR;
	}
	SET_DECODER_STATE(BLE_WAIT_FOR_IND);
	ret = BLE_STATUS_INSUFFICIENT_RESOURCES;
	while (ret == BLE_STATUS_INSUFFICIENT_RESOURCES)
	{
		ret = aci_gatt_update_char_value(Service->ServiceHandle, Characteristic->CharHandle, 0, len, pData);
		if (ret == BLE_STATUS_INSUFFICIENT_RESOURCES)
		{
			BLE_WAIT_FOR_TX_POOL();
		}
		else if (ret != BLE_STATUS_SUCCESS)
		{
			/*
			 * Sometimes we get here
			 * Sometimes that is ok
			 * But not always
			 * In any case while(1) is the worst thing that could be put here
			 * So I probably should do something about it
			 */
			//TODO fix this, this should not be like this

			while(1);
		}
	}

	return (ret == BLE_STATUS_SUCCESS)?HAL_OK:HAL_ERROR;
}

#endif
#ifdef BOARD_SENSORTILE
void BLE_ATTR_MODIFIED_CB(uint16_t attr_handle, uint8_t * att_data, uint8_t data_length)
{

	if(attr_handle == BLEDeviceData.sCharIDData[BLE_ACC_CHAR].CharValueHandle)
	{
		BLEDeviceData.sCharIDData[BLE_ACC_CHAR].CharProperties = att_data[0] | (att_data[1] << 8);
	}
	else if(attr_handle == BLEDeviceData.sCharIDData[BLE_SERIAL_WR_CHAR].CharValueHandle)
	{
		switch(att_data[0])
		{
			case '0':
				SetNextClkPreset(NO_PLL_100kHz_CLK);
				break;
			case '1':
				SetNextClkPreset(NO_PLL_200kHz_CLK);
				break;
			case '2':
				SetNextClkPreset(NO_PLL_400kHz_CLK);
				break;
			case '3':
				SetNextClkPreset(NO_PLL_800kHz_CLK);
				break;
			case '4':
				SetNextClkPreset(NO_PLL_1MHz_CLK);
				break;
			case '5':
				SetNextClkPreset(NO_PLL_2MHz_CLK);
				break;
			case '6':
				SetNextClkPreset(NO_PLL_4MHz_CLK);
				break;
			case '7':
				SetNextClkPreset(NO_PLL_8MHz_CLK);
				break;
			case '8':
				SetNextClkPreset(NO_PLL_16MHz_CLK);
				break;
			case '9':
				SetNextClkPreset(NO_PLL_24MHz_CLK);
				break;
			case 'A':
				SetNextClkPreset(NO_PLL_32MHz_CLK);
				break;
			case 'B':
				SetNextClkPreset(NO_PLL_48MHz_CLK);
				break;
		}
	}
}
#endif
#endif

void dummy(void * pData)
{

}

void InitBLEAndSetItToStandby(void)
{
	hci_init(dummy, NULL);
	hci_reset();
	HAL_Delay(5);
	aci_hal_device_standby();
	hci_deinit();

}
