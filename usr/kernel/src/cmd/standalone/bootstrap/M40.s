/ Startup code for two-stage bootstrap
/ Hacked to force to 40boot for distribution boot tape

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
halt	= 0
reset	= 5
systrap = 104400

.globl	_end
.globl	_main,_ubmapset
	jmp	start

/
/ trap vectors
/
	trap;340	/ bus error -- grok!
	trap;341	/ illegal instruction
	trap;342	/ BPT
	trap;343	/ IOT
	trap;344	/ POWER FAIL
	trap;345	/ EMT
tvec:
	start;346	/ TRAP
.=400^.


start:
	reset
	mov	$340,PS
	mov	$140100,sp
/
/ determine what kind of cpu we are running on
/ first, check switches. if they are 40,45 or 70
/ set appropriately
/ otherwise, force to 40 since overlaid kernel does't boot otherwise
/
	mov	$1f,nofault	/ check if we even have switches!
	tst	*$SWREG
	clr	nofault		/ apparently we do
	mov	$40.,r0
	cmp	*$SWREG,$40
	beq	gotcha
	mov	$45.,r0
	cmp	*$SWREG,$45
	beq	gotcha
	mov	$70.,r0
	cmp	*$SWREG,$70
	beq	gotcha
1:	mov	$40.,r0		/ assume 11/40
	clr	nofault
gotcha:
	mov	r0,_cputype
/
/ if not an 11/40 (or 34, 60 etc), setup for sep i/d
	cmp	_cputype,$40.
	jeq	type40
type45:
/
/ set kernel I+D to physical 0 and IO page
	clr	r1
	mov	$77406,r2
	mov	$KISA0,r3
	mov	$KISD0,r4
	jsr	pc,setseg
	mov	$IO,-(r3)
	clr	r1
	mov	$KDSA0,r3
	mov	$KDSD0,r4
	jsr	pc,setseg
	mov	$IO,-(r3)

/ set user I+D to physical 64K (words) and IO page
	mov	$4000,r1
	mov	$UISA0,r3
	mov	$UISD0,r4
	jsr	pc,setseg
	mov	$IO,-(r3)
	mov	$4000,r1
	mov	$UDSA0,r3
	mov	$UDSD0,r4
	jsr	pc,setseg
	mov	$IO,-(r3)
	jbr	1f
/
/ if 11/40, only need to diddle i reg's.
type40:

/ set kernel I to physical 0 and IO page
	clr	r1
	mov	$77406,r2
	mov	$KISA0,r3
	mov	$KISD0,r4
	jsr	pc,setseg
	mov	$IO,-(r3)

/ set user I to physical 64K (words) and IO page
	mov	$4000,r1
	mov	$UISA0,r3
	mov	$UISD0,r4
	jsr	pc,setseg
	mov	$IO,-(r3)

1:
/ enable map
	cmp	_cputype,$70.
	jne	1f
	jsr	pc,_ubmapset
	mov	$65,SSR3	/ 22-bit map
	mov	$3,MSCR
1:
	cmp	_cputype,$45.
	bne	1f
	mov	$5,SSR3
1:
	mov	$30340,PS
	inc	SSR0


/ copy program to user I space
	mov	$_end,r0
	asr	r0
	clr	r1
1:
	mov	(r1),-(sp)
	mtpi	(r1)+
	sob	r0,1b


/ continue execution in user space copy
	mov	$140004,sp
	cmp	_cputype,$40.
	bne	1f
	clr	*$KISA6
	br	2f
1:
	clr	*$KDSA6
2:	mov	$140340,-(sp)
	mov	$user,-(sp)
	rtt
user:
	mov	$_end+512.,sp
	mov	sp,r5

	jsr	pc,_main
	mov	_cputype,r0
	systrap

	br	user

setseg:
	mov	$8,r0
1:
	mov	r1,(r3)+
	add	$200,r1
	mov	r2,(r4)+
	sob	r0,1b
	rts	pc

.globl	_setseg
_setseg:
	mov	2(sp),r1
	mov	r2,-(sp)
	mov	r3,-(sp)
	mov	r4,-(sp)
	mov	$77406,r2
	mov	$KISA0,r3
	mov	$KISD0,r4
	jsr	pc,setseg
	mov	$IO,-(r3)
	mov	(sp)+,r4
	mov	(sp)+,r3
	mov	(sp)+,r2
	rts	pc

.globl	_setnosep
_setnosep:
	bic	$4,SSR3	/ turn off kernel i/d sep
	rts pc

.globl	_setsep
_setsep:
	bis	$4,SSR3	/ turn on kernel i/d sep (if not already)
	rts pc

/ clrseg(addr,count)
.globl	_clrseg
_clrseg:
	mov	4(sp),r0
	beq	2f
	asr	r0
	bic	$!77777,r0
	mov	2(sp),r1
1:
	clr	-(sp)
	mtpi	(r1)+
	sob	r0,1b
2:
	rts	pc


/ mtpi(word,addr)
.globl	_mtpi
_mtpi:
	mov	4(sp),r0
	mov	2(sp),-(sp)
	mtpi	(r0)+
	rts	pc

.globl	__rtt
__rtt:
	halt

.globl	_trap

trap:
	mov	*$PS,-(sp)
	mov	r0,-(sp)
	mov	r1,-(sp)
	tst	nofault
	bne	3f
	cmp	$40.,_cputype	/ find out whether we are sep i/d
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

PS	= 177776
SSR0	= 177572
SSR1	= 177574
SSR2	= 177576
SSR3	= 172516
KISA0	= 172340
KISA1	= 172342
KISA6	= 172354
KISA7	= 172356
KISD0	= 172300
KISD7	= 172316
KDSA0	= 172360
KDSA6	= 172374
KDSA7	= 172376
KDSD0	= 172320
KDSD5	= 172332
SISA0	= 172240
SISA1	= 172242
SISD0	= 172200
SISD1	= 172202
UISA0	= 177640
UISD0	= 177600
UDSA0	= 177660
UDSD0	= 177620
MSCR	= 017777746	/ 11/70 memory control register
IO	= 177600
SWREG	= 177570
UBMAP	= 170200


.data
.globl	_cputype

nofault:	.=.+2	/ where to go on predicted trap
_cputype:	.=.+2	/ cpu type (currently 40,45 or 70)
