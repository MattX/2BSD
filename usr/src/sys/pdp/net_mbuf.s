/*
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)net_mbuf.s	1.1 (2.10BSD Berkeley) 4/10/88
 */

#include "DEFS.h"
#include "../machine/mch_iopage.h"


/*
 * mbcopyin(click, off, ptr, len)
 *	u_short click;		/ address of external DMA buffer
 *	u_short off;		/ offset into click of start of buffer
 *	caddr_t ptr;		/ address of internal buffer
 *	short len;		/ bytes to copy from click/off to ptr
 *
 * mbcopyout(ptr, click, off, len)
 *	u_short click;		/ address of external DMA buffer
 *	u_short off;		/ offset into click of start of buffer
 *	caddr_t ptr;		/ address of internal buffer
 *	short len;		/ bytes to copy from ptr to click/off
 *
 * Used for copying data from/to the UNIBUS DMA pages to/from the
 * in-address-space mbufs.  Cannot copy data to or from the stack.  Len
 * cannot exceed 8Kbytes.
 *
 * We remap SDSA6 to do the copy and run at SPL7.
 */

ENTRY(mbcopyin)
	jsr	r5,csv
	mov	012(r5),r2		/ grab byte count
	ble	mbdone			/ nothing to do if len <= 0 ...
	cmp	r2,$8192.		/ too big?
	bge	mbbad			/   yes, panic

	mov	06(r5),r0		/ grab offset and computer pointer
	add	$140000,r0		/   to mapped in source data
	mov	010(r5),r1		/ grab ptr (destination address)
	mov	PS,-(sp)		/ save PS since we lock out interrupts
	SPL7
	mov	SDSA6,r3		/ save mapping for stack / u. area
	mov	04(r5),SDSA6		/ map in source data
	br	mbloop

ENTRY(mbcopyout)
	jsr	r5,csv
	mov	012(r5),r2		/ grab byte count
	ble	mbdone			/ nothing to do if len <= 0 ...
	cmp	r2,$8192.		/ too big?
	bge	mbbad			/   yes, panic

	mov	04(r5),r0		/ grab ptr (source address)
	mov	010(r5),r1		/ grab offset and compute pointer
	add	$140000,r1		/   to mapped destination address
	mov	PS,-(sp)		/ save PS since we lock out interrupts
	SPL7
	mov	SDSA6,r3		/ save mapping for stack / u. area
	mov	06(r5),SDSA6		/ map in destination address data


/*
 * Common loop and return for mbcopyin and mbcopyout.  Copy from (r0) to
 * (r1) for r2 bytes.  R3 contains old seg6 address descriptor.  The copy
 * loop should probably be rewritten to do something like bcopy ...
 */
mbloop:
	movb	(r0)+,(r1)+		/ copy one byte at a time to avoid
	sob	r2,mbloop		/   allignment problems.

	mov	r3,SDSA6		/ map back in stack / u. area
	mov	(sp)+,PS		/ restore original priority
mbdone:
	jmp	cret
mbbad:
	halt				/ causes a panic trap from supervisor
					/   mode
