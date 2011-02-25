# -Werror

all::
	arm-elf-as -o start.o start.s
	arm-elf-gcc -Wall -Wextra  -nostdlib -nostartfiles -ffreestanding -std=gnu99 -c main.c
	arm-elf-ld -T link.ld -o kernel start.o main.o
	cp kernel ../../../run/qemu/0.13.0/kernel

run::
	qemu-system-arm -m 128 -kernel kernel -serial stdio

clean::
	-rm *.[od] kernel