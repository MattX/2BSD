#include <sys/param.h>
#include <sys/systm.h>
#include <sys/map.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/buf.h>
#include <sys/reg.h>
#include <sys/inode.h>
#include <sys/seg.h>
#include <sys/acct.h>
# include <sys/file.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)sys1.c	3.3";
#endif


/*
 * exec system call, with and without environments.
 */
struct execa {
	char	*fname;
	char	**argp;
	char	**envp;
};

exec()
{
	((struct execa *)u.u_ap)->envp = NULL;
	exece();
}

exece()
{
	register nc;
	register char *cp;
	register struct buf *bp;
	register struct execa *uap;
	int na, ne, bno, ucp, ap, c;
	struct inode *ip;
# ifdef MENLO_SCRIPT
# define SCRMAG '#!'
	extern int schar();
	int uid,gid,indir;
# endif

	if ((ip = namei(uchar, 0)) == NULL)
		return;
	bno = 0;
	bp = 0;
# ifdef MENLO_SCRIPT
	indir = 0;
	uid = u.u_uid;
	gid = u.u_gid;
	/*formerly if(uid!=0) ...?*/
	if( ip->i_mode&ISUID )
	    uid = ip->i_uid;
	if( ip->i_mode&ISGID )
	    gid = ip->i_gid;
again:
# endif
	if(access(ip, IEXEC))
		goto bad;
	if((ip->i_mode & IFMT) != IFREG ||
	   (ip->i_mode & (IEXEC|(IEXEC>>3)|(IEXEC>>6))) == 0) {
		u.u_error = EACCES;
		goto bad;
	}
# ifdef MENLO_SCRIPT
	/*moved this from getxfile()*/
	u.u_base = (caddr_t)&u.u_exdata;
	u.u_count = sizeof u.u_exdata;
	u.u_offset = 0;
	u.u_segflg = 1;
	readi(ip);
	u.u_segflg = 0;
	if( u.u_error )
	    goto bad;
	/*check if script.  one lvl only*/
	if( indir==0
	 && u.u_exdata.ux_mag==SCRMAG
	 && u.u_count<sizeof u.u_exdata-sizeof u.u_exdata.ux_mag )
	{
	    indir++;
	    cp = (char*)&u.u_exdata+sizeof u.u_exdata.ux_mag;
	    while( *cp==' ' && cp<(char*)&u.u_exdata+sizeof u.u_exdata-1 )
		cp++;
	    u.u_dirp = cp;
	    while( cp<(char*)&u.u_exdata+sizeof u.u_exdata-1
	     && *cp!='\n' )
		cp++;
	    *cp = 000;
	    iput(ip);
	    if( (ip = namei(schar,0))==NULL )
		return;
	    goto again;
	}
	/*other magic numbers are described in getxfile()*/
# endif
	
	/*
	 * Collect arguments on "file" in swap space.
	 */
	na = 0;
	ne = 0;
	nc = 0;
	uap = (struct execa *)u.u_ap;
#ifndef UCB_NKB
	if ((bno = malloc(swapmap, (unsigned) ((NCARGS+BSIZE-1)/BSIZE))) == 0)
		panic("Out of swap");
#else
	if ((bno = malloc(swapmap, (unsigned) ctod((int) btoc(NCARGS+BSIZE)))) == 0)
		panic("Out of swap");
#endif
	if (uap->argp) for (;;) {
		ap = NULL;
# ifdef MENLO_SCRIPT
		/*insert script path name as 1th arg*/
		if( indir && na==1 )
		    ap = uap->fname;
		else
# endif
		if (uap->argp) {
			ap = fuword((caddr_t)uap->argp);
			uap->argp++;
		}
		if (ap==NULL && uap->envp) {
			uap->argp = NULL;
			if ((ap = fuword((caddr_t)uap->envp)) == NULL)
				break;
			uap->envp++;
			ne++;
		}
		if (ap==NULL)
			break;
		na++;
		if(ap == -1)
			u.u_error = EFAULT;
		do {
			if (nc >= NCARGS-1)
				u.u_error = E2BIG;
			if ((c = fubyte((caddr_t)ap++)) < 0)
				u.u_error = EFAULT;
			if (u.u_error)
				goto bad;
			if ((nc&BMASK) == 0) {
				if (bp) {
#ifdef	UCB_BUFOUT
					mapout(bp);
#endif
					bawrite(bp);
				}
#ifndef UCB_NKB
				bp = getblk(swapdev, swplo+bno+(nc>>BSHIFT));
#else
				bp = getblk(swapdev, dbtofsb(clrnd(swplo+bno))+(nc>>BSHIFT));
#endif
#ifdef	UCB_BUFOUT
				cp = mapin(bp);
#else
				cp = bp->b_un.b_addr;
#endif
			}
			nc++;
			*cp++ = c;
		} while (c>0);
	}
	if (bp) {
#ifdef	UCB_BUFOUT
		mapout(bp);
#endif
		bawrite(bp);
	}
	bp = 0;
	nc = (nc + NBPW-1) & ~(NBPW-1);
# ifndef MENLO_SCRIPT
	if (getxfile(ip, nc) || u.u_error)
		goto bad;
# else
	if( getxfile(ip,nc,uid,gid) || u.u_error )
	    goto bad;
# endif

	/*
	 * copy back arglist
	 */

	ucp = -nc - NBPW;
	ap = ucp - na*NBPW - 3*NBPW;
	u.u_ar0[R6] = ap;
	suword((caddr_t)ap, na-ne);
	nc = 0;
	for (;;) {
		ap += NBPW;
		if (na==ne) {
			suword((caddr_t)ap, 0);
			ap += NBPW;
		}
		if (--na < 0)
			break;
		suword((caddr_t)ap, ucp);
		do {
			if ((nc&BMASK) == 0) {
				if (bp) {
#ifdef	UCB_BUFOUT
					mapout(bp);
#endif
					bp->b_flags |= B_AGE;
					brelse(bp);
				}
#ifndef UCB_NKB
				bp = bread(swapdev, swplo+bno+(nc>>BSHIFT));
#else
				bp = bread(swapdev, dbtofsb(clrnd(swplo+bno))+(nc>>BSHIFT));
#endif
				bp->b_flags &= ~B_DELWRI;
#ifdef	UCB_BUFOUT
				cp = mapin(bp);
#else
				cp = bp->b_un.b_addr;
#endif
# ifdef MENLO_SCRIPT
				/*stick in interpreter name for accting*/
				/*who cares*/
				if( indir && nc==0 )
				    bcopy(cp,(caddr_t)u.u_dbuf,DIRSIZ);
# endif
			}
			subyte((caddr_t)ucp++, (c = *cp++));
			nc++;
		} while(c&0377);
	}
	suword((caddr_t)ap, 0);
	suword((caddr_t)ucp, 0);
#ifdef	UCB_BUFOUT
	if(bp) {
		mapout(bp);
		bp->b_flags |= B_AGE;
		brelse(bp);
		bp = 0;
	}
#endif
	setregs();
bad:
	if (bp) {
#ifdef	UCB_BUFOUT
		mapout(bp);
#endif
		bp->b_flags |= B_AGE;
		brelse(bp);
	}
	if(bno)
#ifndef UCB_NKB
		mfree(swapmap, (unsigned) ((NCARGS+BSIZE-1)/BSIZE), (unsigned) bno);
#else
		mfree(swapmap, (unsigned) ctod((int) btoc(NCARGS+BSIZE)), (unsigned) bno);
#endif
	iput(ip);
}

