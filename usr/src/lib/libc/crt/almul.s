/*	@(#)almul.s	2.1	SCCS id keyword	*/
/
/ 32-bit multiplication routine for fixed pt hardware.
/ also recomended for float hardware, for compatility & speed.
/  Implements *= operator
/ Credit to an unknown author who slipped it under the door.
/ "called" with long i, j; almul(&i, j);
/ $Log:	almul.s,v $
/ Revision 1.2  85/06/14  15:21:05  ach
/ Use integer math for integer multiply.  Since we don't have to
/ use csv/cret, we can have the same or better speed than using
/ floating point. -sku
/ 
/
.globl	almul
.globl	csv, cret

almul:
	jsr	r5,csv
	mov	4(r5),r4
	mov	2(r4),r2
	sxt	r1
	sub	(r4),r1
	mov	8.(r5),r0
	sxt	r3
	sub	6.(r5),r3
	mul	r0,r1
	mul	r2,r3
	add	r1,r3
	mul	r2,r0
	sub	r3,r0
	mov	r0,(r4)+
	mov	r1,(r4)
	jmp	cret
