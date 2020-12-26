/*
 * LoadWeighingAL.c
 *
 *  Created on: Dec 31, 2019
 *      Author: Mateusz Jurczak
 *
 *      Dec 26, 2020
 *      Note to self:
 *      Always comment your code, people change and you are not the same person that wrote that code
 *      All of this will probably need to be discarded, not really needed for the project
 */



#include "LoadWeighingAL.h"

typedef enum {S_SLOT_TAKEN, S_SLOT_FREE} SLOT_STATE;

/*not to be changed*/
#define MAX_SLOT_NO 80
#define HIGH_SLOT_COUNT 60

int32_t TimPSLP_RUN =0;
int32_t TimPS_RUN =0;
int32_t TimPS =0;


typedef struct{
	RunPriority Priority;
	PowerState ePowerState;
	VoidFuncPointer VoidFunction;
	SLOT_STATE SlotState;
	SLOT_TYPE  SlotType;
	uint32_t TimingMargin;
}FunctionSlot;

FunctionSlot sFunctionSlot[NoOfFunctionSlots];
uint8_t PID_List[NoOfFunctionSlots] = {0};

#if (NoOfFunctionSlots < MAX_SLOT_NO) && (NoOfFunctionSlots > 0)
	uint8_t 	NoOfActiveFunctions = 0;
	#if NoOfFunctionSlots >= HIGH_SLOT_COUNT
		#warning "Number of function slots is high ! It is ok, but if it causes problems please lower it in configuration.h"
	#endif
#else
	#error "Number of function slots greater than #MAX_SLOT_NO or less than 0 !!! Please check configuration.h"
#endif


void InitLWAL(void)
{
	uint8_t iter;
	for(iter = 0; iter < NoOfFunctionSlots; iter++)
	{
		PID_List[iter] = iter + 1;
	}
}


int32_t 		CheckTimingMarginForState	(PowerState ePowerState)
{
	switch(ePowerState){
		case PS_LOW_POWER_RUN:
			return TimPSLP_RUN;
			break;
		case PS_HIGH_SPEED:
			return TimPS_RUN;
			break;
		case PS_LOW_SPEED:
			return TimPS;
			break;
		default:
			return 0;
			break;
	}
}

int32_t 		CheckTimingMarginForProc	(ProcessID PID)
{
	/*recurrent procs only*/
	return sFunctionSlot[PID].TimingMargin;

}

ProcessID 			GetTheMostOffendingPID		(void)
{
	uint8_t u8SlotIter = 0;
	ProcessID PID;
	int32_t i32HiTime = -2147483648; //min int32 value
	for(u8SlotIter = 0; u8SlotIter < NoOfFunctionSlots; u8SlotIter++)
	{
		if(sFunctionSlot[u8SlotIter].TimingMargin > i32HiTime)
		{
			i32HiTime = (sFunctionSlot[u8SlotIter].TimingMargin > i32HiTime)?sFunctionSlot[u8SlotIter].TimingMargin:i32HiTime;
			PID = u8SlotIter;
		}
	}
	return PID;
}

ProcessID			AddFunctionToScheduler		(VoidFuncPointer Function, RunPriority Priority, PowerState ePowerState, SLOT_TYPE SlotType)
{
#ifdef DEBUG_LW_AL
		uint8_t WhileDebug = 1;
		uint8_t NotEnoughSpaceLeft = 0;
#endif
		ProcessID PID;
		if(NoOfActiveFunctions >= NoOfFunctionSlots){
#ifdef DEBUG_LW_AL
			NotEnoughSpaceLeft = NotEnoughSpaceLeft + 1;
#endif
			return 0; //When no function slots free return zero, if debugging is enabled this incident will be recorded
		}
		PID = PID_List[NoOfActiveFunctions];

		if(sFunctionSlot[PID].SlotState == S_SLOT_FREE){
			sFunctionSlot[PID].SlotState = S_SLOT_TAKEN;
			sFunctionSlot[PID].Priority = Priority;
			sFunctionSlot[PID].VoidFunction = Function;
			sFunctionSlot[PID].ePowerState = ePowerState;
			sFunctionSlot[PID].SlotType = SlotType;
		}
#ifdef DEBUG_LW_AL
		else
		{
			while(WhileDebug > 0){} //Keep variable values
		}
#endif
		NoOfActiveFunctions++;
		return PID;

}

