.globl  _socketa,cerror
local	= 58.
socketa = 26.   	/ get socket address

_socketa:
	mov	r5,-(sp)
	mov	sp,r5
	mov     4.(r5),0f
	mov     6.(r5),0f+2
	clr     r0
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys     socketa; 0:.. ; ..
.text
