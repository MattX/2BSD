/ low core

.text
ZERO:

br4 = 200
br5 = 240
br6 = 300
br7 = 340

. = ZERO+0
	br	1f
	4

/ trap vectors
	trap; br7+0.		/ bus error
	trap; br7+1.		/ illegal instruction
	trap; br7+2.		/ bpt-trace trap
	trap; br7+3.		/ iot trap
	trap; br7+4.		/ power fail
	trap; br7+5.		/ emulator trap
	start;br7+6.		/ system  (overlaid by 'trap')

. = ZERO+40
.globl	start, dump
1:	jmp	start
	jmp	dump


. = ZERO+60
	klin; br4
	klou; br4

. = ZERO+100
	kwlp; br6
	kwlp; br6

. = ZERO+114
	trap; br7+10.		/ 11/70 parity

. = ZERO+160
	rlio; br5

. = ZERO+200
	lpou; br4

. = ZERO+210
	hkio; br5

. = ZERO+220
	rkio; br5

. = ZERO+224
	tmio; br5
	htio; br5
	tsio; br5

. = ZERO+240
	trap; br7+7.		/ programmed interrupt
	trap; br7+8.		/ floating point
	trap; br7+9.		/ segmentation violation

. = ZERO+254
	xpio; br5
	dvhpio; br5
	rpio; br5

. = ZERO+300
	dzin; br5+0.
	dzou; br5+0.

/ overlay descriptor tables

. = ZERO+400
.globl	ova, ovd, ovend
ova:	.=.+16.		/ overlay addresses
ovd:	.=.+16.		/ overlay sizes
ovend:	.=.+2		/ end of overlays

//////////////////////////////////////////////////////
/		interface code to C
//////////////////////////////////////////////////////

.text
.globl	call, trap

.globl	_klrint
klin:	jsr	r0,call; jmp _klrint
.globl	_klxint
klou:	jsr	r0,call; jmp _klxint

.globl	_clock
kwlp:	jsr	r0,call; jmp _clock

.globl	_tmintr
tmio:	jsr	r0,call; jmp _tmintr

.globl	_tsintr
tsio:	jsr	r0,call; jmp _tsintr

.globl	_htintr
htio:	jsr	r0,call; jmp _htintr

.globl	_xpintr
xpio:	jsr	r0,call; jmp _xpintr

.globl	_dvhpintr
dvhpio:	jsr	r0,call; jmp _dvhpintr

.globl	_rpintr
rpio:	jsr	r0,call; jmp _rpintr

.globl	_rkintr
rkio:	jsr	r0,call; jmp _rkintr

.globl	_hkintr
hkio:	jsr	r0,call; jmp _hkintr

.globl	_rlintr
rlio:	jsr	r0,call; jmp _rlintr

.globl _lpintr
lpou:	jsr	r0,call; jmp _lpintr

.globl	_dzrint
dzin:	jsr	r0,call; jmp _dzrint
.globl	_dzxint
dzou:	jsr	r0,call; jmp _dzxint


