/ low core

.data
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

. = ZERO+70
	muxin; br4+0;
	muxou; br4+0

. = ZERO+100
	kwlp; br6
	kwlp; br6

. = ZERO+114
	trap; br7+10.		/ 11/70 parity

. = ZERO+130
	muxin; br4+1;
	muxou; br4+1

. = ZERO+200
	lpou; br4

. = ZERO+204
	hsio; br5

. = ZERO+224
	htio; br5

. = ZERO+240
	trap; br7+7.		/ programmed interrupt
	trap; br7+8.		/ floating point
	trap; br7+9.		/ segmentation violation

. = ZERO+260
	dvhpio; br5

. = ZERO+500
	dzin; br5+0.
	dzou; br5+0.

	dzin; br5+1.
	dzou; br5+1.

	dzin; br5+2.
	dzou; br5+2.

	dzin; br5+3.
	dzou; br5+3.


//////////////////////////////////////////////////////
/		interface code to C
//////////////////////////////////////////////////////

.text
.globl	call, trap

.globl	_klrint
klin:	jsr	r0,call; jmp _klrint
.globl	_klxint
klou:	jsr	r0,call; jmp _klxint

.globl	_dzrint
dzin:	jsr	r0,call; jmp _dzrint
.globl	_dzxint
dzou:	jsr	r0,call; jmp _dzxint

.globl	_muxrint
muxin:	jsr	r0,call; jmp _muxrint
.globl	_muxxint
muxou:	jsr	r0,call; jmp _muxxint

.globl	_clock
kwlp:	jsr	r0,call; jmp _clock

.globl	_htintr
htio:	jsr	r0,call; jmp _htintr

.globl	_dvhpintr
dvhpio:	jsr	r0,call; jmp _dvhpintr

.globl _lpintr
lpou:	jsr	r0,call; jmp _lpintr

.globl	_hsintr
hsio:	jsr	r0,call; jmp _hsintr
