/	<@(#)m40.s	3.2\0>

/ machine language assist
/ for 11/40
/ modified for overlaid kernel
/ Bill Shannon - 3/80

/ parameters

.fpp	= 1		/ 0 => don't even look for floating point processor
.pi	= 0		/ 1 => include piget and piput
CGL_RTP = 1		/ 1 == CGL_RTP
.METER	= 1		/ == UCB_METER
.LOAD	= 1		/ == UCB_LOAD
.dump	= 1		/ Mag tape dump.


/ non-UNIX instructions
mfpi	= 6500^tst
mtpi	= 6600^tst
halt	= 0
wait	= 1
reset	= 5
rtt	= 6

.globl	start, _end, _edata, _main, dump
.globl	ovend, ova, ovd
.if	.dump
/ Mag tape dump
/ save registers in low core and
/ write all core onto mag tape.
/ entry is thru 44 abs

dump:

/ save regs r0,r1,r2,r3,r4,r5,r6,KIA6
/ starting at abs location 4

	inc	$-1		/ check for first time
	bne	1f		/ if not, don't save registers again
	mov	r0,4
	mov	$6,r0
	mov	r1,(r0)+
	mov	r2,(r0)+
	mov	r3,(r0)+
	mov	r4,(r0)+
	mov	r5,(r0)+
	mov	sp,(r0)+
	mov	KISA6,(r0)+
1:

/ dump all of core (ie to first mt error)
/ onto mag tape. (9 track or 7 track 'binary')

.if HTDUMP
	mov	$HTCS1,r0
	mov	$40,*$HTCS2
	mov	$2300,*$HTTC
	clr	*$HTBA
	mov	$1,(r0)
1:
	mov	$-512.,*$HTFC
	mov	$-256.,*$HTWC
	movb	$61,(r0)
2:
	tstb	(r0)
	bge	2b
	bit	$1,(r0)
	bne	2b
	bit	$40000,(r0)
	beq	1b
	mov	$27,(r0)
.endif
HT	= 0172440
HTCS1	= HT+0
HTWC	= HT+2
HTBA	= HT+4
HTFC	= HT+6
HTCS2	= HT+10
HTTC	= HT+32

MTC = 172522
.if TUDUMP
	mov	$MTC,r0
	mov	$60004,(r0)+
	clr	2(r0)
1:
	mov	$-512.,(r0)
	inc	-(r0)
2:
	tstb	(r0)
	bge	2b
	tst	(r0)+
	bge	1b
	reset

/ end of file and loop

	mov	$60007,-(r0)
.endif
.endif
	br	.

start:
	bit	$1,SSR0		/ is memory management enabled ?
	beq	.		/ better be !!!
	mov	$trap,34

/ Set location 0 and 2 to catch traps and jumps to 0

	mov	$42,0		/ illegal instruction if jump
	mov	$777,2		/ trace trap at high priority if trap

/ set kernel page descriptors
	mov	$KISD0,r0
	mov	$77402,(r0)+		/ base segment 0
	mov	$77402,(r0)+		/ base segment 1
	mov	$0,(r0)+		/ overlay, mapped when first called
	mov	$KISD5,r0
	mov	$_end,r1
	cmp	$120000,r1		/ test if KISA5 used
	blo	1f
	beq	2f
	mov	$0,(r0)
	mov	$KISD4,r0
1:	dec	r1			/ round down to clicks
2:	bic	$!37700,r1
	ash	$2,r1
	bis	$6,r1
	mov	r1,(r0)			/ for partial bss segment (4 or 5)

/ initialize user segment

	mov	ovend,KISA6		/ ksr6 = sysu
	mov	$usize-1\<8|6,KISD6

/ get a sp

	mov	$_u+[usize*64.],sp

.if	.fpp
/ test for floating point
	mov	$1f,nofault
	setd				/ jumps to 1f if no fpu
	inc	fpp
1:
.endif
	clr	nofault

/ clear bss

	mov	$_edata,r0
1:
	clr	(r0)+
	cmp	r0,$_end
	blo	1b

/ clear user block

	mov	$_u,r0
1:
	clr	(r0)+
	cmp	r0,$_u+[usize*64.]
	blo	1b

/ set up previous mode and call main
/ on return, enter user mode at 0R

	mov	$30000,PS
	jsr	pc,_main
	mov	$170000,-(sp)
	clr	-(sp)
	rtt

.globl	trap, call
.globl	_trap

/ all traps and interrupts are
/ vectored thru this routine.

trap:
	mov	PS,saveps
	tst	nofault
	bne	1f
	mov	SSR0,ssr
	mov	SSR2,ssr+4
	mov	$1,SSR0
	jsr	r0,call1; jmp _trap
	/ no return
1:
	mov	$1,SSR0
	mov	nofault,(sp)
	rtt
.text

.globl	_runrun
call1:
	mov	saveps,-(sp)
	bic	$HIPRI,PS
	br	1f

call:
	mov	PS,-(sp)
1:
/ the following code shuffles the stack so that the overlay number
/ is saved higher on the stack than the other state information.
/ this is so that the C versions of trap, etc. don't have to be
/ changed when running with an overlaid kernel.

	mov	(sp),-(sp)	/ move down nps
	mov	4(sp),2(sp)	/ move down r0
	mov	6(sp),4(sp)	/ move down pc
	mov	8.(sp),6(sp)	/ move down ps
	mov	__ovno,8.(sp)	/ save overlay number

	mov	r1,-(sp)
	mfpi	sp
	.if	.METER
.globl	_cnt
/ cnt.v_intr++;		#ifdev UCB_METER; count device interrupts
	add	$1,16+_cnt
	adc	14+_cnt
/ #endif UCB_METER
	.endif
	mov	4(sp),-(sp)
	bic	$!37,(sp)
	bit	$30000,PS
	beq	1f
	jsr	pc,(r0)+
	tstb	_runrun
	beq	2f
	mov	$12.,(sp)		/ trap 12 is give up cpu
	jsr	pc,_trap
2:
	tst	(sp)+
	mtpi	sp
	br	2f
1:
	bis	$30000,PS
	jsr	pc,(r0)+
	cmp	(sp)+,(sp)+
2:
/ restore previous overlay number and mapping.
/ unshuffle stack.

	mov	10.(sp),r0	/ get saved overlay number
	mov	PS,-(sp)	/ save PS
	bis	$HIPRI,PS	/ mask all interrupts
	mov	r0,__ovno
	asl	r0
	mov	ova(r0),KISA2
	mov	ovd(r0),KISD2
	mov	(sp)+,PS	/ restore PS, unmask interrupts
	mov	8.(sp),10.(sp)	/ move ps up
	mov	6.(sp),8.(sp)	/ move pc up

	mov	(sp)+,r1
	tst	(sp)+
	mov	(sp)+,r0
	tst	(sp)+
	rtt
.globl	_savfp
_savfp:
.if	.fpp
	tst	fpp
	beq	8f
	mov	2(sp),r1
	stfps	(r1)+
	setd
	movf	fr0,(r1)+
	movf	fr1,(r1)+
	movf	fr2,(r1)+
	movf	fr3,(r1)+
	movf	fr4,fr0
	movf	fr0,(r1)+
	movf	fr5,fr0
	movf	fr0,(r1)+
8:
.endif
	rts	pc

.globl	_restfp
_restfp:
.if	.fpp
	tst	fpp
	beq	8f
	mov	2(sp),r1
	mov	r1,r0
	setd
	add	$8.+2.,r1
	movf	(r1)+,fr1
	movf	(r1)+,fr2
	movf	(r1)+,fr3
	movf	(r1)+,fr0
	movf	fr0,fr4
	movf	(r1)+,fr0
	movf	fr0,fr5
	movf	2(r0),fr0
	ldfps	(r0)
8:
.endif
	rts	pc

.globl	_addupc
_addupc:
	mov	r2,-(sp)
	mov	6(sp),r2		/ base of prof with base,leng,off,scale
	mov	4(sp),r0		/ pc
	sub	4(r2),r0		/ offset
	clc
	ror	r0
	mov	6(r2),r1
	clc
	ror	r1
	mul	r1,r0		/ scale
	ashc	$-14.,r0
	inc	r1
	bic	$1,r1
	cmp	r1,2(r2)		/ length
	bhis	1f
	add	(r2),r1		/ base
	mov	nofault,-(sp)
	mov	$2f,nofault
	mfpi	(r1)
	add	12.(sp),(sp)
	mtpi	(r1)
	br	3f
2:
	clr	6(r2)
3:
	mov	(sp)+,nofault
1:
	mov	(sp)+,r2
	rts	pc

.globl	_display
_display:
	rts	pc


.globl	_backup
.globl	_regloc
_backup:
	mov	2(sp),ssr+2
	mov	r2,-(sp)
	jsr	pc,backup
	mov	r2,ssr+2
	mov	(sp)+,r2
	movb	jflg,r0
	bne	2f
	mov	2(sp),r0
	movb	ssr+2,r1
	jsr	pc,1f
	movb	ssr+3,r1
	jsr	pc,1f
	movb	_regloc+7,r1
	asl	r1
	add	r0,r1
	mov	ssr+4,(r1)
	clr	r0
2:
	rts	pc
1:
	mov	r1,-(sp)
	asr	(sp)
	asr	(sp)
	asr	(sp)
	bic	$!7,r1
	movb	_regloc(r1),r1
	asl	r1
	add	r0,r1
	sub	(sp)+,(r1)
	rts	pc

/ hard part
/ simulate the ssr2 register missing on 11/40

backup:
	clr	r2		/ backup register ssr1
	mov	$1,bflg		/ clrs jflg
	mov	ssr+4,r0
	jsr	pc,fetch
	mov	r0,r1
	ash	$-11.,r0
	bic	$!36,r0
	jmp	*0f(r0)
0:		t00; t01; t02; t03; t04; t05; t06; t07
		t10; t11; t12; t13; t14; t15; t16; t17

t00:
	clrb	bflg

t10:
	mov	r1,r0
	swab	r0
	bic	$!16,r0
	jmp	*0f(r0)
0:		u0; u1; u2; u3; u4; u5; u6; u7

u6:	/ single op, m[tf]pi, sxt, illegal
	bit	$400,r1
	beq	u5		/ all but m[tf], sxt
	bit	$200,r1
	beq	1f		/ mfpi
	bit	$100,r1
	bne	u5		/ sxt

/ simulate mtpi with double (sp)+,dd
	bic	$4000,r1	/ turn instr into (sp)+
	br	t01

/ simulate mfpi with double ss,-(sp)
1:
	ash	$6,r1
	bis	$46,r1		/ -(sp)
	br	t01

u4:	/ jsr
	mov	r1,r0
	jsr	pc,setreg	/ assume no fault
	bis	$173000,r2	/ -2 from sp
	rts	pc

t07:	/ EIS
	clrb	bflg

u0:	/ jmp, swab
u5:	/ single op
f5:	/ movei, movfi
ff1:	/ ldfps
ff2:	/ stfps
ff3:	/ stst
	mov	r1,r0
	br	setreg

t01:	/ mov
t02:	/ cmp
t03:	/ bit
t04:	/ bic
t05:	/ bis
t06:	/ add
t16:	/ sub
	clrb	bflg

t11:	/ movb
t12:	/ cmpb
t13:	/ bitb
t14:	/ bicb
t15:	/ bisb
	mov	r1,r0
	ash	$-6,r0
	jsr	pc,setreg
	swab	r2
	mov	r1,r0
	jsr	pc,setreg

/ if delta(dest) is zero,
/ no need to fetch source

	bit	$370,r2
	beq	1f

/ if mode(source) is R,
/ no fault is possible

	bit	$7000,r1
	beq	1f

/ if reg(source) is reg(dest),
/ too bad.

	mov	r2,-(sp)
	bic	$174370,(sp)
	cmpb	1(sp),(sp)+
	beq	u7

/ start source cycle
/ pick up value of reg

	mov	r1,r0
	ash	$-6,r0
	bic	$!7,r0
	movb	_regloc(r0),r0
	asl	r0
	add	ssr+2,r0
	mov	(r0),r0

/ if reg has been incremented,
/ must decrement it before fetch

	bit	$174000,r2
	ble	2f
	dec	r0
	bit	$10000,r2
	beq	2f
	dec	r0
2:

/ if mode is 6,7 fetch and add X(R) to R

	bit	$4000,r1
	beq	2f
	bit	$2000,r1
	beq	2f
	mov	r0,-(sp)
	mov	ssr+4,r0
	add	$2,r0
	jsr	pc,fetch
	add	(sp)+,r0
2:

/ fetch operand
/ if mode is 3,5,7 fetch *

	jsr	pc,fetch
	bit	$1000,r1
	beq	1f
	bit	$6000,r1
	bne	fetch
1:
	rts	pc

t17:	/ floating point instructions
	clrb	bflg
	mov	r1,r0
	swab	r0
	bic	$!16,r0
	jmp	*0f(r0)
0:		f0; f1; f2; f3; f4; f5; f6; f7

f0:
	mov	r1,r0
	ash	$-5,r0
	bic	$!16,r0
	jmp	*0f(r0)
0:		ff0; ff1; ff2; ff3; ff4; ff5; ff6; ff7

f1:	/ mulf, modf
f2:	/ addf, movf
f3:	/ subf, cmpf
f4:	/ movf, divf
ff4:	/ clrf
ff5:	/ tstf
ff6:	/ absf
ff7:	/ negf
	inc	fflg
	mov	r1,r0
	br	setreg

f6:
	bit	$400,r1
	beq	f1	/ movfo
	br	f5	/ movie

f7:
	bit	$400,r1
	beq	f5	/ movif
	br	f1	/ movof

ff0:	/ cfcc, setf, setd, seti, setl
u1:	/ br
u2:	/ br
u3:	/ br
u7:	/ illegal
	incb	jflg
	rts	pc

setreg:
	mov	r0,-(sp)
	bic	$!7,r0
	bis	r0,r2
	mov	(sp)+,r0
	ash	$-3,r0
	bic	$!7,r0
	movb	0f(r0),r0
	tstb	bflg
	beq	1f
	bit	$2,r2
	beq	2f
	bit	$4,r2
	beq	2f
1:
	cmp	r0,$20
	beq	2f
	cmp	r0,$-20
	beq	2f
	asl	r0
2:
	tstb	fflg
	beq	3f
	asl	r0
	stfps	r1
	bit	$200,r1
	beq	3f
	asl	r0
3:
	bisb	r0,r2
	rts	pc

0:	.byte	0,0,10,20,-10,-20,0,0

fetch:
	bic	$1,r0
	mov	nofault,-(sp)
	mov	$1f,nofault
	mfpi	(r0)
	mov	(sp)+,r0
	mov	(sp)+,nofault
	rts	pc

1:
 	mov	(sp)+,nofault
	clrb	r2			/ clear out dest on fault
	mov	$-1,r0
	rts	pc

.bss
bflg:	.=.+1
jflg:	.=.+1
fflg:	.=.+1
.text

.text

.globl	_fubyte, _subyte
.globl	_fuibyte, _suibyte
.globl	_fuword, _suword
.globl	_fuiword, _suiword
_fuibyte:
_fubyte:
	mov	2(sp),r1
	bic	$1,r1
	jsr	pc,gword
	cmp	r1,2(sp)
	beq	1f
	swab	r0
1:
	bic	$!377,r0
	rts	pc

_suibyte:
_subyte:
	mov	2(sp),r1
	bic	$1,r1
	jsr	pc,gword
	mov	r0,-(sp)
	cmp	r1,4(sp)
	beq	1f
	movb	6(sp),1(sp)
	br	2f
1:
	movb	6(sp),(sp)
2:
	mov	(sp)+,r0
	jsr	pc,pword
	clr	r0
	rts	pc
_fuiword:
_fuword:
	mov	2(sp),r1
fuword:
	jsr	pc,gword
	rts	pc

gword:
	mov	PS,-(sp)
	bis	$HIPRI,PS
	mov	nofault,-(sp)
	mov	$err,nofault
	mfpi	(r1)
	mov	(sp)+,r0
	br	1f

_suiword:
_suword:
	mov	2(sp),r1
	mov	4(sp),r0
suword:
	jsr	pc,pword
	rts	pc

pword:
	mov	PS,-(sp)
	bis	$HIPRI,PS
	mov	nofault,-(sp)
	mov	$err,nofault
	mov	r0,-(sp)
	mtpi	(r1)
1:
	mov	(sp)+,nofault
	mov	(sp)+,PS
	rts	pc

err:
	mov	(sp)+,nofault
	mov	(sp)+,PS
	tst	(sp)+
	mov	$-1,r0
	rts	pc
	rts	pc
.globl	_copyin, _copyout
.globl	_copyiin, _copyiout
_copyiin:
_copyin:
	jsr	pc,copsu
1:
	mfpi	(r0)+
	mov	(sp)+,(r1)+
	sob	r2,1b
	br	2f

_copyiout:
_copyout:
	jsr	pc,copsu
1:
	mov	(r0)+,-(sp)
	mtpi	(r1)+
	sob	r2,1b
2:
	mov	(sp)+,nofault
	mov	(sp)+,r2
	clr	r0
	rts	pc

copsu:
	mov	(sp)+,r0
	mov	r2,-(sp)
	mov	nofault,-(sp)
	mov	r0,-(sp)
	mov	10(sp),r0
	mov	12(sp),r1
	mov	14(sp),r2
	asr	r2
	mov	$1f,nofault
	rts	pc

1:
	mov	(sp)+,nofault
	mov	(sp)+,r2
	mov	$-1,r0
	rts	pc

.globl	_idle, _waitloc
_idle:
	mov	PS,-(sp)
	bic	$HIPRI,PS
	wait
waitloc:
	mov	(sp)+,PS
	rts	pc
	.data
_waitloc:
	waitloc
	.text

.globl	_save
_save:
	mov	(sp)+,r1
	mov	(sp),r0
	mov	r2,(r0)+
	mov	r3,(r0)+
	mov	r4,(r0)+
	mov	r5,(r0)+
	mov	sp,(r0)+
	mov	__ovno,(r0)+
	mov	r1,(r0)+
	clr	r0
	jmp	(r1)

.globl	_resume
_resume:
	mov	2(sp),r0		/ new process
	mov	4(sp),r1		/ new stack
	bis	$HIPRI,PS
	mov	r0,KISA6		/ In new process
	mov	(r1)+,r2
	mov	(r1)+,r3
	mov	(r1)+,r4
	mov	(r1)+,r5
	mov	(r1)+,sp
	mov	(r1)+,r0
	mov	r0,__ovno
	asl	r0
	mov	ova(r0),KISA2
	mov	ovd(r0),KISD2
	mov	$1,r0
	bic	$HIPRI,PS
	jmp	*(r1)+

.globl	_spl0, _spl1, _spl4, _spl5, _spl6, _spl7, _splx
_spl0:
	mov	PS,r0
	bic	$HIPRI,PS
	rts	pc

_spl1:
	mov	PS,r0
	bis	$HIPRI,PS
	bic	$300,PS
	rts	pc

_spl4:
	mov	PS,r0
	bis	$HIPRI,PS
	bic	$140,PS
	rts	pc

_spl5:
	mov	PS,r0
	bis	$HIPRI,PS
	bic	$100,PS
	rts	pc

_spl6:
	mov	PS,r0
	bis	$HIPRI,PS
	bic	$40,PS
	rts	pc

_spl7:
	mov	PS,r0
	bis	$HIPRI,PS
	rts	pc

_splx:
	mov	2(sp),PS
	rts	pc

.globl	_copyseg
_copyseg:
	.if	CGL_RTP
.globl	_copy1seg, _rtpp, _runrtp
	tst	_rtpp
	beq	1f
	jsr	pc,_runrtp
1:
_copy1seg:
	.endif
	mov	PS,-(sp)
	mov	UISA0,-(sp)
	mov	UISA1,-(sp)
	mov	$30340,PS
	mov	10(sp),UISA0
	mov	12(sp),UISA1
	mov	UISD0,-(sp)
	mov	UISD1,-(sp)
	mov	$6,UISD0
	mov	$6,UISD1
	mov	r2,-(sp)
	clr	r0
	mov	$8192.,r1
	mov	$32.,r2
1:
	mfpi	(r0)+
	mtpi	(r1)+
	sob	r2,1b
	mov	(sp)+,r2
	mov	(sp)+,UISD1
	mov	(sp)+,UISD0
	mov	(sp)+,UISA1
	mov	(sp)+,UISA0
	mov	(sp)+,PS
	rts	pc

.globl	_clearseg
_clearseg:
	.if	CGL_RTP
	tst	_rtpp
	beq	1f
	jsr	pc,_runrtp
1:
	.endif
	mov	PS,-(sp)
	mov	UISA0,-(sp)
	mov	$30340,PS
	mov	6(sp),UISA0
	mov	UISD0,-(sp)
	mov	$6,UISD0
	clr	r0
	mov	$32.,r1
1:
	clr	-(sp)
	mtpi	(r0)+
	sob	r1,1b
	mov	(sp)+,UISD0
	mov	(sp)+,UISA0
	mov	(sp)+,PS
	rts	pc

.if	.pi
.globl	_piget, _piput
_piget:
	mov	PS,-(sp)
	jsr	pc,2f
	mfpi	(r0)
	mov	(sp)+,r0
1:
	mov	(sp)+,PS
	rts	pc

_piput:
	mov	PS,-(sp)
	jsr	pc,2f
	mov	10(sp),r1
	mov	r1,-(sp)
	mtpi	(r0)
	br	1b
2:
	mov	6(sp),r0
	mov	10(sp),r1
	ashc	$-6,r0
	mov	HIPRI,PS
	mov	r1,KISA7
	mov	10(sp),r0
	bic	$!77,r0
	bis	$160000,r0
	rts	pc
.endif

/ Long quotient

	.globl	ldiv
ldiv:
	jsr	r5,csv
	mov	10.(r5),r3
	sxt	r4
	bpl	1f
	neg	r3
1:
	cmp	r4,8.(r5)
	bne	hardldiv
	mov	6.(r5),r2
	mov	4.(r5),r1
	bge	1f
	neg	r1
	neg	r2
	sbc	r1
	com	r4
1:
	mov	r4,-(sp)
	clr	r0
	div	r3,r0
	mov	r0,r4		/high quotient
	mov	r1,r0
	mov	r2,r1
	div	r3,r0
	bvc	1f
	sub	r3,r0		/ this is the clever part
	div	r3,r0
	tst	r1
	sxt	r1
	add	r1,r0		/ cannot overflow!
1:
	mov	r0,r1
	mov	r4,r0
	tst	(sp)+
	bpl	9f
	neg	r0
	neg	r1
	sbc	r0
9:
	jmp	cret

hardldiv:
	4

/ Long remainder

	.globl	lrem
lrem:
	jsr	r5,csv
	mov	10.(r5),r3
	sxt	r4
	bpl	1f
	neg	r3
1:
	cmp	r4,8.(r5)
	bne	hardlrem
	mov	6.(r5),r2
	mov	4.(r5),r1
	mov	r1,r4
	bge	1f
	neg	r1
	neg	r2
	sbc	r1
1:
	clr	r0
	div	r3,r0
	mov	r1,r0
	mov	r2,r1
	div	r3,r0
	bvc	1f
	sub	r3,r0
	div	r3,r0
	tst	r1
	beq	9f
	add	r3,r1
1:
	tst	r4
	bpl	9f
	neg	r1
9:
	sxt	r0
	jmp	cret

/ The divisor is known to be >= 2^15.	Only 16 cycles are
/ needed to get a remainder.
hardlrem:
	4

.if	.LOAD
/ lmul added back into the system for UCB_LOAD
.globl	lmul
lmul:
	mov	r2,-(sp)
	mov	r3,-(sp)
	mov	8(sp),r2
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
.endif

/ C register save and restore -- version 7/75
/ modified by wnj && cbh 6/79 for overlaid text registers
/ modified by was 3/80 for use in kernel
/ inter-overlay calls call thunk which calls ovhndlr to
/ save registers.  intra-overlay calls call function
/ directly which calls csv to save registers.

.globl	csv
.globl	cret
.globl	ovhndlr
.globl  __ovno
.globl  _etext
.data
__ovno:	0
.text

csv:
	mov	r5,r1
	mov	sp,r5
	mov	__ovno,-(sp)	/ overlay is extra (first) word in mark
	mov	r4,-(sp)
	mov	r3,-(sp)
	mov	r2,-(sp)
	jsr	pc,(r1)		/ jsr part is sub $2,sp

cret:
	mov	r5,r2
/ get the overlay out of the mark, and if it is non-zero
/ make sure it is the currently loaded one
	mov	-(r2),r4
	bne	1f		/ zero is easy
2:
	mov	-(r2),r4
	mov	-(r2),r3
	mov	-(r2),r2
	mov	r5,sp
	mov	(sp)+,r5
	rts	pc
/ not returning to base segment, so check that the right
/ overlay is mapped in, and if not change the mapping
1:
	cmp	r4,__ovno
	beq	2b		/ lucked out!
/ if return address is in base segment, then nothing to do
	cmp	2(r5),$_etext
	blos	2b
/ returning to wrong overlay --- do something!
	mov	PS,-(sp)	/ save PS 
	bis	$HIPRI,PS	/ mask interrupts
	mov	r4,__ovno
	asl	r4
	mov	ova(r4),KISA2
	mov	ovd(r4),KISD2
	mov	(sp)+,PS	/ restore PS, unmask interrupts
/ could measure switches[ovno][r4]++ here
	jmp	2b

/ ovhndlr makes the argument (in r0) be the current overlay,
/ saves the registers ala csv (but saves the previous overlay number),
/ and then jmp's to the function, skipping the function's initial
/ call to csv.
ovhndlr:
	mov	r5,r1
	mov	sp,r5
	mov	__ovno,-(sp)	/ save previous overlay number
	mov	PS,-(sp)	/ save PS
	bis	$HIPRI,PS	/ mask interrupts
	mov	r0,__ovno	/ set new overlay number
	asl	r0
	mov	ova(r0),KISA2
	mov	ovd(r0),KISD2
	mov	(sp)+,PS	/ restore PS, unmask interrupts
	mov	r4,-(sp)
	mov	r3,-(sp)
	mov	r2,-(sp)
	mov	-6(r1),r1	/ pick up address of function
	jsr	pc,4(r1)	/ skip function's call to csv

.globl	_u
_u	= 140000
usize	= 16.

HIPRI	= 340

PS	= 177776
SSR0	= 177572
SSR2	= 177576
KISA0	= 172340
KISA2	= 172344
KISA6	= 172354
KISA7	= 172356
KISD0	= 172300
KISD2	= 172304
KISD4	= 172310
KISD5	= 172312
KISD6	= 172314
MTC	= 172522
TUC	= 172440
UISA0	= 177640
UISA1	= 177642
UISD0	= 177600
UISD1	= 177602
IO	= 7600

.data
.globl	_ka6
.globl	_cputype

_ka6:	KISA6
_cputype:40.
stk:	0
.if	.fpp
fpp:	0
.endif

.bss
nofault:.=.+2
ssr:	.=.+6
saveps: .=.+2
power:	.=.+2

.text
.globl	_stst
stst	= 170300^tst
_stst:
.if	.fpp
	tst	fpp
	beq	9f
	stst	r0
	mov	r0,*2(sp)
9:
.endif
