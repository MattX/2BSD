.globl  _select,cerror
local	= 58.
select  = 18.   	/ select active fd

_select:
	mov	r5,-(sp)
	mov	sp,r5
	mov     4.(r5),0f
	mov     6.(r5),0f+2
	mov     8.(r5),0f+4
	mov     10.(r5),0f+6
	mov     12.(r5),0f+8.
	clr     r0
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys     select; 0:.. ; .. ; .. ; .. ; ..
.text
