/ _exit(status);

.globl	__exit
exit	= 1.

__exit:
	mov	r5,-(sp)
	mov	sp,r5
	mov	4(r5),r0
	sys	exit
	/*NOTREACHED*/