/*
 * Read in and set up memory for executed file.
 * Zero return is normal;
 * non-zero means only the text is being replaced
 */
# ifndef MENLO_SCRIPT
getxfile(ip, nargc)
# else
int
getxfile(ip,nargc,uid,gid)
    int nargc;
    int uid,gid;
# endif
register struct inode *ip;
{
	register unsigned ds;
	register sep;
	register unsigned ts, ss;
	register i, overlay;
#ifdef MENLO_OVLY
	register ovflag,ovmax;
	struct u_ovd sovdata;
	unsigned ovhead[8];
#endif
	long lsize;

# ifndef MENLO_SCRIPT
	/*
	 * read in first few bytes
	 * of file for segment
	 * sizes:
	 * ux_mag = 407/410/411/405
	 *  407 is plain executable
	 *  410 is RO text
	 *  411 is separated ID
	 *  405 is overlaid text
#ifdef MENLO_OVLY
	 *  430 is non-sep auto-overlay
	 *  431 is sep auto overlay
#endif
	 */

	u.u_base = (caddr_t)&u.u_exdata;
	u.u_count = sizeof(u.u_exdata);
	u.u_offset = 0;
	u.u_segflg = 1;
	readi(ip);
	u.u_segflg = 0;
	if(u.u_error)
		goto bad;
	if (u.u_count!=0) {
		u.u_error = ENOEXEC;
		goto bad;
	}
# endif
	sep = 0;
	overlay = 0;
#ifdef MENLO_OVLY
	ovflag = 0;
#endif
	if(u.u_exdata.ux_mag == 0407) {
		lsize = (long)u.u_exdata.ux_dsize + u.u_exdata.ux_tsize;
		u.u_exdata.ux_dsize = lsize;
		if (lsize != u.u_exdata.ux_dsize) {	/* check overflow */
			u.u_error = ENOMEM;
			goto bad;
		}
		u.u_exdata.ux_tsize = 0;
	} else if (u.u_exdata.ux_mag == 0411)
		sep++;
	else if (u.u_exdata.ux_mag == 0405)
		overlay++;
#ifdef MENLO_OVLY
	else if (u.u_exdata.ux_mag == 0430)
		ovflag++;
	else if (u.u_exdata.ux_mag == 0431) {
		sep++;
		ovflag++;
	}
#endif
	else if (u.u_exdata.ux_mag != 0410) {
		u.u_error = ENOEXEC;
		goto bad;
	}
	if(u.u_exdata.ux_tsize!=0 && (ip->i_flag&ITEXT)==0 && ip->i_count!=1) {
		u.u_error = ETXTBSY;
		goto bad;
	}

	/*
	 * find text and data sizes
	 * try them out for possible
	 * overflow of max sizes
	 */
	ts = btoc(u.u_exdata.ux_tsize);
	lsize = (long)u.u_exdata.ux_dsize + u.u_exdata.ux_bsize;
	if (lsize != (unsigned)lsize) {
		u.u_error = ENOMEM;
		goto bad;
	}
	ds = btoc(lsize);
	ss = SSIZE + btoc(nargc);
#ifdef MENLO_OVLY

	/*
	 * if auto overlay get second header
	 */

	sovdata = u.u_ovdata;
	u.u_ovdata.uo_ovbase = 0;
	u.u_ovdata.uo_curov = 0;
	if (ovflag) {
		u.u_base = (caddr_t) ovhead;
		u.u_count = sizeof(ovhead);
		u.u_offset = sizeof(u.u_exdata);
		u.u_segflg = 1;
		readi(ip);
		u.u_segflg = 0;
		if (u.u_count != 0)
			u.u_error = ENOEXEC;
		if (u.u_error) {
			u.u_ovdata = sovdata;
			goto bad;
		}
		/* set begining of overlay address space */
		u.u_ovdata.uo_ovbase = ts;
		/* 0th entry is max size of a given ovly */
		ovmax = btoc(ovhead[0]);
		/* set max number of segm. registers to be used */
		u.u_ovdata.uo_nseg = ctos(ovmax);

		/* set base of data space */
		u.u_ovdata.uo_dbase = ts + ovmax;
		/*
		 * Setup a table of offsets to each of the
		 * overlay segements. The i'th overlay runs
		 * from ov_offst[i-1] to ov_offst[i].
		 */
		for (i = 0; i < 8; i++) {
			register t;
			if (i != 0)
				/* check if any overlay is larger than ovmax */
					
				if ((t=btoc(ovhead[i])) > ovmax)
				{
					u.u_error = ENOEXEC;
					u.u_ovdata = sovdata;
					goto bad;
				}
				else	u.u_ovdata.uo_ov_offst[i] = t + u.u_ovdata.uo_ov_offst[i-1];
			else	u.u_ovdata.uo_ov_offst[i] = ts;
		}
	}

#endif
	if (overlay) {
		if (u.u_sep==0 && ctos(ts) != ctos(u.u_tsize) || nargc) {
			u.u_error = ENOMEM;
#ifdef MENLO_OVLY
			u.u_ovdata = sovdata;
#endif
			goto bad;
		}
		ds = u.u_dsize;
		ss = u.u_ssize;
		sep = u.u_sep;
		xfree();
		xalloc(ip);
		u.u_ar0[PC] = u.u_exdata.ux_entloc & ~01;
	} else {
		if(estabur(ts, ds, ss, sep, RO))
#ifdef MENLO_OVLY
		{
			u.u_ovdata = sovdata;
#endif
			goto bad;
#ifdef MENLO_OVLY
		}
#endif

		/*
		 * allocate and clear core
		 * at this point, committed
		 * to the new image
		 */
	
		u.u_prof.pr_scale = 0;
		xfree();
		i = USIZE+ds+ss;
		expand(i);
		while(--i >= USIZE)
			clearseg(u.u_procp->p_addr+i);
		xalloc(ip);
	
		/*
		 * read in data segment
		 */
	
		estabur((unsigned)0, ds, (unsigned)0, 0, RO);
		u.u_base = 0;
#ifndef MENLO_OVLY
		u.u_offset = sizeof(u.u_exdata)+u.u_exdata.ux_tsize;
#else
		u.u_offset = sizeof(u.u_exdata);
		if(ovflag)
		{
			u.u_offset += sizeof(ovhead);
			u.u_offset += (((long)u.u_ovdata.uo_ov_offst[7])<<6);
		}
		else
			u.u_offset += u.u_exdata.ux_tsize;
#endif
		u.u_count = u.u_exdata.ux_dsize;
		readi(ip);
		/*
		 * set SUID/SGID protections, if no tracing
		 */
		if ((u.u_procp->p_flag&STRC)==0) {
# ifndef MENLO_SCRIPT
			if(ip->i_mode&ISUID)
				if(u.u_uid != 0) {
					u.u_uid = ip->i_uid;
					u.u_procp->p_uid = ip->i_uid;
				}
			if(ip->i_mode&ISGID)
				u.u_gid = ip->i_gid;
# else
			u.u_uid = uid;
			u.u_procp->p_uid = uid;
			u.u_gid = gid;
# endif
		} else
			psignal(u.u_procp, SIGTRAP);
	}
	u.u_tsize = ts;
	u.u_dsize = ds;
	u.u_ssize = ss;
	u.u_sep = sep;
	estabur(ts, ds, ss, sep, RO);
bad:
	return(overlay);
}

