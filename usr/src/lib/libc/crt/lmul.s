/*	@(#)lmul.s	2.1	SCCS id keyword	*/
/
/ 32-bit multiplication routine for fixed pt hardware.
/ also recommend with floating hardware, for speed & compatibility
/  Implements * operator
/ Credit to an unknown author who slipped it under the door.
/ $Log:	lmul.s,v $
/ Revision 1.2  85/06/14  15:30:35  ach
/ Use integer math for multiply, rather than floating point.  Don't
/ call csv/cret.  This is faster on an 11/44 than the floating point,
/ and also is more "correct".  Overflow with the floating point routines
/ produces zero.  This is a change: rand(3) expects to see the LS 32
/ bits of the answer. -sku
/ 

.globl	lmul

lmul:	mov	r2,-(sp)	/ faster than csv and just 1 more word
	mov	r3,-(sp)
	mov	8.(sp),r2
	sxt	r1
	sub	6(sp),r1
	mov	12.(sp),r0
	sxt	r3
	sub	10.(sp),r3
	mul	r0,r1
	mul	r2,r3
	add	r1,r3
	mul	r2,r0
	sub	r3,r0
	mov	(sp)+,r3
	mov	(sp)+,r2
	rts	pc
