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
volatile uint8_t a,b;
extern BufferStruct sBuffer;

#define MEMORY_COPY1() do {\
	a = b;\
	b = a;\
	} while(0);
#define  ADDITIVE() do{\
	a = a + b;\
	b = a - b;\
	}while(0);
#define MULTIPLICATIVE() do{\
	a = a * b;\
	b = 2;\
	b = a / b;\
	}while(0);
#define BIT_MANIPULATION() do{\
	a = b >> a;\
	b = a << b;\
	}while(0);

#define TEST1() 			MEMORY_COPY1()\
							ADDITIVE()\
							MULTIPLICATIVE()\
							BIT_MANIPULATION()\


#define TEST2() 			TEST1()\
							TEST1()\
							TEST1()\
							TEST1()\
							TEST1()\
							TEST1()\
							TEST1()\
							TEST1()\
							TEST1()\
							TEST1()

#define TEST() 				TEST2()\
							TEST2()\
							TEST2()\
							TEST2()\
							TEST2()\
							TEST2()\
							TEST2()\
							TEST2()\
							TEST2()\
							TEST2()
void TestPayload(void)
{
//	uint32_t start_time, end_Time;
//	float time;


#if defined(HAS_BLUETOOTH)
	char message[200];
	static unsigned char len = 0;
	len = sprintf(message, "123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n");
	SendToBLESerial((unsigned char *)message, len);
#endif

	TEST()
	TEST()
	TEST()
	TEST()

#if defined(HAS_BLUETOOTH)
	len = sprintf(message, "123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n");
	SendToBLESerial((unsigned char *)message, len);
#endif


}



void MemoryCopyTest(void)
{
	/*
	 * Non-DMA, blocking test
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
		MEMORY_COPY1()
	}
}
#endif
