# -Werror
def:: kernel

all::
	arm-elf-as -o start.o start.s
	arm-elf-gcc -Wall -Wextra  -nostdlib -nostartfiles -ffreestanding -std=gnu99 -c main.c
	cp kernel ../../../run/qemu/0.13.0/kernel

kernel: main.o drv_pl050_ps2.o start.o
	arm-elf-ld -T link.ld -o kernel start.o main.o drv_pl050_ps2.o
	cp kernel ../../../run/qemu/0.13.0/kernel

start.o: start.s
	arm-elf-as -o start.o start.s
        
drv_pl050_ps2.o: drv_pl050_ps2.c


CC=arm-elf-gcc
CFLAGS=-Wall -Wextra  -nostdlib -nostartfiles -ffreestanding -std=gnu99

run::
	qemu-system-arm -m 128 -kernel kernel -serial stdio

clean::
	-rm *.[od] kernel
        