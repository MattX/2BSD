.globl alrem
/ long assign remainder (fp)
/ called: 2(sp): ptr to LHS, 4(sp):RHS
/ $Log:	alrem.s,v $
/ Revision 1.2  85/06/14  15:25:08  ach
/ Put in code to check for divide by zero.  It is not desireable to have
/ a floating point divide trap when doing integer math. -sku
/ 

alrem:
	setl
	tst	4(sp)		/ divide by zero check
	jne	1f
	tst	6(sp)
	jne	1f
	mov	2(sp),r0	/ return LHS
	mov	2(r0),r1
	mov	(r0),r0
	rts	pc
1:	mov	2(sp),r0
	movif	(r0),fr0
	movif	4(sp),fr1
	movf	fr0,fr2
	movf	fr1,fr3
	divf	fr1,fr0
	modf	$40200,fr0
	mulf	fr3,fr1
	subf	fr1,fr2
	movfi	fr2,(r0)
	mov	2(r0),r1
	mov	(r0),r0
	seti
	rts	pc
