#ifndef GPIO_H
#define GPIO_H

#include "kernel.h"

extern void InitGpio();
extern int ReadGpio();
extern void WriteGpio(int value);
extern void Flash32(uint32_t value);

#define FlashOk(condition, value) \
	while(condition){\
		WriteGpio((value & 0xF) | 0x10);\
		WaitSomeTime(-1);\
		WriteGpio(value & 0xF);\
		WaitSomeTime(-1);\
	}

#endif
