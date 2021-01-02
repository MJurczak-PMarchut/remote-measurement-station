/*
 * test_payloads.c
 *
 *  Created on: 29 gru 2020
 *      Author: Mateusz Jurczak
 */
#include "test_payloads.h"


#define MEMORY_COPY_TEST
#define SIMPLE_ARITHMETIC_TEST
#define DIV_MUL_TEST

#define TestIterationCount 17
uint8_t a,b;


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
	uint32_t start_time, end_Time;
	unsigned char len;
	unsigned char message[50];
#ifdef MEMORY_COPY_TEST

	start_time = HAL_GetTick();
	MemoryCopyTest();
	end_Time = HAL_GetTick();
	len = sprintf(message, "Time per test : %d ms\n", end_Time-start_time);
	SendToBLESerial(message, len);
	start_time = HAL_GetTick();
	len = sprintf(message, "Time per msg send : %d ms\n", start_time-end_Time);
	SendToBLESerial(message, len);
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