HAL_StatusTypeDef	DeleteFunctionFromScheduler	(ProcessID PID)
{
#ifdef DEBUG_LW_AL
		uint8_t WhileDebug = 1;
#endif


		if(sFunctionSlot[PID].SlotState == S_SLOT_TAKEN){
			sFunctionSlot[PID].SlotState = S_SLOT_FREE;
#ifdef DEBUG_LW_AL
		while((NoOfActiveFunctions == 0) && (NoOfActiveFunctions >= NoOfFunctionSlots))
#endif
			NoOfActiveFunctions--;
			PID_List[NoOfActiveFunctions] = PID;
			return HAL_OK;
		}

#ifdef DEBUG_LW_AL
		else
		{
			while(WhileDebug > 0){} //Keep variable values
		}
#endif


		return HAL_ERROR;

}


HAL_StatusTypeDef	CheckIfProcCompleted		(ProcessID PID)
{
	return (sFunctionSlot[PID].SlotState == S_SLOT_TAKEN)?HAL_OK:HAL_ERROR;
}

HAL_StatusTypeDef	ChangeProcPriority			(ProcessID PID, RunPriority NewPriority)
{
	if(sFunctionSlot[PID].SlotState == S_SLOT_TAKEN){
		sFunctionSlot[PID].Priority = NewPriority;
		return HAL_OK;
	}
	return HAL_ERROR;
}


HAL_StatusTypeDef	LW_Scheduler				(PowerState ePowerState)
{
#ifdef DEBUG_LW_AL
		uint8_t WhileDebug = 1;
#endif
	uint32_t SchedulerStartTime = HAL_GetTick();
	uint32_t recurrentTimeStart;
	uint32_t TimingMargin;
	uint8_t u8SlotIter  = 0;
	RunPriority HighestPriority = LOWEST_PRIORITY + 1;
	ProcessID PID;
	uint8_t PowerCheck = 0x01;

#ifdef POWER_FIRST
	SchedulerStartTime = HAL_GetTick();
SCHEDULING_START:
	for(u8SlotIter = 0; u8SlotIter < NoOfFunctionSlots; u8SlotIter ++)
	{
		if((sFunctionSlot[u8SlotIter].SlotState == S_SLOT_TAKEN) && (sFunctionSlot[u8SlotIter].Priority < HighestPriority))
		{
			if(PowerCheck >= 0x01){
				if(sFunctionSlot[u8SlotIter].ePowerState == ePowerState){
					HighestPriority = sFunctionSlot[u8SlotIter].Priority;
					PID = u8SlotIter;
				}
			}
			else{
				HighestPriority = sFunctionSlot[u8SlotIter].Priority;
				PID = u8SlotIter;
			}
		}
	}
	if(HighestPriority <= LOWEST_PRIORITY)
	{
		if(sFunctionSlot[PID].SlotType == ST_RECURRENT)
		{
			recurrentTimeStart = HAL_GetTick();
		}
		sFunctionSlot[PID].VoidFunction();
		if(sFunctionSlot[PID].SlotType == ST_RECURRENT)
		{
			sFunctionSlot[PID].TimingMargin = HAL_GetTick() - recurrentTimeStart;
		}
		sFunctionSlot[PID].SlotState = (sFunctionSlot[PID].SlotType == ST_RECURRENT)?S_SLOT_TAKEN:S_SLOT_FREE;
		HighestPriority = LOWEST_PRIORITY + 1;
		TimingMargin = HAL_GetTick() - SchedulerStartTime;
		switch(ePowerState){
		case PS_LOW_POWER_RUN:
			if(TimingMargin < TIMING_MARGIN_LP_RUN)
			{
				goto SCHEDULING_START;
			}
			break;
		case PS_HIGH_SPEED:
			if(TimingMargin < TIMING_MARGIN_RUN)
			{
				goto SCHEDULING_START;
			}
			break;
		case PS_LOW_SPEED:
			if(TimingMargin < TIMING_MARGIN)
			{
				goto SCHEDULING_START;
			}
			break;
		default:
#ifdef DEBUG_LW_AL
		while(WhileDebug){}
#else
		return HAL_ERROR; //we shouldn't come here in other power states
#endif
		break;
		}
	}
	else
	{
		switch(ePowerState){
			case PS_LOW_POWER_RUN:
				if(TimingMargin < TIMING_MARGIN_LP_RUN)
				{
					TimPSLP_RUN = TimPSLP_RUN + TimingMargin - TIMING_MARGIN_LP_RUN;
				}
				break;
			case PS_HIGH_SPEED:
				if(TimingMargin < TIMING_MARGIN_RUN)
				{
					TimPS_RUN = TimPS_RUN + TimingMargin - TIMING_MARGIN_RUN;
				}
				break;
			case PS_LOW_SPEED:
				if(TimingMargin < TIMING_MARGIN)
				{
					TimPS = TimPS + TimingMargin - TIMING_MARGIN;
				}
				break;
			default:
				break;
		}
		return HAL_OK;
	}


#endif


	return HAL_OK;
}
