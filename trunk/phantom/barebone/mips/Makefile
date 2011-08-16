# A brief explanation.
#  
# Copyright (C) 2001 MontaVista Software Inc.
# Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
# 
# This program is free software; you can redistribute  it and/or modify it
# under  the terms of  the GNU General  Public License as published by the
# Free Software Foundation;  either version 2 of the  License, or (at your
# option) any later version.

######## CONFIGURATION #########

#LOADADDR = 0x0
#LOADADDR = 0xa0040000
#LOADADDR = 0x80040000
LOADADDR = 0x80000000
RAMSIZE = 0x00100000		# 1MB

#CROSS_COMPILE = mipsel-linux-
CROSS_COMPILE = mips-elf-

######## END OF CONFIGURATION #########

.S.s:
	$(CPP) $(CFLAGS) $< -o $*.s
.S.o:
	$(CC) $(CFLAGS) -c $< -o $*.o
.c.o:
	$(CC) $(CFLAGS) -c $< -o $*.o

CC =       $(CROSS_COMPILE)gcc
LD =       $(CROSS_COMPILE)ld
OBJCOPY =  $(CROSS_COMPILE)objcopy
OBJDUMP =  $(CROSS_COMPILE)objdump

CFLAGS = -O -G 0 -mno-abicalls -fno-pic -Wall -DRAMSIZE=${RAMSIZE} -g

# Drop some uninteresting sections in the kernel.
# This is only relevant for ELF kernels but doesn't hurt a.out
drop-sections   = .reginfo .mdebug
strip-flags     = $(addprefix --remove-section=,$(drop-sections))


all : elf 
#srec bin

srec : barebone.srec
elf : barebone.elf
bin : barebone.bin
#print.o printf.o
barebone.elf : start.o main.o uart16550.o test.o
	$(LD) -g -o barebone.elf -N -Tbarebone.lds -Ttext $(LOADADDR) $^

barebone.srec : barebone.elf
	$(OBJCOPY) -S -O srec $(strip-flags) barebone.elf barebone.srec

barebone.bin : barebone.elf
	$(OBJCOPY) -S -O binary $(strip-flags) barebone.elf barebone.bin

clean:
	rm -f *.o barebone.elf barebone.srec barebone.bin
