/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)kern_acct.c	2.5 (2.11BSD) 1997/2/16
 *
 * This module is a real mishmash of FreeBSD, 4.3BSD, and home brewed code.
 */

#include "param.h"
#include "systm.h"
#include "fs.h"
#include "dir.h"
#include "inode.h"
#include "user.h"
#include "namei.h"
#include "proc.h"
#include <sys/file.h>
#include "acct.h"
#include "kernel.h"
#include "syslog.h"

/*
 * SHOULD REPLACE THIS WITH A DRIVER THAT CAN BE READ TO SIMPLIFY.
 */
short	acctsuspend = 2;	/* stop accounting when < 2% free space left */
short	acctresume = 4;		/* resume when free space risen to > 4% */
short	acctchkfreq = 15;	/* frequency to check space for accounting */
short	acctdisabled = 0;	/* 0 = not disabled */
struct	inode *acctp;
comp_t	compress();
static	int	chkfreesp();

/*
 * Perform process accounting functions.
 */
sysacct()
	{
	register struct inode *ip = NULL;
	register struct a {
		char	*fname;
	} *uap = (struct a *)u.u_ap;
	struct	nameidata nd;
	register struct nameidata *ndp = &nd;
	int	error;

	if	(!suser())
		{
		error = u.u_error;	/* XXX */
		goto out;
		}
/*
 * If accounting is to be started to a file, "open" that file for
 * writing.  We don't check that the file is 'normal' because while it may
 * be strange to write to a tape or (unmounted) disk why should it be
 * prohibited?
*/
	if	(uap->fname != NULL)
		{
		NDINIT(ndp, LOOKUP, FOLLOW, UIO_USERSPACE, uap->fname);
		if	((error = vn_open(ndp, FFLAGS(O_WRONLY), 0)) != 0)
			goto	out;
		ip = ndp->ni_ip;
		}
/*
 * Swap the accounting files.
*/
	error = swapacctf(ip);

out:
	return(u.u_error = error);
	}

/*
 * This was broken out into a function of its own so that it could be 
 * called from elsewhere in the kernel.  The experiment that was done for
 * didn't work out but it doesn't hurt anything to retain this function 
 * (it might come in handy in the future).
*/
swapacctf(ip)
	register struct inode *ip;
	{
	register struct inode *oacctp;

	oacctp = acctp;
	acctp = ip;
	if	(oacctp)
		(void)vn_close(oacctp, FWRITE);
	if	(acctp)
		acctwatch();
	return(0);
	}

acctwatch()
	{
	register struct fs *fs;
	static	time_t	acctchecktime;

	if	(acctp == NULL || time.tv_sec < acctchecktime)
		return;		/* do not refresh timer */
	acctchecktime = time.tv_sec + acctchkfreq;
	fs = acctp->i_fs;

	if	(acctdisabled)
		{
		if	(chkfreesp(fs, acctresume) > 0)
			{
			acctdisabled = 0;
			log(LOG_NOTICE, "Accounting resumed\n");
			}
		}
	else
		{
		if	(chkfreesp(fs, acctsuspend) <= 0)
			{
			log(LOG_NOTICE, "Accounting suspended\n");
			acctdisabled = 1;
			}
		}
	}

/*
 * On exit, write a record on the accounting file.
 */
acct()
	{
	struct	acct acctbuf;
	register struct inode *ip;
	register struct acct *ap = &acctbuf;

	acctwatch();

	if	((ip = acctp) == NULL || acctdisabled)
		return;
	ilock(ip);
	bcopy(u.u_comm, ap->ac_comm, sizeof(acctbuf.ac_comm));
/*
 * The 'user' and 'system' times need to be converted from 'hz' (linefrequency)
 * clockticks to the AHZ pseudo-tick unit of measure.  The elapsed time is
 * converted from seconds to AHZ ticks.
*/
	ap->ac_utime = compress(((u_long)u.u_ru.ru_utime * AHZ) / hz);
	ap->ac_stime = compress(((u_long)u.u_ru.ru_stime * AHZ) / hz);
	ap->ac_etime = compress((u_long)(time.tv_sec - u.u_start) * AHZ);
	ap->ac_btime = u.u_start;
	ap->ac_uid = u.u_ruid;
	ap->ac_gid = u.u_rgid;
	ap->ac_mem = (u.u_dsize+u.u_ssize) / 16; /* fast ctok() */
/*
 * Section 3.9 of the 4.3BSD book says that I/O is measured in 1/AHZ units too.
*/
	ap->ac_io = compress((u_long)(u.u_ru.ru_inblock+u.u_ru.ru_oublock)*AHZ);
	if	(u.u_ttyp)
		ap->ac_tty = u.u_ttyd;
	else
		ap->ac_tty = NODEV;
	ap->ac_flag = u.u_acflag;
	u.u_error = rdwri(UIO_WRITE, ip, ap, sizeof(acctbuf), ip->i_size,
			UIO_SYSSPACE, IO_UNIT|IO_APPEND, (int *)0);
	if	(u.u_error)
		{
/*
 * The only time this should happen is when a physical error occurs on the
 *  disk drive or the space is exhausted.  The diagnostic message is not
 * enabled by default to save space and also because there's apparently a
 * race condition during 'reboot'/'fastboot' that would elicit the (harmless
 * I hope) warning message.
*/
		acctdisabled = 1;
#ifdef	DIAGNOSTIC
		log(LOG_NOTICE, "acct rdwri=%d\n", u.u_error);
#endif
		}
	iunlock(ip);
	}

/*
 * Raise exponent and drop bits off the right of the mantissa until the
 * mantissa fits.  If we run out of exponent space, return max value (all
 * one bits).  With AHZ set to 64 this is good for close to 8.5 years:
 * (8191 * (1 << (3*7)) / 64 / 60 / 60 / 24 / 365 ~= 8.5)
 */

#define	MANTSIZE	13			/* 13 bit mantissa. */
#define	EXPSIZE		3			/* Base 8 (3 bit) exponent. */

comp_t
compress(mant)
	u_long mant;
	{
	register int exp;

	for	(exp = 0; exp < (1 << EXPSIZE); exp++, mant >>= EXPSIZE)
		if	(mant < (1L << MANTSIZE))
			return(mant | (exp << MANTSIZE));
	return(~0);
	}

/*
 * A helper function since freespace's generated code is so voluminous.  All
 * we really want is an indication if there is the desired amount of space
 * available (greater than or equal, less than zero).
*/
static int
chkfreesp(fs, percent)
	register struct fs *fs;
	int	percent;
	{
	daddr_t	l;

	l = freespace(fs, percent);
	if	(l < 0)
		return(-1);
	else if	(l == 0)
		return(0);
	return(1);
	}
