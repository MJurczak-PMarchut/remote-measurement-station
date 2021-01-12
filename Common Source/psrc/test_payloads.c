/*
 * test_payloads.c
 *
 *  Created on: 29 gru 2020
 *      Author: Mateusz Jurczak
 */
#include "test_payloads.h"
#include <stdio.h>
#ifdef BOARD_SENSORTILE
#define MEMORY_COPY_TEST
#define SIMPLE_ARITHMETIC_TEST
#define DIV_MUL_TEST

#define TestIterationCount 17
uint8_t a,b;
extern BufferStruct sBuffer;

#define MEMORY_COPY1() do {\
	a = b;\
	b = a;\
	} while(0);

#define MEMORY_COPY2() 		MEMORY_COPY1()\
							MEMORY_COPY1()\
							MEMORY_COPY1()\
							MEMORY_COPY1()\
							MEMORY_COPY1()\
							MEMORY_COPY1()\
							MEMORY_COPY1()\
							MEMORY_COPY1()\
							MEMORY_COPY1()\
							MEMORY_COPY1()

#define MEMORY_COPY3() 		MEMORY_COPY2()\
							MEMORY_COPY2()\
							MEMORY_COPY2()\
							MEMORY_COPY2()\
							MEMORY_COPY2()\
							MEMORY_COPY2()\
							MEMORY_COPY2()\
							MEMORY_COPY2()\
							MEMORY_COPY2()\
							MEMORY_COPY2()

#define MEMORY_COPY() 		MEMORY_COPY3()\
							MEMORY_COPY3()\
							MEMORY_COPY3()\
							MEMORY_COPY3()\
							MEMORY_COPY3()\
							MEMORY_COPY3()\
							MEMORY_COPY3()\
							MEMORY_COPY3()\
							MEMORY_COPY3()\
							MEMORY_COPY3()
void TestPayload(void)
{
//	uint32_t start_time, end_Time;
//	float time;
	static unsigned char len = 0;
	char message[200];
	if (len == 0){
			len = sprintf(message, "123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n");
			SendToBLESerial((unsigned char *)message, len);
			len = sprintf(message, "123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n");
			SendToBLESerial((unsigned char *)message, len);
	}
	if(sBuffer.BufferEnd == 0)
	{
		len = 0;
	}

#ifdef MEMORY_COPY_TEST
//	static uint8_t flag = 0;
//	start_time = GetTim2Tick();
//	MemoryCopyTest();
//	end_Time = GetTim2Tick();
//	time = ((float)end_Time-(float)start_time)/(TIM2_FREQUENCY_FOR_TIMING/1000);
//	len = sprintf(message, "Time per test : %.3f ms\n", time);
//	SendToBLESerial(message, len);
//	start_time = GetTim2Tick();
//	time = ((float)start_time-(float)end_Time)/(TIM2_FREQUENCY_FOR_TIMING/1000);
//	len = sprintf(message, "Time per msg send : %.3f ms\n", time);
//	SendToBLESerial(message, len);
//	len = sprintf(message, "Free space in msg buffer %d\n", CheckBufferSize());
//	SendToBLESerial(message, len);

#endif

}



void MemoryCopyTest(void)
{
	/*
	 * Non-DMA, blocking test
	 * each iteration is 1000 repeats of a a=b, b=a copy
	 * after every copy segment there will be a
	 * ldr, adds, str, ldr, cmp.w, bcc.w
	 *
	 * each copy is a
	 * ldr, ldrb, ldr, strb, ldr, ldrb, ldr, strb
	 *
	 * after 244 bytes of code there is a b.n jump
	 */
	unsigned int iter = 0;
	for(iter = 0; iter < TestIterationCount; iter++)
	{
		MEMORY_COPY()
	}
}
#endif
