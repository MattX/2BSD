/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)kern_acct.c	2.1 (2.11BSD) 3/10/93
 */

#include "param.h"
#include "systm.h"
#include "fs.h"
#include "dir.h"
#include "inode.h"
#include "user.h"
#include "proc.h"
#include "acct.h"
#include "kernel.h"
#include "syslog.h"

/*
 * SHOULD REPLACE THIS WITH A DRIVER THAT CAN BE READ TO SIMPLIFY.
 */
int	acctsuspend = 2;	/* stop accounting when < 2% free space left */
int	acctresume = 4;		/* resume when free space risen to > 4% */
struct	timeval chk = {15, 0};	/* frequency to check space for accounting */
struct	inode *acctp;
struct	inode *savacctp;

/*
 * Perform process accounting functions.
 */
sysacct()
{
	register struct inode *ip;
	register struct a {
		char	*fname;
	} *uap = (struct a *)u.u_ap;
	register struct nameidata *ndp = &u.u_nd;
	int acctwatch();

	if (suser()) {
		if (savacctp) {
			acctp = savacctp;
			savacctp = NULL;
		}
		if (uap->fname==NULL) {
			if (ip = acctp) {
				irele(ip);
				acctp = NULL;
				chk.tv_usec = 0;
				untimeout(acctwatch, &chk);
			}
			return;
		}
		ndp->ni_nameiop = LOOKUP | FOLLOW;
		ndp->ni_segflg = UIO_USERSPACE;
		ndp->ni_dirp = uap->fname;
		ip = namei(ndp);
		if (ip == NULL)
			return;
		if ((ip->i_mode&IFMT) != IFREG) {
			u.u_error = EACCES;
			iput(ip);
			return;
		}
		if (ip->i_fs->fs_ronly) {
			u.u_error = EROFS;
			iput(ip);
			return;
		}
		if (acctp && (acctp->i_number != ip->i_number ||
		    acctp->i_dev != ip->i_dev))
			irele(acctp);
		acctp = ip;
		iunlock(ip);
		if (chk.tv_usec == 0) {
			chk.tv_usec = 1;	/* usec is timer enabled flag */
			timeout(acctwatch, &chk, chk.tv_sec * hz);
		}
	}
}

acctwatch(resettime)
	register struct	timeval *resettime;
{
	register struct fs *fs;

	if (savacctp) {
		fs = savacctp->i_fs;
		if (freespace(fs, acctresume) > 0) {
			acctp = savacctp;
			savacctp = NULL;
			log(LOG_NOTICE, "Accounting resumed\n");
/*			return;		/* XXX - fall thru and refresh timer */
		}
	}
	if (acctp == NULL)
		return;		/* do not refresh timer */
	fs = acctp->i_fs;
	if (freespace(fs, acctsuspend) <= 0) {
		savacctp = acctp;
		acctp = NULL;
		log(LOG_NOTICE, "Accounting suspended\n");
	}
	timeout(acctwatch, resettime, resettime->tv_sec * hz);
}

/*
 * On exit, write a record on the accounting file.
 */
acct()
{
	struct	acct acctbuf;
	register struct inode *ip;
	off_t siz;
	register struct acct *ap = &acctbuf;

	if ((ip = acctp) == NULL)
		return;
	ilock(ip);
	bcopy(u.u_comm, ap->ac_comm, sizeof(acctbuf.ac_comm));
	ap->ac_utime = compress(u.u_ru.ru_utime);
	ap->ac_stime = compress(u.u_ru.ru_stime);
	ap->ac_etime = compress(time.tv_sec - u.u_start);
	ap->ac_btime = u.u_start;
	ap->ac_uid = u.u_ruid;
	ap->ac_gid = u.u_rgid;
	ap->ac_mem = (u.u_dsize+u.u_ssize) / 16; /* fast ctok() */
	ap->ac_io = compress(u.u_ru.ru_inblock + u.u_ru.ru_oublock);
	if (u.u_ttyp)
		ap->ac_tty = u.u_ttyd;
	else
		ap->ac_tty = NODEV;
	ap->ac_flag = u.u_acflag;
	siz = ip->i_size;
	u.u_error = rdwri(UIO_WRITE, ip, ap, sizeof(acctbuf), siz,
			UIO_SYSSPACE, (int *)0);
	if (u.u_error)
		itrunc(ip, (u_long)siz);
	iunlock(ip);
}

/*
 * Produce a pseudo-floating point representation
 * with 3 bits base-8 exponent, 13 bits fraction.
 */
compress(t)
register time_t t;
{
	register exp = 0, round = 0;

	while (t >= 8192) {
		exp++;
		round = t&04L;
		t >>= 3;
	}
	if (round) {
		t++;
		if (t >= 8192) {
			t >>= 3;
			exp++;
		}
	}
	return((exp<<13) + t);
}
