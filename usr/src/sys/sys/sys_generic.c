/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)sys_generic.c	1.6 (2.11BSD GTE) 1997/2/14
 */

#include "param.h"
#include "../machine/seg.h"

#include "user.h"
#include "proc.h"
#include "inode.h"
#include "file.h"
#include "ioctl.h"
#include "conf.h"
#include "uio.h"
#include "pty.h"
#include "kernel.h"
#include "systm.h"

/* 
 * this is consolidated here rather than being scattered all over the
 * place.  the socketops table has to be in kernel space, but since
 * networking might not be defined an appropriate error has to be set
*/

	int	sorw(), soctl(), sosel(), socls();
	struct	fileops	socketops =
		{ sorw, soctl, sosel, socls };
extern	struct	fileops	inodeops, pipeops;
	struct	fileops	*Fops[] = { NULL, &inodeops, &socketops, &pipeops };

/*
 * Read system call.
 */
read()
{
	register struct a {
		int	fdes;
		char	*cbuf;
		unsigned count;
	} *uap = (struct a *)u.u_ap;
	struct uio auio;
	struct iovec aiov;

	aiov.iov_base = (caddr_t)uap->cbuf;
	aiov.iov_len = uap->count;
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_rw = UIO_READ;
	rwuio(&auio);
}

readv()
{
	register struct a {
		int	fdes;
		struct	iovec *iovp;
		unsigned iovcnt;
	} *uap = (struct a *)u.u_ap;
	struct uio auio;
	struct iovec aiov[16];		/* XXX */

	if (uap->iovcnt > sizeof(aiov)/sizeof(aiov[0])) {
		u.u_error = EINVAL;
		return;
	}
	auio.uio_iov = aiov;
	auio.uio_iovcnt = uap->iovcnt;
	auio.uio_rw = UIO_READ;
	u.u_error = copyin((caddr_t)uap->iovp, (caddr_t)aiov,
	    uap->iovcnt * sizeof (struct iovec));
	if (u.u_error)
		return;
	rwuio(&auio);
}

/*
 * Write system call
 */
write()
{
	register struct a {
		int	fdes;
		char	*cbuf;
		unsigned count;
	} *uap = (struct a *)u.u_ap;
	struct uio auio;
	struct iovec aiov;

	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_rw = UIO_WRITE;
	aiov.iov_base = uap->cbuf;
	aiov.iov_len = uap->count;
	rwuio(&auio);
}

writev()
{
	register struct a {
		int	fdes;
		struct	iovec *iovp;
		unsigned iovcnt;
	} *uap = (struct a *)u.u_ap;
	struct uio auio;
	struct iovec aiov[16];		/* XXX */

	if (uap->iovcnt > sizeof(aiov)/sizeof(aiov[0])) {
		u.u_error = EINVAL;
		return;
	}
	auio.uio_iov = aiov;
	auio.uio_iovcnt = uap->iovcnt;
	auio.uio_rw = UIO_WRITE;
	u.u_error = copyin((caddr_t)uap->iovp, (caddr_t)aiov,
	    uap->iovcnt * sizeof (struct iovec));
	if (u.u_error)
		return;
	rwuio(&auio);
}

static
rwuio(uio)
	register struct uio *uio;
{
	struct a {
		int	fdes;
	};
	register struct file *fp;
	register struct iovec *iov;
	u_int i, count;
	off_t	total;

	GETF(fp, ((struct a *)u.u_ap)->fdes);
	if ((fp->f_flag & (uio->uio_rw == UIO_READ ? FREAD : FWRITE)) == 0) {
		u.u_error = EBADF;
		return;
	}
	total =(off_t)0;
	uio->uio_resid = 0;
	uio->uio_segflg = UIO_USERSPACE;
	iov = uio->uio_iov;
	for (i = 0; i < uio->uio_iovcnt; i++) {
#ifdef	pdp11
		total += iov->iov_len;
#else
		if (iov->iov_len < 0) {
			u.u_error = EINVAL;
			return;
		}
		uio->uio_resid += iov->iov_len;
		if (uio->uio_resid < 0) {
			u.u_error = EINVAL;
			return;
		}
#endif
		iov++;
	}
#ifdef	pdp11
	uio->uio_resid = total;
	if (uio->uio_resid != total) {	/* check wraparound */
		u.u_error = EINVAL;
		return;
	}
#endif
	count = uio->uio_resid;
	if (setjmp(&u.u_qsave)) {
		if (uio->uio_resid == count) {
			if ((u.u_sigintr & sigmask(u.u_procp->p_cursig)) != 0)
				u.u_error = EINTR;
			else
				u.u_eosys = RESTARTSYS;
		}
	} else
		u.u_error = (*Fops[fp->f_type]->fo_rw)(fp, uio);
	u.u_r.r_val1 = count - uio->uio_resid;
}

