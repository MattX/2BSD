.globl aldiv
/ long assign divide (fp)
/ called: 2(sp): ptr to LHS, 4(sp):RHS
/ $Log:	aldiv.s,v $
/ Revision 1.2  85/04/28  17:15:02  ach
/ check for divide by zero, so that you can do what the old aldiv
/ did (the one that didn't use floating point), rather than getting
/ a floating divide trap with integer arithmetic. -sku
/ 

aldiv:
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
	divf	fr1,fr0
	movfi	fr0,(r0)
	mov	2(r0),r1
	mov	(r0),r0
	seti
	rts	pc
