/*
 * Copyright (C) 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

//#include "printf.h"

void uart_test();


//#define ISA 0x1fd00000
#define ISA 0x14000000

//char *uart_data = (0x3f8+ISA);

int main()
{
    unsigned a = 0xA000000u;
    a |= ISA;
    a |= 0x3f8;

    a = 0xB40003f8u;

	volatile char *uart_data = (void *)a;

	*uart_data = 'm';
	*uart_data = 'a';
	*uart_data = 'i';
	*uart_data = 'n';
	*uart_data = '!';
	*uart_data = '!';
	*uart_data = '\n';
	*uart_data = '\r';

    //uart_test();

	//printf("\n");
	//printf("(main) Hello, world!\n");
	//printf("loop forever ...\n");
	for(;;);
}