/*
 * Ioctl system call
 */
ioctl()
{
	register struct file *fp;
	register struct a {
		int	fdes;
		long	cmd;
		caddr_t	cmarg;
	} *uap;
	long com;
	u_int k_com;
	register u_int size;
	char data[IOCPARM_MASK+1];

	uap = (struct a *)u.u_ap;
	if ((fp = getf(uap->fdes)) == NULL)
		return;
	if ((fp->f_flag & (FREAD|FWRITE)) == 0) {
		u.u_error = EBADF;
		return;
	}
	com = uap->cmd;

	/* THE 2.11 KERNEL STILL THINKS THAT IOCTL COMMANDS ARE 16 BITS */
	k_com = (u_int)com;

	if (k_com == FIOCLEX) {
		u.u_pofile[uap->fdes] |= UF_EXCLOSE;
		return;
	}
	if (k_com == FIONCLEX) {
		u.u_pofile[uap->fdes] &= ~UF_EXCLOSE;
		return;
	}

	/*
	 * Interpret high order word to find
	 * amount of data to be copied to/from the
	 * user's address space.
	 */
	size = (com &~ (IOC_INOUT|IOC_VOID)) >> 16;
	if (size > sizeof (data)) {
		u.u_error = EFAULT;
		return;
	}
	if (com&IOC_IN) {
		if (size) {
			if (((u_int)uap->cmarg|size)&1)
			    u.u_error =
				vcopyin(uap->cmarg, (caddr_t)data, size);
			else
			    u.u_error =
				copyin(uap->cmarg, (caddr_t)data, size);
			if (u.u_error)
				return;
		} else
			*(caddr_t *)data = uap->cmarg;
	} else if ((com&IOC_OUT) && size)
		/*
		 * Zero the buffer on the stack so the user
		 * always gets back something deterministic.
		 */
		bzero((caddr_t)data, size);
	else if (com&IOC_VOID)
		*(caddr_t *)data = uap->cmarg;

	switch (k_com) {

	case FIONBIO:
		u.u_error = fset(fp, FNONBLOCK, *(int *)data);
		return;

	case FIOASYNC:
		u.u_error = fset(fp, FASYNC, *(int *)data);
		return;

	case FIOSETOWN:
		u.u_error = fsetown(fp, *(int *)data);
		return;

	case FIOGETOWN:
		u.u_error = fgetown(fp, (int *)data);
		return;
	}
	u.u_error = (*Fops[fp->f_type]->fo_ioctl)(fp, k_com, data);
	/*
	 * Copy any data to user, size was
	 * already set and checked above.
	 */
	if (u.u_error == 0 && (com&IOC_OUT) && size)
		if (((u_int)uap->cmarg|size)&1)
			u.u_error = vcopyout(data, uap->cmarg, size);
		else
			u.u_error = copyout(data, uap->cmarg, size);
}

int	unselect();
int	nselcoll;

/*
 * Select system call.
 */
