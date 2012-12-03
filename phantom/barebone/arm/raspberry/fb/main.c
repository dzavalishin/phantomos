#include "fb.h"
#include "gpio.h"
#include "kernel.h"

void kmain()
{
	InitGpio(); //We need to set up the GPIO before we can use it

	WriteGpio(7); //Check that LEDs haven't been yanked out while I wasn't looking
	WaitSomeTime(0); //Light them for a non-zero amount of time
	WriteGpio(8); //Switch the GPIOs to low (except the OK LED)
	WaitSomeTime(0); //Wait a little

	InitFb(); //Initialize the frame buffer
	
	PrintString("Hello, world!");

	
	//If we wish to retain the GPIO LEDs, comment the line above out
	while (1); //Don't even try to return from here
}
