#!/bin/bash

#A useful note for searching through the source
#grep -i init -r `patch --dry-run -p0 < r-pi_linux_3.1.9.patch | grep "patching file linux" | sed -e "s/patching file linux/rpilinux/" | xargs` | grep -v -E "html|usb|mmc"

#PREFIX=/home/nicholas/crosscompile/linux-x86/bin/arm-bcm2708-linux-gnueabi-
PREFIX=arm-none-eabi-
CFLAGS=-Wall\ --std=c99\ -O0\ -ffreestanding\ -nostdlib\ -fno-builtin #\ -fdata-sections\ -ffunction-sections -mcpu=arm1176jzf-s

#cd chars
#./chars.sh
#cd ../

${PREFIX}gcc $CFLAGS -o fb.o -c fb.c
${PREFIX}gcc $CFLAGS -o gpio.o -c gpio.c
${PREFIX}gcc $CFLAGS -o kernel.o -c kernel.c
${PREFIX}gcc $CFLAGS -o main.o -c main.c
${PREFIX}as -o start.o start.S #Compile the assembly file

${PREFIX}ld -o kernel.elf fb.o gpio.o kernel.o main.o -T kernel.ld
${PREFIX}objcopy -R junk -O binary kernel.elf kernel.img #Produce a flat binary file

#cat first32k.bin kernel.img > kernel.img.tmp #chars_pixels.bin
#mv kernel.img.tmp kernel.img

#rm *.o *.elf chars/chars #chars_pixels.h #Clean up chars/chars_pixels.bin