select()
{
	register struct uap  {
		int	nd;
		fd_set	*in, *ou, *ex;
		struct	timeval *tv;
	} *uap = (struct uap *)u.u_ap;
	fd_set ibits[3], obits[3];
	struct timeval atv;
	register int s, ni;
	int ncoll;
	label_t lqsave;

	bzero((caddr_t)ibits, sizeof(ibits));
	bzero((caddr_t)obits, sizeof(obits));
	if (uap->nd > NOFILE)
		uap->nd = NOFILE;	/* forgiving, if slightly wrong */
	ni = howmany(uap->nd, NFDBITS);

#define	getbits(name, x) \
	if (uap->name) { \
		u.u_error = copyin((caddr_t)uap->name, (caddr_t)&ibits[x], \
		    (unsigned)(ni * sizeof(fd_mask))); \
		if (u.u_error) \
			goto done; \
	}
	getbits(in, 0);
	getbits(ou, 1);
	getbits(ex, 2);
#undef	getbits

	if (uap->tv) {
		u.u_error = copyin((caddr_t)uap->tv, (caddr_t)&atv,
			sizeof (atv));
		if (u.u_error)
			goto done;
		if (itimerfix(&atv)) {
			u.u_error = EINVAL;
			goto done;
		}
		s = splhigh();
		time.tv_usec = lbolt * mshz;
		timevaladd(&atv, &time);
		splx(s);
	}
retry:
	ncoll = nselcoll;
	u.u_procp->p_flag |= SSEL;
	u.u_r.r_val1 = selscan(ibits, obits, uap->nd);
	if (u.u_error || u.u_r.r_val1)
		goto done;
	s = splhigh();
	/* this should be timercmp(&time, &atv, >=) */
	if (uap->tv && (time.tv_sec > atv.tv_sec || (time.tv_sec == atv.tv_sec
	    && lbolt * mshz >= atv.tv_usec))) {
		splx(s);
		goto done;
	}
	if ((u.u_procp->p_flag & SSEL) == 0 || nselcoll != ncoll) {
		u.u_procp->p_flag &= ~SSEL;
		splx(s);
		goto retry;
	}
	u.u_procp->p_flag &= ~SSEL;
	if (uap->tv) {
		lqsave = u.u_qsave;
		if (setjmp(&u.u_qsave)) {
			untimeout(unselect, (caddr_t)u.u_procp);
			u.u_error = EINTR;
			splx(s);
			goto done;
		}
		timeout(unselect, (caddr_t)u.u_procp, hzto(&atv));
	}
	sleep((caddr_t)&selwait, PZERO+1);
	if (uap->tv) {
		u.u_qsave = lqsave;
		untimeout(unselect, (caddr_t)u.u_procp);
	}
	splx(s);
	goto retry;
done:
#define	putbits(name, x) \
	if (uap->name) { \
		int error = copyout((caddr_t)&obits[x], (caddr_t)uap->name, \
		    (unsigned)(ni * sizeof(fd_mask))); \
		if (error) \
			u.u_error = error; \
	}
	if (u.u_error == 0) {
		putbits(in, 0);
		putbits(ou, 1);
		putbits(ex, 2);
#undef putbits
	}
}

unselect(p)
	register struct proc *p;
{
	register int s = splhigh();

	switch (p->p_stat) {

	case SSLEEP:
		setrun(p);
		break;

	case SSTOP:
		unsleep(p);
		break;
	}
	splx(s);
}

selscan(ibits, obits, nfd)
	fd_set *ibits, *obits;
	int nfd;
{
	register int i, j;
	fd_mask bits;
	register int which, flag;
	struct file *fp;
	int n = 0;

	for (which = 0; which < 3; which++) {
		switch (which) {

		case 0:
			flag = FREAD; break;

		case 1:
			flag = FWRITE; break;

		case 2:
			flag = 0; break;
		}
		for (i = 0; i < nfd; i += NFDBITS) {
			bits = ibits[which].fds_bits[i/NFDBITS];
			while ((j = ffs(bits)) && i + --j < nfd) {
				bits &= ~(1L << j);
				fp = u.u_ofile[i + j];
				if (fp == NULL) {
					u.u_error = EBADF;
					break;
				}
				if ((*Fops[fp->f_type]->fo_select)(fp,flag)) {
					FD_SET(i + j, &obits[which]);
					n++;
				}
			}
		}
	}
	return (n);
}

/*ARGSUSED*/
seltrue(dev, flag)
	dev_t dev;
	int flag;
{

	return (1);
}

selwakeup(p, coll)
	register struct proc *p;
	long coll;
{
	mapinfo map;

	savemap(map);
	if (coll) {
		nselcoll++;
		wakeup((caddr_t)&selwait);
	}
	if (p) {
		register int s = splhigh();
		if (p->p_wchan == (caddr_t)&selwait) {
			if (p->p_stat == SSLEEP)
				setrun(p);
			else
				unsleep(p);
		} else if (p->p_flag & SSEL)
			p->p_flag &= ~SSEL;
		splx(s);
	}
	restormap(map);
}

sorw(fp, uio)
	register struct file *fp;
	register struct uio *uio;
{
#ifdef	INET
	if (uio->uio_rw == UIO_READ)
		return(SORECEIVE((struct socket *)fp->f_socket, 0, uio, 0, 0));
	return(SOSEND((struct socket *)fp->f_socket, 0, uio, 0, 0));
#else
	return (EOPNOTSUPP);
#endif
}

soctl(fp, com, data)
	struct file *fp;
	u_int	com;
	char	*data;
	{
#ifdef	INET
	return (SOO_IOCTL(fp, com, data));
#else
	return (EOPNOTSUPP);
#endif
}

sosel(fp, flag)
	struct file *fp;
	int	flag;
{
#ifdef	INET
	return (SOO_SELECT(fp, flag));
#else
	return (EOPNOTSUPP);
#endif
}

socls(fp)
	register struct file *fp;
{
	register int error = 0;

#ifdef	INET
	if	(fp->f_data)
		error = SOCLOSE((struct socket *)fp->f_data);
	fp->f_data = 0;
#else
	error = EOPNOTSUPP;
#endif
	return(error);
}
