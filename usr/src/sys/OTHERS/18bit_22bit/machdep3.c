/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)machdep3.c	1.1 (2.10BSD Berkeley) 12/1/86
 *	jpn added Matt Jacobs code
 */

#include "jpn.h"
#include "param.h"
#include "../machine/seg.h"
#include "../machine/iopage.h"

#include "inode.h"
#include "time.h"
#include "resource.h"
#include "proc.h"
#include "fs.h"
#include "map.h"
#include "buf.h"
#include "text.h"
#include "file.h"
#include "clist.h"
#include "uba.h"
#include "callout.h"
#include "reboot.h"
#include "errno.h"
#include "systm.h"
#include "ram.h"

#ifdef	SOFUB_MAP

/*
 * sofmap -	implement soft unibus map
 *		for 22-bit q-bus systems
 *		generally only used by the tape
 *		driver (for tar and dump)
 *
 *		matt jacob
 *		University of California at Davis
 *		22-Nov-84
 *		Available to all Unix Licence holders
 *
 */

#define	QMEG	((long)(256L*1024L))
#define	exad(x,y)	((long)((long)x<<16|(unsigned)y))
#define	B_UBUF		exad(bp->b_xmem,(u_int)bp->b_un.b_addr)
#define	E_UBUF		(B_UBUF + (long) bp->b_bcount)
size_t	sofub_addr;
size_t	sofub_off;
memaddr	sofub_base;
unsigned sofub_size;
int	sofub_cnt;

#ifdef SOFUB_DEBUG
int bpxm;
int bpadd;
#endif

/*
 * Sofub_alloc - allocate usage of soft unibus map
 *		 only one at a time for now...
 *
 *	called from strategy routine of pertinent
 *	device driver
 *
 *	returns 0 if not okay, else returns 1.
 *	expects a buffer pointer as an argument
 *	expects all addresses in bp already validated (by physio)
 *	sets appropriate bits and calls iodone if error
 *
 */

int
Sofub_alloc(bp)
register struct buf *bp;
{
	register int s;
	register int count;
	size_t uaddr;
	memaddr sbase;
	extern size_t sofub_addr, sofub_off;
	extern memaddr sofub_base;
	extern unsigned sofub_size;
	extern int sofub_cnt;


	if(E_UBUF < QMEG)
		return(1);	/* okay, < 256kb	*/
	else if(bp->b_bcount > sofub_size)
	{
		uprintf("I/O > 256kb on device (%d,%d)\n\
		    Base = %ld, End = %ld\nSee System Manager.\n"
		    ,major(bp->b_dev),minor(bp->b_dev),B_UBUF,E_UBUF);
		bp->b_flags |= B_ERROR;
		bp->b_error = EFAULT;
		iodone(bp);
		return(0);
	}

	/*
	 * if map is busy, sleep on it...
	 *
	 */

	while(sofub_cnt != 0)
	{
#ifdef SOFUB_DEBUG
		printf("sleeping on sofub_cnt\n");
#endif
		sleep((caddr_t) &sofub_cnt,PSWP+2);
	}

	/*
	 * now grab it...
	 *
	 */

	sofub_cnt++;

	/*
	 * now calculate virtual address of user buffer...
	 *
	 */

	sofub_off = (size_t)((u_int)bp->b_un.b_addr & 077);
	sofub_addr = (size_t)(((u_int)bp->b_un.b_addr>>6)&01777) | (((int) bp->b_xmem) << 10);

#ifdef SOFUB_DEBUG
	bpadd = (u_int)bp->b_un.b_addr; bpxm = bp->b_xmem;
#endif

	/*
	 * if this is a write, we have to fetch data from user buffer
	 * first
	 *
	 */

	if((bp->b_flags & B_READ) == 0)
	{
		count = bp->b_bcount;
		uaddr = sofub_addr;
		sbase = sofub_base;

		/*
		 * first, copy all 8kb-1click segments..
		 *
		 */

		s = spl5();

		while(count > (8192-64))
		{
			copyv(uaddr,sofub_off,sbase,0,(8192-64));
			count -= (8192-64);
			uaddr += 0177;	/* add 8192-64 bytes */
			sbase += 0177;
		}

		/*
		 * copy last residual segment
		 *
		 */

		copyv(uaddr,sofub_off,sbase,0,count);

		splx(s);
	}

	/*
	 * put in new 18 bit address
	 * 
	 */

	bp->b_un.b_addr = (caddr_t)ctob((long)sofub_base);

	/*
	 * don't turn sofub_base to clicks here
	 * because half the work is done by
	 * having it in click form here, i.e.,
	 * bp->b_xmem would equal
	 * ctob(x)>>16 (hi six bits of 18)
	 *
	 */

	bp->b_xmem = (sofub_base >> 10)&3;


	/*
	 * and return with a good value
	 *
	 */

	return(1);
}

