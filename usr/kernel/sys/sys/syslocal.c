#include <sys/param.h>
#include <sys/dir.h>
#include <sys/proc.h>
#include <sys/seg.h>
#include <sys/user.h>
#include <sys/quota.h>
#include <sys/systm.h>
#include <sys/inode.h>
#include <sys/ino.h>
#include <sys/file.h>
#include <sys/conf.h>
#ifdef UCB_LSTAT
#include <sys/lstat.h>
#endif

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)syslocal.c	3.3";
#endif


/*
 * These routines implement local system calls
 */

/*
 * nostk -- Set up the process to have no stack segment.
 *	    The process is responsible for the management
 *	    of its own stack, and can thus use the full
 *	    64k byte address space.
 */

nostk() {
	register size;

	size = u.u_procp->p_size - u.u_ssize;
	if(estabur(u.u_tsize, u.u_dsize, 0, u.u_sep, RO))
		return;
	u.u_ssize = 0;
	expand(size);
	return;
}

/*
 * fetchi -- fetch from user I space
 *	     required because the mfpi instruction
 *	     does not work if current and previous
 *	     are both user
 */

fetchi() {
	struct a {
		caddr_t iaddr;
	};

	u.u_r.r_val1 = fuiword(((struct a *)u.u_ap)->iaddr);
}

/*
 * quota -- establish or change the quota on a directory
 */
quota()
{
#ifdef UCB_QUOTAS
	register struct inode	*ip;
	register struct a {
		char	*name;
		daddr_t	current, max;
	} *uap;

	uap = (struct a *) u.u_ap;
	if (!suser())
		return;
	ip = namei(uchar, 0);
	if (ip == NULL) {
		qprint(01)("QUOTA: namei() returns NULL\n");
		return;
	}
	/*
	 * Round the current up to be a multiple of the
	 * counting unit.
	 */
#if	QCOUNT==2
	ip->i_un.i_qused = ((uap->current + QCOUNT - 1) >> 1) << 1;
#else
	ip->i_un.i_qused = (uap->current + QCOUNT - 1) / QCOUNT * QCOUNT;
#endif
	ip->i_un.i_qmax = uap->max;
	ip->i_flag |= IUPD|IACC;
	qprint(01)("QUOTA: current = %D, max = %D\n", uap->current, uap->max);
	iput(ip);
#else
	nosys();
#endif
}

/*
 * the lfstat system call.
 */
lfstat()
{
#ifdef UCB_LSTAT
	register struct file		*fp;
	register struct a {
		int	fdes;
		struct lstat *sb;
	}			*uap;

	uap = (struct a *) u.u_ap;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	lstat1(fp->f_inode, uap->sb, fp->f_flag&FPIPE? fp->f_un.f_offset: 0);
#else
	nosys();
#endif
}

/*
 * the lstat system call.
 */
lstat()
{
#ifdef	UCB_LSTAT
	register struct inode	*ip;
	register struct a {
		char	*fname;
		struct lstat *sb;
	}			*uap;

	uap = (struct a *) u.u_ap;
	ip = namei(uchar, 0);
	if(ip == NULL)
		return;
	lstat1(ip, uap->sb, (off_t)0);
	iput(ip);
#else
	nosys();
#endif
}

/*
 * The basic routine for lfstat and lstat:
 * get the inode and pass all of it back.
 */
#ifdef	UCB_LSTAT
lstat1(ip, ub, pipeadj)
register struct inode	*ip;
struct inode		*ub;
off_t			pipeadj;
{
	register struct dinode	*dp;
	register struct buf	*bp;
	struct lstat		ls, *lp;

#ifdef UCB_FSFIX
	iupdat(ip, &time, &time, 0);
#else
	iupdat(ip, &time, &time);
#endif
	/*
	 * first copy from inode table
	 */
	lp = &ls;
	*((struct inode *) lp) = *ip;
	/*
	 * next the dates in the disk
	 */
	bp = bread(ip->i_dev, itod(ip->i_number));
#ifdef	UCB_BUFOUT
	dp = (struct dinode *) mapin(bp);
#else
	dp = bp->b_un.b_dino;
#endif
	dp += itoo(ip->i_number);
	ls.ls_atime = dp->di_atime;
	ls.ls_mtime = dp->di_mtime;
	ls.ls_ctime = dp->di_ctime;
#ifdef	UCB_BUFOUT
	mapout(bp);
#endif
	brelse(bp);
	if (copyout((caddr_t) & ls, (caddr_t) ub, sizeof ls) < 0)
		u.u_error = EFAULT;
}
#endif

/*
 * chfile -- change all references to the inode named by
 *	     device/inum to the file referred to by fd.
 */

chfile() {
	register struct file *fp;
	register struct inode *from;
	register struct inode *to;
	off_t offset;
	dev_t dev;
	int rw;
	struct a {
		dev_t	device;
		ino_t	inum;
		int	fd;
	} *uap;

	if(!suser())
		return;
	uap = (struct a *) u.u_ap;
	fp = getf(uap->fd);
	if(fp == NULL)
		return;
	if(fp->f_flag & FPIPE) {
		u.u_error = EBADF;
		return;
	}
	for(from = &inode[0]; from < &inode[NINODE]; from++)
		if(from->i_number == uap->inum && from->i_dev == uap->device)
			break;
	if(from >= &inode[NINODE])
		return;
	offset = fp->f_un.f_offset;
	to = fp->f_inode;
	from->i_count++;
	for(fp = &file[0]; fp < &file[NFILE]; fp++) {
		if(fp->f_count > 0 && fp->f_inode == from) {
			fp->f_inode = to;
			to->i_count++;
			fp->f_un.f_offset = offset;
			rw |= fp->f_flag & FWRITE;
			iput(from);
		}
	}
	/*
	 * This inode is no longer referenced.
	 * Switch out to the appropriate close
	 * routine, if required
	 */
	dev = (dev_t)from->i_un.i_rdev;
	switch(from->i_mode & IFMT) {

	case IFCHR:
	case IFMPC:
		(*cdevsw[major(dev)].d_close)(dev, rw);
		break;
	
	case IFBLK:
	case IFMPB:
		(*bdevsw[major(dev)].d_close)(dev, rw);
		break;

	default:
		break;
	}
	iput(from);
}

