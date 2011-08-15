/*
 * PL011 UART driver
 *
 * Copyright (C) 2009 B Labs Ltd.
 */
#include <libdev/uart.h>
#include <libdev/io.h>
#include "drv_pl011_uart.h"
 
/* Error status bits in receive status register */
#define PL011_FE		(1 << 0)
#define PL011_PE		(1 << 1)
#define PL011_BE		(1 << 2)
#define PL011_OE		(1 << 3)
 
/* Status bits in flag register */
#define PL011_TXFE		(1 << 7)
#define PL011_RXFF		(1 << 6)
#define PL011_TXFF		(1 << 5)
#define PL011_RXFE		(1 << 4)
#define PL011_BUSY		(1 << 3)
#define PL011_DCD		(1 << 2)
#define PL011_DSR		(1 << 1)
#define PL011_CTS		(1 << 0)
 
void uart_tx_char(unsigned long base, char c)
{
	unsigned int val = 0;
 
	do {
		val = read((base + PL011_UARTFR));
	} while (val & PL011_TXFF);  /* TX FIFO FULL */
 
	write(c, (base + PL011_UARTDR));
}
 
char uart_rx_char(unsigned long base)
{
	unsigned int val = 0;
 
	do {
		val = read(base + PL011_UARTFR);
	} while (val & PL011_RXFE); /* RX FIFO Empty */
 
	return (char)read((base + PL011_UARTDR));
}
 
/*
 * Sets the baud rate in kbps. It is recommended to use
 * standard rates such as: 1200, 2400, 3600, 4800, 7200,
 * 9600, 14400, 19200, 28800, 38400, 57600 76800, 115200.
 */
void pl011_set_baudrate(unsigned long base, unsigned int baud,
			unsigned int clkrate)
{
	const unsigned int uartclk = 24000000;	/* 24Mhz clock fixed on pb926 */
	unsigned int val = 0, ipart = 0, fpart = 0;
 
	/* Use default pb926 rate if no rate is supplied */
	if (clkrate == 0)
		clkrate = uartclk;
	if (baud > 115200 || baud < 1200)
		baud = 38400;	/* Default rate. */
 
	/* 24000000 / (38400 * 16) */
	ipart = 39;
 
	write(ipart, base + PL011_UARTIBRD);
	write(fpart, base + PL011_UARTFBRD);
 
	/*
	 * For the IBAUD and FBAUD to update, we need to
	 * write to UARTLCR_H because the 3 registers are
	 * actually part of a single register in hardware
	 * which only updates by a write to UARTLCR_H
	 */
	val = read(base + PL011_UARTLCR_H);
	write(val, base + PL011_UARTLCR_H);
 
}
 
void uart_init(unsigned long uart_base)
{
	/* Initialise data register for 8 bit data read/writes */
	pl011_set_word_width(uart_base, 8);
 
	/*
	 * Fifos are disabled because by default it is assumed the port
	 * will be used as a user terminal, and in that case the typed
	 * characters will only show up when fifos are flushed, rather than
	 * when each character is typed. We avoid this by not using fifos.
	 */
	pl011_disable_fifos(uart_base);
 
	/* Set default baud rate of 38400 */
	pl011_set_baudrate(uart_base, 38400, 24000000);
 
	/* Set default settings of 1 stop bit, no parity, no hw flow ctrl */
	pl011_set_stopbits(uart_base, 1);
	pl011_parity_disable(uart_base);
 
	/* Enable rx, tx, and uart chip */
	pl011_tx_enable(uart_base);
	pl011_rx_enable(uart_base);
	pl011_uart_enable(uart_base);
}
 
unsigned long uart_print_base;
 
void platform_init(void)
{
	uart_print_base = PL011_BASE;
 
	/*
	 * We dont need to initialize uart here for variant-userspace,
	 * as this is the same uart as used by kernel and hence
	 * already initialized, we just need
	 * a uart struct instance with proper base address.
	 *
	 * But in case of baremetal like loader, no one has done
	 * initialization, so we need to do it.
	 */
#if defined(VARIANT_BAREMETAL)
	uart_init(uart_print_base);
#endif
}
