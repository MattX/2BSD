/ error = getgroups(ngroups, grouplist);
/	unsigned int ngroups;
/	int *grouplist;

.globl	_getgroups
.globl	cerror

getgroups = 33.
local = 58.

_getgroups:
	mov	r5,-(sp)
	mov	sp,r5
	mov	4(r5),0f
	mov	6(r5),0f+2
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc

.data
9:
	sys	getgroups; 0:..; ..