/*
 * Clear registers on exec
 */
setregs()
{
	register int *rp;
	register char *cp;
	register i;

	for(rp = &u.u_signal[0]; rp < &u.u_signal[NSIG]; rp++)
		if((*rp & 1) == 0)
			*rp = 0;
	for(cp = &regloc[0]; cp < &regloc[6];)
		u.u_ar0[*cp++] = 0;
	u.u_ar0[PC] = u.u_exdata.ux_entloc & ~01;
	for(rp = (int *)&u.u_fps; rp < (int *)&u.u_fps.u_fpregs[6];)
		*rp++ = 0;
	for(i=0; i<NOFILE; i++) {
		if (u.u_pofile[i]&EXCLOSE) {
			closef(u.u_ofile[i]);
			u.u_ofile[i] = NULL;
			u.u_pofile[i] &= ~EXCLOSE;
		}
	}
	/*
	 * Remember file name for accounting.
	 */
#ifdef	ACCT
	u.u_acflag &= ~AFORK;
	bcopy((caddr_t)u.u_dbuf, (caddr_t)u.u_comm, DIRSIZ);
#endif
}

/*
 * exit system call:
 * pass back caller's arg
 */
rexit()
{
	register struct a {
		int	rval;
	} *uap;

	uap = (struct a *)u.u_ap;
	exit((uap->rval & 0377) << 8);
}

