/*
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)net_xxx.s	1.1 (2.10BSD Berkeley) 4/3/88
 */

#include "DEFS.h"
#include "../machine/mch_iopage.h"


/*
 * badaddr(addr, len)
 *	caddr_t addr;
 *	int len;
 *
 * See if accessing addr with a len type instruction causes a memory fault.
 * Len is length os access (1=byte, 2=short, 4=long).  Returns 0 if the
 * address is OK, -1 on error.  if the address is odd, and make sure that
 * the address is valid.
 */
ENTRY(badaddr)
	mov	PS,-(sp)		/ save current PS and set previous
	bic	$30000,PS		/   mode = kernel
	mfpd	*$nofault		/ save current nofault and set up
	mov	$4f,-(sp)		/   our own trap
	mtpd	*$nofault
	cmp	10(sp),$1		/ len == 1??
	beq	1f
	tstb	*6(sp)			/ yes, just do a tstb on the address
	br	2f
1:
	tst	*6(sp)			/ no, try a tst ...
2:
	clr	r0			/ we made it - return success
3:
	mtpd	*$nofault		/ restore previous fault trap
	mov	(sp)+,PS		/   and PS
	rts	pc			/ and return
4:
	mov	$-1,r0			/ we faulted out - return failure
	br	3b


/*
 * locc(mask, size, str)
 * 	u_char mask;
 * 	u_int size;
 * 	u_char *str;
 *
 * Scan through str up to (but not including str[size]) stopping when a
 * character equals mask.  Return number of characters left in str.
 */
ENTRY(locc)
	mov	4(sp),r0		/ r0 = size
	beq	3f			/   exit early if zero
	mov	6(sp),r1		/ r1 = str
	mov	r2,-(sp)		/ r2 = mask
	mov	2+2(sp),r2
1:					/ do
	cmpb	(r1)+,r2		/   if (*str++ == mask)
	beq	2f
	sob	r0,1b			/ while (--size != 0)
2:
	mov	(sp)+,r2		/ restore registers
3:
	rts	pc			/ and return size
