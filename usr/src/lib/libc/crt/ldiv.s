.globl ldiv
/ long divide (fp)
/ called: 2(sp): LHS 6(sp):RHS
/ $Log:	ldiv.s,v $
/ Revision 1.2  85/06/14  15:27:54  ach
/ Put in a check for divide by zero.  Don't want floating divide
/ trap in integer math. -sku.
/

ldiv:
	setl
	tst	6(sp)		/ divide by zero check
	jne	1f
	tst	8.(sp)
	jne	1f
	mov	2(sp),r0
	mov	4(sp),r1	/ return LHS
	rts	pc
1:	movif	2(sp),r0
	movif	6(sp),r1
	divf	r1,r0
	movfi	r0,-(sp)
	mov	(sp)+,r0
	mov	(sp)+,r1
	seti
	rts	pc
