#include <stddef.h>
#include <stdint.h>

#include "kernel.h"

#define PERIPHERAL_BASE 0x20000000 /* Base address for all (ARM?) peripherals */

uint32_t ReadMmReg32(uint32_t base, uint32_t reg)
{
	MemoryBarrier(); //Make sure we read the right thing
	//Technically, this memory barrier isn't necessary all the time, but it
	//shouldn't hurt unless we really care about performance. We do need it
	//sometimes though
	return *(volatile uint32_t *)(PERIPHERAL_BASE + base + reg);
}

void WriteMmReg32(uint32_t base, uint32_t reg, uint32_t value)
{
	MemoryBarrier(); //Probably not necessary, but let's be safe
	*(volatile uint32_t *)(PERIPHERAL_BASE + base + reg) = value;
	MemoryBarrier(); //Make sure the write is visible elsewhere
		//(see ReadMmReg32 for note on how necessary this is)
}

/* Occasionally useful for UI purposes */
void WaitSomeTime(int n)
{
	//Loops take time to execute; volatile stops the compiler from optimising
	//this away
	for (volatile int i = 0; i < (1 << (22 + n)); i++);
}

uint32_t ArmToVc(void *p)
{
	//Some things (e.g: the GPU) expect bus addresses, not ARM physical
	//addresses
	return ((uint32_t)p) + 0xC0000000;
}

void *VcToArm(uint32_t p)
{
	//Go the other way to ArmToVc
	return (void *)(p - 0xC0000000);
}


/* Without this, GCC gives undefined reference to `memcpy' even with 
 * -ffreestanding -nostdlib -fno-builtin */
void *memcpy(void *dest, const void *src, size_t count)
{
	for (size_t i = 0; i < count; i++){
		((unsigned char *)dest)[i] = ((const unsigned char *)src)[i];
	}
	return dest;
}
