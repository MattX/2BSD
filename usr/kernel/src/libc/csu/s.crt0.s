h46581
s 00001/00000/00036
d D 2.1 80/08/20 21:52:29 ucb 2 1
c UCB version, SCCS id keyword added (mss)
e
s 00036/00000/00000
d D 1.1 80/08/20 21:50:14 ucb 1 0
c date and time created 80/08/20 21:50:14 by ucb
e
u
U
f b 
f n 
t
T
I 2
/*	%W%	SCCS id keyword	*/
E 2
I 1
/ C runtime startoff

.globl	_exit, _environ
.globl	start
.globl	_main
exit = 1.

start:
	setd
	mov	2(sp),r0
	clr	-2(r0)
	mov	sp,r0
	sub	$4,sp
	mov	4(sp),(sp)
	tst	(r0)+
	mov	r0,2(sp)
1:
	tst	(r0)+
	bne	1b
	cmp	r0,*2(sp)
	blo	1f
	tst	-(r0)
1:
	mov	r0,4(sp)
	mov	r0,_environ
	jsr	pc,_main
	cmp	(sp)+,(sp)+
	mov	r0,(sp)
	jsr	pc,*$_exit
	sys	exit

.bss
_environ:
	.=.+2
.data
	.=.+2		/ loc 0 for I/D; null ptr points here.
E 1
