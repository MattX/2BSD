.globl  _socket,cerror
local	= 58.
socket  = 21.   	/ get socket fd

_socket:
	mov	r5,-(sp)
	mov	sp,r5
	mov     4.(r5),0f
	mov     6.(r5),0f+2
	mov     8.(r5),0f+4
	mov     10.(r5),0f+6
	clr     r0
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys     socket; 0:.. ; .. ; .. ; ..
.text
