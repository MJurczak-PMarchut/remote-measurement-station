/*
 * LoadWeighingAL.h
 *
 *  Created on: Dec 31, 2019
 *      Author: Mateusz Jurczak
 */

#ifndef INC_LOADWEIGHINGAL_H_
#define INC_LOADWEIGHINGAL_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "configuration.h"

#ifdef BOARD_N64_F4
	#include "stm32f4xx.h"
#else
	#include "stm32l4xx.h"
#endif
#include "SpecialTypes.h"


void 				InitLWAL					(void);

int32_t		 		CheckTimingMarginForState	(PowerState ePowerState);
int32_t		 		CheckTimingMarginForProc	(ProcessID PID);
ProcessID 			GetTheMostOffendingPID		(void);
ProcessID			AddFunctionToScheduler		(VoidFuncPointer Function, RunPriority Priority, PowerState ePowerState, SLOT_TYPE SlotType);
HAL_StatusTypeDef	DeleteFunctionFromScheduler	(ProcessID PID);
HAL_StatusTypeDef	CheckIfProcCompleted		(ProcessID PID);
HAL_StatusTypeDef	ChangeProcPriority			(ProcessID PID, RunPriority NewPriority);

HAL_StatusTypeDef	LW_Scheduler				(PowerState ePowerState);


#ifdef __cplusplus
}
#endif
#endif /* INC_LOADWEIGHINGAL_H_ */
