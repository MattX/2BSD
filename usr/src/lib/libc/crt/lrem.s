.globl lrem
/ long remainder (fp)
/ called: 2(sp): LHS 6(sp):RHS
/ $Log:	lrem.s,v $
/ Revision 1.2  85/04/28  17:31:37  ach
/ Put in code to check for divide by zero.  It is not desireable to do
/ a floating divide check for integer math. -sku.
/ 

lrem:
	setl
	tst	6(sp)		/ divide by zero check
	jne	1f
	tst	8.(sp)
	jne	1f
	mov	2(sp),r0
	mov	4(sp),r1	/ return LHS
	rts	pc
1:	movif	2(sp),r0
	movf	r0,r2
	movif	6(sp),r1
	movf	r1,r3
	divf	r1,r0
	modf	$40200,r0
	mulf	r3,r1
	subf	r1,r2
	movfi	r2,-(sp)
	mov	(sp)+,r0
	mov	(sp)+,r1
	seti
	rts	pc
