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

#ifdef MEMORY_COPY_TEST
	MemoryCopyTest();
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
	 */
	unsigned int iter = 0;
	for(iter = 0; iter < TestIterationCount; iter++)
	{
		MEMORY_COPY()

	}
}