/*
 * killpg -- send all processes the specified
 *	     process group the given signal.
 */

killpg() {
	register struct a {
		int pgrp;
		int sig;
	} *uap;

	uap = (struct a *) u.u_ap;
	killgrp(uap->pgrp, uap->sig, 0);
}

/*
 * killbkg -- signal processes in the specified group that
 *	      have not been blessed by a submit call
 */
#ifdef UCB_SUBM

killbkg() {
	register struct a {
		int pgrp;
		int sig;
	} *uap;

	uap = (struct a *) u.u_ap;
	killgrp(uap->pgrp, uap->sig, SSUBM);
}
#endif

/*
 * common code for killpg and killbkg
 *
 * if mask is non-zero, send signal
 * only to processes whose (p_flag & mask)
 * is zero.
 */

killgrp(pgrp, sig, mask)
register pgrp;
register sig;
{
	register struct proc *p;
	int count;

	if(!suser())
		return;
	count = 0;
	for(p = &proc[2]; p < &proc[NPROC]; p++) {
		if(p->p_stat == SZOMB || p->p_pgrp != pgrp)
			continue;
		/*
		 * include following if suser is immune
		 *
		if(p->p_uid == 0)
			continue;
		 *
		 */
		if(mask && (mask & p->p_flag)) {
			continue;
		}
		count++;
		psignal(p, sig);
	}
	if(count == 0)
		u.u_error = ESRCH;
}

/*
 * submit -- mark the specified process to allow execution after logout
 */
#ifdef	UCB_SUBM

submit() {
	register struct proc *p;
	register group;
	register pid;
	struct a {
		int pid;
	};

	pid = ((struct a *)u.u_ap)->pid;
	group = u.u_procp->p_pgrp;
	for(p = &proc[2]; p < &proc[NPROC]; p++)
		if(p->p_pid == pid) {
			if(p->p_pgrp != group && !suser())
				return;
			p->p_flag |= SSUBM;
			return;
		}
	u.u_error = ESRCH;
}
#endif

/*
 * login -- mark a process as a login process,
 *	    and record accounting information.
 */
#ifdef	UCB_LOGIN

login() {
	register i;
	register struct a {
		int	tslot;
		char	crn[4];
	} *uap;

	if(!suser())
		return;
	uap = (struct a *) u.u_ap;
	u.u_login = uap->tslot;
	for(i = 0; i < sizeof u.u_crn; i++)
		u.u_crn[i] = uap->crn[i];
}

#endif
/*
 * iwait system call - used by init.
 * Search for a terminated (zombie) child,
 * finally lay it to rest, and collect its status.
 * Look also for stopped (traced) children,
 * and pass back status from them.
 *
 * For now this is an almost direct copy of
 * the wait() call.  The two can be merged
 * to save some space.
 */
iwait()
{
	register f;
	register struct proc *p;
	struct a {
		caddr_t tslotp;
	} *uap;

	f = 0;
	uap = (struct a *) u.u_ap;

loop:
	for(p = &proc[0]; p < &proc[NPROC]; p++)
	if(p->p_ppid == u.u_procp->p_pid) {
		f++;
		if(p->p_stat == SZOMB) {
			u.u_r.r_val1 = p->p_pid;
			u.u_r.r_val2 = ((struct xproc *)p)->xp_xstat;
#ifdef	UCB_LOGIN
			suword(uap->tslotp, ((struct xproc *)p)->xp_login);
#endif
			u.u_cutime += ((struct xproc *)p)->xp_utime;
			u.u_cstime += ((struct xproc *)p)->xp_stime;
			p->p_pid = 0;
			p->p_ppid = 0;
			p->p_pgrp = 0;
			p->p_sig = 0;
			p->p_flag = 0;
			p->p_wchan = 0;
			p->p_stat = NULL;
			return;
		}
		if(p->p_stat == SSTOP) {
			if((p->p_flag&SWTED) == 0) {
				p->p_flag |= SWTED;
				u.u_r.r_val1 = p->p_pid;
				u.u_r.r_val2 = (fsig(p)<<8) | 0177;
				return;
			}
			continue;
		}
	}
	if(f) {
		sleep((caddr_t)u.u_procp, PWAIT);
		goto loop;
	}
	u.u_error = ECHILD;
}

/*
 * establish a new process group
 */

setpgrp() {
	struct proc *pp;

	if(!suser())
		return;
	pp = u.u_procp;
	pp->p_pgrp = pp->p_pid;
}

/*
 * gldav -- get the load averages
 */

gldav() {
#ifdef	UCB_LOAD
	extern short avenrun[];
	register struct a {
		int *ptr;
	} *uap;

	uap = (struct a *) *u.u_ap;
	copyout( (caddr_t)avenrun, (caddr_t)uap, 3*sizeof(short));
#else
	nosys();
#endif
}
/*
 * fperr - return floating point error registers
 */

fperr() {
	u.u_r.r_val1 = u.u_fperr.f_fec;
	u.u_r.r_val2 = u.u_fperr.f_fea;
}
