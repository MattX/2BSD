/*	@(#)kill.s	2.2	SCCS id keyword	*/
/ C library -- kill

.globl	_killpg, cerror

_killpg:
	mov	r5,-(sp)
	mov	sp,r5
	mov	4(sp),8f
	mov	6(sp),8f+2
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	clr	r0
	mov	(sp)+,r5
	rts	pc

.data
9:
	sys	killpg; 8:..; ..
