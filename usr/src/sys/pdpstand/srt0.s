/ Startup code for standalone utilities
/ sms- mods to pass boot device and csr on to program
/ wfj- mod's to allow non sep i/d machines, error recovery
/      note that the bootstrap passes the cputype through in
/      r0.

PS	= 177776

.globl	_end
.globl	_main, __rtt, _devsw, _ADJcsr
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
	mov	r1,_bootcsr	/ assume that boot left csr in r1
	mov	r3,_bootdev	/ and boot device major,minor in r3
	mov	$157772,sp	/ return address,psw at 157774&6
	mov	$_edata,r0
	mov	$_end,r1
	sub	r0,r1
	inc	r1
	clc
	ror	r1
1:
	clr	(r0)+
	sob	r1,1b

/ controller number is in bits 6&7 of r3 (_bootdev).  major device number
/ is in bits 8-15.  what we need to do now is place the csr into
/ the appropriate driver's csrlist which is indexed by controller number.

	ash	$-5,r3			/ r3 = controller# * 2
	mov	r3,r0			/ controller number in bits 1,2
	bic	$!6,r0			/ make a word index
	ash	$-3,r3			/ major device # to bits 0-7
	mov	r3,r2			/ save major for later
	mul	$10.,r3			/ devsw[] members are 10. bytes each
	mov	_devsw+10(r3),r3	/ get csrlist for driver
	add	r0,r3			/ point to controller's entry

/ the CSR passed from the ROMs is not necessarily the first address
/ of a device!  We therefore have to adjust the CSR so that the structure
/ pointers in the drivers are origined at the base address rather than
/ the ROM supplied address.  The adjustment was not stored in devsw[] to
/ save space (can get by with an array of bytes instead of words).

	movb	_ADJcsr(r2),r2		/ adjust (possibly) the CSR
	mov	_bootcsr,r1
	sub	r2,r1

	mov	r1,(r3)			/ store controller's csr in table
	jsr	pc,_main

/ fix up stack to point at trap ps-pc pair located at top of memory
/ so we can return to the bootstrap.
/
/ the return protocol originally changed from 140000 because icheck was
/ too large.  icheck was reduced in size but the 157774 return protocol
/ was retained to allow for slightly oversized programs.

__rtt:
	mov	$157774,sp
	rtt				/ we hope!
	br	.


.globl	_trap
trap:
	mov	*$PS,-(sp)
	tst	nofault
	bne	3f
	mov	r0,-(sp)
	mov	r1,-(sp)
2:	jsr	pc,_trap
	mov	(sp)+,r1
	mov	(sp)+,r0
	tst	(sp)+
	rtt
3:	tst	(sp)+
	mov	nofault,(sp)
	rtt

.data
.globl	_cputype, _bootcsr, _bootdev

nofault:	.=.+2	/ where to go on predicted trap
_cputype:	.=.+2	/ cpu type (currently 44, 70, 73)
_bootdev:	.=.+2	/ makedev(major,unit) for boot device
_bootcsr:	.=.+2	/ csr of boot controller
