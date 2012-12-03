/******** Init *********/
.section .init
b entry

/******** Entry ********/
.section .text
entry:
	/* Set up */
	mov sp, #0x100000 /* Give C some stack space (at 1 MB) */
	
	/* Call kmain */
	b kmain

/* Modified from the Linux kernel source TODO: Where? */
.globl MemoryBarrier
MemoryBarrier:
	mcr	p15, 0, ip, c7, c5, 0		@ invalidate I cache
	mcr	p15, 0, ip, c7, c5, 6		@ invalidate BTB
	mcr	p15, 0, ip, c7, c10, 4		@ drain write buffer
	mcr	p15, 0, ip, c7, c5, 4		@ prefetch flush
	mov	pc, lr
