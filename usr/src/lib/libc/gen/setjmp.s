/*	@(#)setjmp.s	2.1	SCCS id keyword	*/
/ C library -- setjmp, longjmp

/	longjmp(a,v)
/ will generate a "return(v)" from
/ the last call to
/	setjmp(a)
/ by restoring sp, r5, pc from `a'
/ and doing a return.
/

.globl	_setjmp
.globl	_longjmp
.globl	csv, cret

_setjmp:
	mov	2(sp),r0
	mov	r5,(r0)+
	mov	sp,(r0)+
	mov	(sp),(r0)
	clr	r0
	rts	pc

_longjmp:
	jsr	r5,csv
	mov	4(r5),r1
	mov	6(r5),r0
	bne	1f
	mov	$1,r0
1:
	cmp	(r5),(r1)
	beq	2f
	mov	(r5),r5
	bne	1b
/ panic -- r2-r4 lost
	br	1f
2:
	/ can't use cret as number or args may be different.
	mov	r5,r2
	mov	-(r2),r4
	mov	-(r2),r3
	mov	-(r2),r2
1:
	mov	(r1)+,r5
	mov	(r1)+,sp
	mov	(r1),(sp)
	rts	pc