/*
 * Sofub_relse	-	release sofub_map
 *
 *	passed a buffer pointer
 *	and a transfer byte count...
 *	(for use if was a read)
 *
 *	note, we are called at interrupt level...
 *	so we save mapping context..
 *
 */

Sofub_relse(bp,count)
register struct buf *bp;
register unsigned count;
{
	register int s;
	size_t uaddr;
	memaddr sbase;
	mapinfo map;
	extern size_t sofub_addr, sofub_off;
	extern memaddr sofub_base;
	extern unsigned sofub_size;
	extern int sofub_cnt;

	/*
	 * It is not clear that I have to save mapping here,
	 * but it is probably just as well to do so, and to
	 * conform to the 2.9 kernel interrupt protocol....
	 *
	 */

	savemap(map);

	if(sofub_cnt == 0)
	{
		restormap(map);
		return;	/* not being used	*/
	}
	else if(bp->b_flags & B_READ)
	{
		uaddr = sofub_addr;
		sbase = sofub_base;

		/*
		 * first, copy all 8kb-1click segments..
		 *
		 */

		s = spl5();
		while(count > (8192-64))
		{
			copyv(sbase,0,uaddr,sofub_off,(8192-64));
			count -= (8192-64);
			uaddr += 0177;	/* add 8192-64 bytes */
			sbase += 0177;
		}

		/*
		 * copy last residual segment
		 *
		 */

		copyv(sbase,0,uaddr,sofub_off,count);

		splx(s);
	}

	bp->b_un.b_addr = (caddr_t)((sofub_addr << 6) | sofub_off);
	bp->b_xmem = ((u_int)sofub_addr >> 10) & 077;

#ifdef SOFUB_DEBUG
	if(bp->b_un.b_addr != bpadd || bp->b_xmem != bpxm)
	{
		printf("mismatch: new bp=(%o,%o), old bp=(%o,%o)\n",
			bp->b_un.b_addr,(int)bp->b_xmem,bpadd,bpxm);
		bp->b_flags |= B_ERROR;
		bp->b_error = EFAULT;
	}
#endif
	/*
	 * decrement reference count to map
	 *
	 */

	sofub_cnt = MAX(0,sofub_cnt-1);

	/*
	 * and wake up anyone waiting on it....
	 *
	 */

	if(sofub_cnt > 0)
	{
		printf("waking up on sofub_cnt\n");
		wakeup((caddr_t) &sofub_cnt);
	}

	restormap(map);
}

/*
 * Sofub_init - initialize soft unibus map
 *
 *		return # of clicks consumed....
 *
 */


int
Sofub_init()
{
	extern size_t sofub_addr, sofub_off;
	extern memaddr sofub_base;
	extern unsigned sofub_size;
	extern int sofub_cnt;

	/* allocate a a 10 kb buffer for 18-bit raw xfers on q-bus	*/

#define B	10240+64
/* #define B	5120+64 */

	sofub_cnt = 0;
	sofub_size = (unsigned) B;

	if ((sofub_base = malloc(coremap, btoc(B))) == 0)
		panic("Sofmap: no mem");
	else if(((sofub_base+btoc(B))>>10) > 3)	/* > 256kb!	*/
		panic("Sofmap: > 256kb");
	else
	{
		sofub_addr = sofub_base;	/* for safety...	*/
		sofub_off = (sofub_base>>10)&3;
		return(btoc(B));
	}

#undef	B
}
#endif SOFUB_MAP
