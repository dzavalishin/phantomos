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


/* #define DEBUG_LED (*(unsigned short*)0xb7ffffc0) */
// #define DEBUG_LED (*(unsigned char*)(0xb9010000))
int DEBUG_LED;
#include "uart16550.h"
#include "printf.h"

void uart_test()
{
    int diff = 'A' - 'a';

    DEBUG_LED  = 0x11;
#if 0
    Uart16550Init(9600, 
                  UART16550_DATA_8BIT,
                  UART16550_PARITY_NONE,
                  UART16550_STOP_1BIT);
#endif

    DEBUG_LED  = 0x12;

    Uart16550Put('\r');
    Uart16550Put('\n');
    Uart16550Put('h');
    Uart16550Put('e');
    Uart16550Put('l');
    Uart16550Put('l');
    Uart16550Put('o');
    Uart16550Put('\r');
    Uart16550Put('\n');

    DEBUG_LED  = 0x14;
    for(;;) {
        int c = Uart16550GetPoll();
        Uart16550Put(c);
        c += diff;
        Uart16550Put(c);
        Uart16550Put('\r');
        Uart16550Put('\n');
    } 
}

void printf_test()
{
    printf("hello through printf!\n");
    printf("test string: %s\n", "test string");
    printf("test int: %d\n", -555);
    printf("test binary: %b\n", 555);
    printf("test hex: %08x\n", 555);
}
