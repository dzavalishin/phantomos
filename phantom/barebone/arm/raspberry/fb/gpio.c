#include "kernel.h"

#define GPIO_BASE 0x200000 /* Base address of the GPIO */

#define GPIO_MODE_00_09 0x0 /* Address of the mode bits for GPIOs 0-9 */
#define GPIO_MODE_10_19 0x4 /* Address of the mode bits for GPIOs 10-19 */
#define GPIO_MODE_20_29 0x8 /* Address of the mode bits for GPIOs 20-29 */

#define GPIO_SET_0_31 0x1C /* Bits to set GPIO pins 0-31 */
#define GPIO_CLR_0_31 0x28 /* Bits to clear GPIO pins 0-31 */
#define GPIO_VAL_0_31 0x34 /* Bits to read GPIO pins 0-31 */

/* See public BCM2835 peripheral document */
#define GPIO08_OUTPUT (1 << 24)
#define GPIO09_OUTPUT (1 << 27)
#define GPIO11_OUTPUT (1 << 3)
#define GPIO14_OUTPUT (1 << 12)
#define GPIO16_OUTPUT (1 << 18)
#define GPIO25_OUTPUT (1 << 15)

void InitGpio()
{
	/* Initialise pins 8 and 9 to output */
	WriteMmReg32(GPIO_BASE, GPIO_MODE_00_09, GPIO08_OUTPUT | GPIO09_OUTPUT);
	
	/* Initialise pin 11 and 16 to output (pin 14 is input by default) */
	WriteMmReg32(GPIO_BASE, GPIO_MODE_10_19, GPIO11_OUTPUT | GPIO16_OUTPUT);
	
	/* Initialise pin 25 to output */
	WriteMmReg32(GPIO_BASE, GPIO_MODE_20_29, GPIO25_OUTPUT);
}

int ReadGpio()
{
	/* Read the first 32 GPIO pins, and mask out all but pin 14 */
	int v = ((ReadMmReg32(GPIO_BASE, GPIO_VAL_0_31) & (1 << 14)) ? 1 : 0);
	return v;
}

void WriteGpio(int value)
{
	/* Unset the existing LEDs */
	WriteMmReg32(GPIO_BASE, GPIO_CLR_0_31,
		(1 << 8) | (1 << 9) | (1 << 11) | (1 << 25) | (1 << 16));
	
	/* Determine which GPIO pins to set */
	uint32_t set = 0;
	set |= (value & (1 << 0)) ? (1 << 8) : 0;
	set |= (value & (1 << 1)) ? (1 << 9) : 0;
	set |= (value & (1 << 2)) ? (1 << 11) : 0;
	set |= (value & (1 << 3)) ? (1 << 25) : 0;
	set |= (value & (1 << 4)) ? (1 << 16) : 0;
	
	/* Set the chosen pins */
	WriteMmReg32(GPIO_BASE, GPIO_SET_0_31, set);
}

void Flash32_Value(uint32_t value)
{
	for (unsigned int i = 0; i < 4; i++){
		WriteGpio((value & 0xF));
		WaitSomeTime(-2);
		WriteGpio(0x10 | (value & 0xF));
		WaitSomeTime(-2);
	}
	
	for (unsigned int i = 4; i < 32; i += 4){
		WriteGpio((value >> i) & 0xF);
		WaitSomeTime(0);
		WriteGpio(0x10 | ((value >> i) & 0xF));
		WaitSomeTime(0);
	}
}

void Flash32(uint32_t value)
{
	if (!ReadGpio()){
		while (!ReadGpio()){
			Flash32_Value(value);
		}
	}
	else {
		while (ReadGpio()){
			Flash32_Value(value);
		}
	}
}