/*
 * Release resources.
 * Save u. area for parent to look at.
 * Enter zombie state.
 * Wake up parent and init processes,
 * and dispose of children.
 */
exit(rv)
{
	register int i;
	register struct proc *p, *q;
	register struct file *f;

	p = u.u_procp;
	p->p_flag &= ~(STRC|SULOCK);
	p->p_clktim = 0;
#ifdef	CGL_RTP
	/*
	 * if this a "real time" process that is dying
	 * remove the rtpp flag.
	 */
	if (rtpp != NULL && rtpp == p)
		rtpp = NULL;
#endif
	for(i=0; i<NSIG; i++)
		u.u_signal[i] = 1;
	for(i=0; i<NOFILE; i++) {
		f = u.u_ofile[i];
		u.u_ofile[i] = NULL;
		closef(f);
	}
	plock(u.u_cdir);
	iput(u.u_cdir);
	if (u.u_rdir) {
		plock(u.u_rdir);
		iput(u.u_rdir);
	}
	xfree();
#ifdef	ACCT
	acct();
#endif
	mfree(coremap, (unsigned) p->p_size, (unsigned) p->p_addr);
	p->p_stat = SZOMB;
	((struct xproc *)p)->xp_xstat = rv;
	((struct xproc *)p)->xp_utime = u.u_cutime + u.u_utime;
	((struct xproc *)p)->xp_stime = u.u_cstime + u.u_stime;
#ifdef	UCB_LOGIN
	((struct xproc *)p)->xp_login = u.u_login;
#endif
	for(q = &proc[0]; q < &proc[NPROC]; q++)
		if(q->p_ppid == p->p_pid) {
			wakeup((caddr_t)&proc[1]);
			q->p_ppid = 1;
			if (q->p_stat==SSTOP)
				setrun(q);
		}
	for(q = &proc[0]; q < &proc[NPROC]; q++)
		if(p->p_ppid == q->p_pid) {
			wakeup((caddr_t)q);
			swtch();
			/* no return */
		}
	swtch();
}

