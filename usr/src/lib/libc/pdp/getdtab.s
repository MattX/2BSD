/ nds = getdtablesize();

.globl  _getdtablesize,cerror
local	= 58.
getdtablesize = 35.		/ get descriptor table size

_getdtablesize:
	mov	r5,-(sp)
	mov	sp,r5
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys	getdtablesize
.text
