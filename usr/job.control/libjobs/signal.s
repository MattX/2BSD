/ C library -- sigsys

/ sigsys(n, SIG_DFL);		/* default action on signal(n) */
/ sigsys(n, SIG_HOLD);		/* block signal temporarily */
/ sigsys(n, SIG_IGN);		/* ignore signal(n) */
/ sigsys(n, label);		/* goto label on signal(n) */
/ sigsys(n, DEFERSIG(label));	/* goto label with signal SIG_HOLD */

/ returns old label, only one level.

.globl	_sigsys
.globl	cerror

_sigsys:
	mov	r5,-(sp)
	mov	sp,r5
	mov	4(r5),0f
	mov	6(r5),0f+2
	sys	0; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys	signal; 0:..; ..

