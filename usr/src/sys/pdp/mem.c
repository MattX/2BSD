/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)mem.c	1.1 (2.10BSD Berkeley) 12/1/86
 */

#include "param.h"
#include "../machine/seg.h"

#include "user.h"
#include "conf.h"
#include "uio.h"
#include "hk.h"
#include "xp.h"

mmread(dev, uio)
	dev_t dev;
	struct uio *uio;
{

	return (mmrw(dev, uio, UIO_READ));
}

mmwrite(dev, uio)
	dev_t dev;
	struct uio *uio;
{

	return (mmrw(dev, uio, UIO_WRITE));
}

/*
 * This routine is callable only from the high
 * kernel as it assumes normal mapping and doesn't
 * bother to save R5.
 */
mmrw(dev, uio, rw)
	dev_t dev;
	register struct uio *uio;
	enum uio_rw rw;
{
	register struct iovec *iov;
	int error = 0;
register u_int c;
	u_int on;

	while (uio->uio_resid && error == 0) {
		iov = uio->uio_iov;
		if (iov->iov_len == 0) {
			uio->uio_iov++;
			uio->uio_iovcnt--;
			if (uio->uio_iovcnt < 0)
				panic("mmrw");
			continue;
		}
		switch (minor(dev)) {

/* minor device 0 is physical memory */
		case 0:
			mapseg5((memaddr)(uio->uio_offset>>6),
				   ((btoc(8192)-1)<<8)|RW);
			on = uio->uio_offset & 077L;
			c = MIN(iov->iov_len, 8192 - on);
			error = uiomove(SEG5+on, c, rw, uio);
			normalseg5();
			continue;
/* minor device 1 is kernel memory */
		case 1:
			error = uiomove((caddr_t)uio->uio_offset, iov->iov_len, rw, uio);
			continue;
/* minor device 2 is EOF/RATHOLE */
		case 2:
			if (rw == UIO_READ)
				return(0);
			c = iov->iov_len;
			break;
		}
		if (error)
			break;
		iov->iov_base += c;
		iov->iov_len -= c;
		uio->uio_offset += c;
		uio->uio_resid -= c;
	}
	return(error);
}

#if NHK > 0 || NXPD > 0
/*
 * Internal versions of mmread(), mmwrite()
 * used by disk driver ecc routines.
 */
getmemc(addr)
	long addr;
{
	register int a, c, d;

	/*
	 * bn = addr >> 6
	 * on = addr & 077
	 */
	a = UISA[0];
	d = UISD[0];
	UISA[0] = addr >> 6;
	UISD[0] = RO;			/* one click, read only */
	c = fuibyte((caddr_t)(addr & 077));
	UISA[0] = a;
	UISD[0] = d;
	return(c);
}

putmemc(addr,contents)
	long addr;
	int contents;
{
	register int a, d;

	/*
	 * bn = addr >> 6
	 * on = addr & 077
	 */
	a = UISA[0];
	d = UISD[0];
	UISA[0] = addr >> 6;
	UISD[0] = RW;			/* one click, read/write */
	suibyte((caddr_t)(addr & 077), contents);
	UISA[0] = a;
	UISD[0] = d;
}
#endif
