/ Startup code for two-stage bootstrap
/ wfj- mod's to allow non sep i/d machines, error recovery
/      note that the bootstrap passes the cputype through in
/      r0.

/ non-UNIX instructions
mfpi	= 6500^tst
stst	= 170300^tst
mtpi	= 6600^tst
mfpd	= 106500^tst
mtpd	= 106600^tst
spl	= 230
ldfps	= 170100^tst
stfps	= 170200^tst
wait	= 1
rtt	= 6
reset	= 5
/ trap	= 104400

PS	= 177776

.globl	_end
.globl	_main, __rtt
.globl	_edata
	jmp	start

/
/ trap vectors
/
	trap;340	/ bus error
	trap;341	/ illegal instruction
	trap;342	/ BPT
	trap;343	/ IOT
	trap;344	/ POWER FAIL
	trap;345	/ EMT
tvec:
	start;346	/ TRAP
.=400^.
.text


start:
	mov	$340,*$PS
	mov	$trap,tvec
/
/ restore what kind of cpu we are running on
	mov	r0,*$_cputype	/ assume that the boot left this in r0
/
/ fix up stack segment clobbered by trap
	cmp	$40.,_cputype
	bne	1f
	mov	$1400,*$KISA6
	br	2f
1:	mov	$1400,*$KDSA6
2:	mov	$157776,sp
	mov	$_edata,r0
	mov	$_end,r1
	sub	r0,r1
	inc	r1
	clc
	ror	r1
1:
	clr	(r0)+
	sob	r1,1b
	jsr	pc,_main

/ fix up stack to point at trap ps-pc pair,
/ located at bottom of memory.
/ so we can return to the bootstrap

__rtt:
	cmp	$40.,_cputype
	bne	1f
	clr	*$KISA6
	br	2f
1:	clr	*$KDSA6
2:	mov	$140000,sp
	rtt				/ we hope!
	br	.


.globl	_trap
trap:
	mov	*$PS,-(sp)
	mov	r0,-(sp)
	mov	r1,-(sp)
	tst	nofault
	bne	3f
	cmp	$40.,_cputype	/ decide wheither we are sep i/d
	bne	1f
	mfpi	6(sp)
	br	2f
1:	mfpd	6(sp)
2:	jsr	pc,_trap
	tst	(sp)+
	mov	(sp)+,r1
	mov	(sp)+,r0
	tst	(sp)+
	rtt
3:	mov	(sp)+,r1
	mov	(sp)+,r0
	tst	(sp)+
	mov	nofault,(sp)
	rtt

.data
.globl	_cputype

nofault:	.=.+2	/ where to go on predicted trap
_cputype:	.=.+2	/ cpu type (currently 40,45 or 70)
KISA6 = 172354
KISA7 = 172356
KDSA6 = 172374
UBMAP = 170200
