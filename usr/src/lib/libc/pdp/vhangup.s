.globl  _vhangup,cerror
local	= 58.
vhangup	= 16.		/ virtually hang up a control terminal

_vhangup:
	mov	r5,-(sp)
	mov	sp,r5
	clr     r0
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys     vhangup
.text
