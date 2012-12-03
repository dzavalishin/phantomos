#!/bin/bash

#PREFIX=/home/nicholas/crosscompile/linux-x86/bin/arm-bcm2708-linux-gnueabi-
CFLAGS=-Wall\ --std=c99\ -O0\ -fdata-sections\ -ffunction-sections

gcc -Wall --std=c99 -o chars chars.c
#convert chars.png gray:- | ./chars -b > chars_pixels.bin
/bin/convert chars.png gray:- | ./chars -b | ./chars -c > chars_pixels.h
#convert chars.png gray:- | ./chars -b | convert -depth 8 -size 8x4096 gray:- test.png

