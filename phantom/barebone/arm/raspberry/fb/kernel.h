#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

extern uint32_t ReadMmReg32(uint32_t base, uint32_t reg);
extern void WriteMmReg32(uint32_t base, uint32_t reg, uint32_t value);
extern uint32_t ArmToVc(void *p);
extern void *VcToArm(uint32_t p);
extern void MemoryBarrier();
extern void WaitSomeTime(int n);

#endif