/*
 * Wait system call.
 * Search for a terminated (zombie) child,
 * finally lay it to rest, and collect its status.
 * Look also for stopped (traced) children,
 * and pass back status from them.
 */
wait()
{
	register f;
	register struct proc *p;

	f = 0;

loop:
	for(p = &proc[0]; p < &proc[NPROC]; p++)
	if(p->p_ppid == u.u_procp->p_pid) {
		f++;
		if(p->p_stat == SZOMB) {
			u.u_r.r_val1 = p->p_pid;
			u.u_r.r_val2 = ((struct xproc *)p)->xp_xstat;
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
 * fork system call.
 *	3/17/80: if UCB_PGRP defined, define process limit by number in
 *	process group instead of number owned by same uid.  SMF, UCB EECS
 */
fork()
{
#ifdef UCB_PGRP
	register struct proc *p1;
	struct proc *p2;
	register a, pg;
#else
	register struct proc *p1, *p2;
	register a;
#endif

	/*
	 * Make sure there's enough swap space for max
	 * core image, thus reducing chances of running out
	 */
	if ((a = malloc(swapmap, (unsigned) ctod(maxmem))) == 0) {
		u.u_error = ENOMEM;
		goto out;
	}
	mfree(swapmap, (unsigned) ctod(maxmem), (unsigned) a);
	a = 0;
#ifdef UCB_PGRP
	pg = u.u_procp->p_pgrp;	/* process group number */
#endif
	p2 = NULL;
	for(p1 = &proc[0]; p1 < &proc[NPROC]; p1++) {
		if (p1->p_stat==NULL && p2==NULL)
			p2 = p1;
		else {
#ifdef UCB_PGRP
			/*
			 * Exempt low positive uids (0-15) for users like uucp
			 * & network which shouldn't lose limits. - mrh 12/1/80
			 */
			if (p1->p_pgrp==pg && (unsigned) u.u_uid>=16 && p1->p_stat!=NULL)
				a++;
#else
			if (p1->p_uid==u.u_uid && p1->p_stat!=NULL)
				a++;
#endif
		}
	}
	/*
	 * Disallow if
	 *  No processes at all;
	 *  not su and too many procs owned (or in pgrp, if UCB_PGRP set);
	 *  or not su and would take last slot.
	 */
	if (p2==NULL || (u.u_uid!=0 && (p2==&proc[NPROC-1] || a>MAXUPRC))) {
		u.u_error = EAGAIN;
		goto out;
	}
	p1 = u.u_procp;
	if(newproc()) {
		u.u_r.r_val1 = p1->p_pid;
		u.u_start = time;
		u.u_cstime = 0;
		u.u_stime = 0;
		u.u_cutime = 0;
		u.u_utime = 0;
#ifdef	UCB_LOGIN
		u.u_login = 0;
#endif
#ifdef	ACCT
		u.u_acflag = AFORK;
#endif
		return;
	}
	u.u_r.r_val1 = p2->p_pid;

out:
	u.u_ar0[R7] += NBPW;
}

/*
 * break system call.
 *  -- bad planning: "break" is a dirty word in C.
 */
sbreak()
{
	struct a {
		char	*nsiz;
	};
	register a, n, d;
	int i;

	/*
	 * set n to new data size
	 * set d to new-old
	 * set n to new total size
	 */

	n = btoc((int)((struct a *)u.u_ap)->nsiz);
	if(!u.u_sep)
#ifdef MENLO_OVLY
		if(u.u_ovdata.uo_ovbase)
			n -= ctos(u.u_ovdata.uo_dbase) * stoc(1);
		else
			n -= ctos(u.u_tsize) * stoc(1);
#else
		n -= ctos(u.u_tsize) * stoc(1);
#endif
	if(n < 0)
		n = 0;
	d = n - u.u_dsize;
	n += USIZE+u.u_ssize;
	if(estabur(u.u_tsize, u.u_dsize+d, u.u_ssize, u.u_sep, RO))
		return;
	u.u_dsize += d;
	if(d > 0)
		goto bigger;
	a = u.u_procp->p_addr + n - u.u_ssize;
	i = n;
	n = u.u_ssize;
	while(n--) {
		copyseg(a-d, a);
		a++;
	}
	expand(i);
	return;

bigger:
	expand(n);
	a = u.u_procp->p_addr + n;
	n = u.u_ssize;
	while(n--) {
		a--;
		copyseg(a-d, a);
	}
	while(d--)
		clearseg(--a);
}


