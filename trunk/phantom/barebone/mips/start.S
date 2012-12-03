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

#include "regdef.h"
#include "cp0regdef.h"
#include "asm.h"

LEAF(_start)
	
//    .set	mips2
//    .set	reorder
	.set	noreorder
	.set	mips32

    /* Disable interrupts */
    mtc0	zero, CP0_STATUS

    /* Disable watch exception. */
    mtc0    zero, CP0_WATCHLO
    mtc0    zero, CP0_WATCHHI

#if 1
    /* disable kernel mode cache */
    mfc0	t0, CP0_CONFIG
    and	t0, ~0x7
    ori	t0, 0x2
    mtc0	t0, CP0_CONFIG
#endif
    /* set up stack */
    li	sp, 0x0000000 + RAMSIZE - 16

 	lui	v0,0xB400
 	ori	v0,v0,0x3f8
 	li	v1,104
 	sb	v1,0(v0)
 	li	v1,101
 	sb	v1,0(v0)
 	li	v1,108
 	sb	v1,0(v0)
 	sb	v1,0(v0)
 	li	v1,111
 	sb	v1,0(v0)
 	li	v1,33
 	sb	v1,0(v0)
 	li	v1,10
 	sb	v1,0(v0)
 	li	v1,13
 	sb	v1,0(v0)

	
    /* jump to main */
    jal	main
    nop

loop:
    j	loop
    nop
END(_start)


// pagemask (cp0 r5) = 0 -> 4Kb page
// wired (cp0 r6) = 24 (half of TLB is wired)?

// get pr id = (cp0 r15)

// TODO cp0 r16 config



