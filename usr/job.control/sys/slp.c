#include <sys/param.h>
#include <sys/systm.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/text.h>
#include <sys/map.h>
#include <sys/file.h>
#include <sys/inode.h>
#include <sys/buf.h>
#ifdef UCB_METER
#include <sys/vm.h>
#endif

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)slp.c	3.2";
#endif

#ifdef UCB_FRCSWAP
extern int	swpdebug;
extern int	idleflg	;	/* If set, allow incore forks and expands */
				/* Set before idle(), cleared in clock.c */
#endif

#define SQSIZE 0100	/* Must be power of 2 */
#define HASH(x)	(( (int) x >> 5) & (SQSIZE-1))
struct proc *slpque[SQSIZE] = 0; /* initialized to force out of mapped page */

/*
 * Give up the processor till a wakeup occurs
 * on chan, at which time the process
 * enters the scheduling queue at priority pri.
 * The most important effect of pri is that when
 * pri<=PZERO a signal cannot disturb the sleep;
 * if pri>PZERO signals will be processed.
 * Callers of this routine must be prepared for
 * premature return, and check that the reason for
 * sleeping has gone away.
 */
sleep(chan, pri)
caddr_t chan;
{
	register struct proc *rp;
#ifdef	MENLO_JCL
	register struct proc **hp;
#else
	register h;
	struct proc *q;
#endif
	register s;

	rp = u.u_procp;
	s = spl6();
#ifdef	MENLO_JCL
	if (chan==0 || rp->p_stat != SRUN )
		panic("sleep");
#else
	if (chan==0)
		panic("zero wchan");
	rp->p_stat = SSLEEP;
#endif
	rp->p_wchan = chan;
#ifdef UCB_METER
	rp->p_slptime = 0;
#endif
	rp->p_pri = pri;
#ifdef	MENLO_JCL
	hp = &slpque[HASH(chan)];
	rp->p_link = *hp;
	*hp = rp;
#else
	h = HASH(chan);
	/*
	 * remove this diagnostic loop
	 * when you're sure it can't happen
	 */
	for(q=slpque[h]; q!=NULL; q=q->p_link)
		if(q == rp) {
			printf("proc asleep %d\n", rp->p_pid);
			goto cont;
		}
	rp->p_link = slpque[h];
	slpque[h] = rp;
cont:
#endif
	if(pri > PZERO) {
#ifdef	MENLO_JCL
		if(issig(rp)) {
			if (rp->p_wchan)
				unsleep(rp);
			rp->p_stat = SRUN;
			(void) spl0();
			goto psig;
		}
		if (rp->p_wchan == 0)
			goto out;
		rp->p_stat = SSLEEP;
#else
		if(issig()) {
			rp->p_wchan = 0;
			rp->p_stat = SRUN;
			slpque[h] = rp->p_link;
			spl0();
			goto psig;
		}
#endif
		spl0();
		if(runin != 0) {
			runin = 0;
			wakeup((caddr_t)&runin);
		}
		swtch();
#ifdef	MENLO_JCL
		if(issig(rp))
			goto psig;
	} else {
		rp->p_stat = SSLEEP;
		(void) spl0();
		swtch();
	}
out:
#else
		if(issig())
			goto psig;
	} else {
		spl0();
		swtch();
	}
#endif
	splx(s);
	return;

	/*
	 * If priority was low (>PZERO) and
	 * there has been a signal,
	 * execute non-local goto to
	 * the qsav location.
	 * (see trap1/trap.c)
	 */
psig:
	resume(u.u_procp->p_addr, u.u_qsav);
	/*NOTREACHED*/
}

#ifdef	MENLO_TIM
/*
 * Sleep on chan at pri.
 * Return in no more than the indicated number of seconds.
 * (If seconds==0, no timeout implied)
 * Return	TS_OK if chan was awakened normally
 *		TS_TIME if timeout occurred
 *		TS_SIG if asynchronous signal occurred
 */
tsleep(chan, pri, seconds)
caddr_t chan;
{
	label_t lqsav;
	register struct proc *pp;
	register sec, n, rval;

	pp = u.u_procp;
	n = spl7();
	sec = 0;
	rval = 0;
	if (pp->p_clktim && pp->p_clktim<seconds)
		seconds = 0;
	if (seconds) {
		pp->p_flag |= STIMO;
		sec = pp->p_clktim-seconds;
		pp->p_clktim = seconds;
	}
	bcopy((caddr_t)u.u_qsav, (caddr_t)lqsav, sizeof (label_t));
	if (save(u.u_qsav))
		rval = TS_SIG;
	else {
		sleep(chan, pri);
		if ((pp->p_flag&STIMO)==0 && seconds)
			rval = TS_TIME;
		else
			rval = TS_OK;
	}
	pp->p_flag &= ~STIMO;
	bcopy((caddr_t)lqsav, (caddr_t)u.u_qsav, sizeof (label_t));
	if (sec > 0)
		pp->p_clktim += sec;
	else
		pp->p_clktim = 0;
	splx(n);
	return(rval);
}
#endif
#ifdef	MENLO_JCL

/*
 * Remove a process from its wait queue
 */
unsleep(p)
register struct proc *p;
{
	register struct proc **hp;
	register s;

	s = spl6();
	if (p->p_wchan) {
		hp = &slpque[HASH(p->p_wchan)];
		while (*hp != p)
			hp = &(*hp)->p_link;
		*hp = p->p_link;
		p->p_wchan = 0;
	}
	splx(s);
}

#endif

/*
 * Wake up all processes sleeping on chan.
 */

wakeup(chan)
register caddr_t chan;
{
	register struct proc *p, **q, **h;
	register i;
	int s;

	s = spl6();
	h = &slpque[HASH(chan)];
restart:
	for (q = h; p = *q; ) {
		if (p->p_stat != SSLEEP && p->p_stat != SSTOP)
			panic("wakeup");
		if (p->p_wchan==chan) {
			p->p_wchan = 0;
			*q = p->p_link;
#ifdef	UCB_METER
			p->p_slptime = 0;
#endif
			if (p->p_stat == SSLEEP) {
				setrun(p);
				goto restart;
			}
		} else
			q = &p->p_link;
	}
	splx(s);
}

#ifndef	MENLO_SCH
/*
 * when you are sure that it
 * is impossible to get the
 * 'proc on q' diagnostic, the
 * diagnostic loop can be removed.
 */
setrq(p)
struct proc *p;
{
	register struct proc *q;
	register s;

	s = spl6();
	for(q=runq; q!=NULL; q=q->p_link)
		if(q == p) {
			printf("proc on q %d\n", p->p_pid);
			goto out;
		}
	p->p_link = runq;
	runq = p;
out:
	splx(s);
}
#endif

/*
 * Set the process running;
 * arrange for it to be swapped in if necessary.
 */
setrun(p)
register struct proc *p;
{
#ifdef	MENLO_JCL
	register s;

	s = spl6();
	switch (p->p_stat) {

	case 0:
	case SWAIT:
	case SRUN:
	case SZOMB:
	default:
		panic("setrun");

	case SSTOP:
	case SSLEEP:
		unsleep(p);		/* e.g. when sending signals */
		break;

	case SIDL:
		break;
	}
	p->p_stat = SRUN;
	if (p->p_flag & SLOAD)
		setrq(p);
	splx(s);
#else
	register caddr_t w;

	if (p->p_stat==0 || p->p_stat==SZOMB)
		panic("Running a dead proc");
	/*
	 * The assignment to w is necessary because of
	 * race conditions. (Interrupt between test and use)
	 */
	if (w = p->p_wchan) {
		wakeup(w);
		return;
	}
	p->p_stat = SRUN;
	setrq(p);
#endif

#ifdef	CGL_RTP
	if(p->p_pri < curpri || p==rtpp)
#else
	if(p->p_pri < curpri)
#endif
		runrun++;
	if(runout != 0 && (p->p_flag&SLOAD) == 0) {
		runout = 0;
		wakeup((caddr_t)&runout);
	}
}

/*
 * Set user priority.
 * The rescheduling flag (runrun)
 * is set if the priority is better
 * than the currently running process.
 */
setpri(pp)
register struct proc *pp;
{
	register p;

	p = (pp->p_cpu & 0377)/16;
	p += PUSER + pp->p_nice - NZERO;
	if(p > 127)
		p = 127;
	if(p < curpri)
		runrun++;
	pp->p_pri = p;
	return(p);
}

/*
 * The main loop of the scheduling (swapping)
 * process.
 * The basic idea is:
 *  see if anyone wants to be swapped in;
 *  swap out processes until there is room;
 *  swap him in;
 *  repeat.
 * The runout flag is set whenever someone is swapped out.
 * Sched sleeps on it awaiting work.
 *
 * Sched sleeps on runin whenever it cannot find enough
 * core (by swapping out or otherwise) to fit the
 * selected swapped process.  It is awakened when the
 * core situation changes and in any case once per second.
 */
sched()
{
	register struct proc *rp, *p;
	register outage, inage;
	int maxsize;

	/*
	 * find user to swap in;
	 * of users ready, select one out longest
	 */

loop:
	spl6();
	outage = -20000;
	for (rp = &proc[0]; rp < &proc[NPROC]; rp++)
	if (rp->p_stat==SRUN && (rp->p_flag&SLOAD)==0 &&
	    rp->p_time - (rp->p_nice-NZERO)*8 > outage) {
		p = rp;
		outage = rp->p_time - (rp->p_nice-NZERO)*8;
	}
	/*
	 * If there is no one there, wait.
	 */
	if (outage == -20000) {
		runout++;
		sleep((caddr_t)&runout, PSWP);
		goto loop;
	}
	spl0();

	/*
	 * See if there is core for that process;
	 * if so, swap it in.
	 */

	if (swapin(p))
		goto loop;

	/*
	 * none found.
	 * look around for core.
	 * Select the largest of those sleeping
	 * at bad priority; if none, select the oldest.
	 */

	spl6();
	p = NULL;
	maxsize = -1;
	inage = -1;
	for (rp = &proc[0]; rp < &proc[NPROC]; rp++) {
		if (rp->p_stat==SZOMB
		 || (rp->p_flag&(SSYS|SLOCK|SULOCK|SLOAD))!=SLOAD)
			continue;
		if (rp->p_textp && rp->p_textp->x_flag&XLOCK)
			continue;
		if (rp->p_stat==SSLEEP&&rp->p_pri>=PZERO || rp->p_stat==SSTOP) {
			if (maxsize < rp->p_size) {
				p = rp;
				maxsize = rp->p_size;
			}
		} else if (maxsize<0 && (rp->p_stat==SRUN||rp->p_stat==SSLEEP)) {
			if (rp->p_time+rp->p_nice-NZERO > inage) {
				p = rp;
				inage = rp->p_time+rp->p_nice-NZERO;
			}
		}
	}
	spl0();
	/*
	 * Swap found user out if sleeping at bad pri,
	 * or if he has spent at least 2 seconds in core and
	 * the swapped-out process has spent at least 3 seconds out.
	 * Otherwise wait a bit and try again.
	 */
	if (maxsize>=0 || (outage>=3 && inage>=2)) {
		p->p_flag &= ~SLOAD;
#ifdef	MENLO_SCH
		if(p->p_stat == SRUN)
			remrq(p);
#endif
		xswap(p, 1, 0);
		goto loop;
	}
	spl6();
	runin++;
	sleep((caddr_t)&runin, PSWP);
	goto loop;
}

/*
 * Swap a process in.
 * Allocate data and possible text separately.
 * It would be better to do largest first.
 */
swapin(p)
register struct proc *p;
{
	register struct text *xp;
	register int a;
	int x;

	if ((a = malloc(coremap, p->p_size)) == NULL)
		return(0);
	if (xp = p->p_textp) {
		xlock(xp);
		if (xp->x_ccount==0) {
			if ((x = malloc(coremap, xp->x_size)) == NULL) {
				xunlock(xp);
				mfree(coremap, p->p_size, a);
				return(0);
			}
			xp->x_caddr = x;
			if ((xp->x_flag&XLOAD)==0)
				swap(xp->x_daddr,x,xp->x_size,B_READ);
		}
		xp->x_ccount++;
		xunlock(xp);
	}
	swap(p->p_addr, a, p->p_size, B_READ);
	mfree(swapmap, ctod(p->p_size), p->p_addr);
	p->p_addr = a;
	p->p_flag |= SLOAD;
	p->p_time = 0;
	return(1);
}

#ifndef	MENLO_SCH
/*
 * put the current process on
 * the Q of running processes and
 * call the scheduler.
 */
qswtch()
{

	setrq(u.u_procp);
	swtch();
}

/*
 * This routine is called to reschedule the CPU.
 * if the calling process is not in RUN state,
 * arrangements for it to restart must have
 * been made elsewhere, usually by calling via sleep.
 * There is a race here. A process may become
 * ready after it has been examined.
 * In this case, idle() will be called and
 * will return in at most 1HZ time.
 * i.e. its not worth putting an spl() in.
 */
swtch()
{
	register n;
	register struct proc *p, *q, *pp, *pq;

#ifdef	UCB_BUFOUT
	extern struct buf *hasmap;
	if(hasmap != (struct buf *)0)
		panic("swtch hasmap");
#endif
	/*
	 * If not the idle process, resume the idle process.
	 */
	if (u.u_procp != &proc[0]) {
		if (save(u.u_rsav)) {
			sureg();
			return;
		}
		if (u.u_fpsaved==0) {
			savfp(&u.u_fps);
			u.u_fpsaved = 1;
		}
		resume(proc[0].p_addr, u.u_qsav);
	}
	/*
	 * The first save returns nonzero when proc 0 is resumed
	 * by another process (above); then the second is not done
	 * and the process-search loop is entered.
	 *
	 * The first save returns 0 when swtch is called in proc 0
	 * from sched().  The second save returns 0 immediately, so
	 * in this case too the process-search loop is entered.
	 * Thus when proc 0 is awakened by being made runnable, it will
	 * find itself and resume itself at rsav, and return to sched().
	 */
	if (save(u.u_qsav)==0 && save(u.u_rsav))
		return;
#ifdef UCB_METER
	cnt.v_swtch++;
#endif
loop:
	spl6();
	runrun = 0;
#ifdef	CGL_RTP
	/*
	 * Test for the presence of a "real time process".
	 * If there is one and it is runnable, give it top
	 * priority.
	 */
	if ((p=rtpp) && p->p_stat==SRUN && (p->p_flag&SLOAD)) {
		pq = NULL;
		for (q=runq; q!=NULL; q=q->p_link) {
			if (q == p)
				break;
			pq = q;
		}
		if (q == NULL)
			panic("rtp not found\n");	/* "cannot happen" */
		n = PRTP;
		goto runem;
	}
#endif
	pp = NULL;
	q = NULL;
	n = 128;
	/*
	 * Search for highest-priority runnable process
	 */
	for(p=runq; p!=NULL; p=p->p_link) {
		if((p->p_stat==SRUN) && (p->p_flag&SLOAD)) {
			if(p->p_pri < n) {
				pp = p;
				pq = q;
				n = p->p_pri;
			}
		}
		q = p;
	}
	/*
	 * If no process is runnable, idle.
	 */
	p = pp;
	if(p == NULL) {
#ifdef UCB_FRCSWAP
		idleflg++;
#endif
		idle();
		goto loop;
	}
#ifdef CGL_RTP
runem:
#endif
	q = pq;
	if(q == NULL)
		runq = p->p_link;
	else
		q->p_link = p->p_link;
	curpri = n;
	spl0();
	/*
	 * The rsav (ssav) contents are interpreted in the n	ew address space
	 */
	n = p->p_flag&SSWAP;
	p->p_flag &= ~SSWAP;
	resume(p->p_addr, n? u.u_ssav: u.u_rsav);
}
#endif

/*
 * Create a new process-- the internal version of
 * sys fork.
 * It returns 1 in the new process, 0 in the old.
 */

newproc()
{
	int a1, a2;
	struct proc *p, *up;
	register struct proc *rpp, *rip;
	register n;

	p = NULL;
	/*
	 * First, just locate a slot for a process
	 * and copy the useful info from this process into it.
	 * The panic "cannot happen" because fork has already
	 * checked for the existence of a slot.
	 */
retry:
	mpid++;
	if(mpid >= 30000) {
		mpid = 0;
		goto retry;
	}
	for(rpp = &proc[0]; rpp < &proc[NPROC]; rpp++) {
		if(rpp->p_stat == NULL && p==NULL)
			p = rpp;
		if (rpp->p_pid==mpid || rpp->p_pgrp==mpid)
			goto retry;
	}
	if ((rpp = p)==NULL)
		panic("no procs");

	/*
	 * make proc entry for new proc
	 */

	rip = u.u_procp;
	up = rip;
	rpp->p_stat = SRUN;
	rpp->p_clktim = 0;
	rpp->p_flag = SLOAD;
#ifdef	UCB_SUBM
	rpp->p_flag |= rip->p_flag & SSUBM;
#endif
	rpp->p_uid = rip->p_uid;
	rpp->p_pgrp = rip->p_pgrp;
	rpp->p_nice = rip->p_nice;
	rpp->p_textp = rip->p_textp;
	rpp->p_pid = mpid;
	rpp->p_ppid = rip->p_pid;
	rpp->p_time = 0;
	rpp->p_cpu = 0;
#ifdef	UCB_METER
	rpp->p_slptime = 0;
#endif
#ifdef	UCB_FRCSWAP
	if(swpdebug & 01) printf("np:%o,op:%o\n",rpp->p_pid,rip->p_pid);
#endif

	/*
	 * make duplicate entries
	 * where needed
	 */

	for(n=0; n<NOFILE; n++)
		if(u.u_ofile[n] != NULL)
			u.u_ofile[n]->f_count++;
	if(up->p_textp != NULL) {
		up->p_textp->x_count++;
		up->p_textp->x_ccount++;
	}
	u.u_cdir->i_count++;
	if (u.u_rdir)
		u.u_rdir->i_count++;
	/*
	 * Partially simulate the environment
	 * of the new process so that when it is actually
	 * created (by copying) it will look right.
	 */
	rpp = p;
	u.u_procp = rpp;
	rip = up;
	n = rip->p_size;
	a1 = rip->p_addr;
	rpp->p_size = n;
	/*
	 * When the resume is executed for the new process,
	 * here's where it will resume.
	 */
	if (save(u.u_ssav)) {
		sureg();
		return(1);
	}
#ifndef UCB_FRCSWAP
	a2 = malloc(coremap, n);
#else
	if(idleflg)
		a2 = malloc(coremap, n);
	else
		a2 = NULL;
#endif
	/*
	 * If there is not enough core for the
	 * new process, swap out the current process to generate the
	 * copy.
	 */
	if(a2 == NULL) {
#ifdef	UCB_FRCSWAP
	if(swpdebug & 04) printf("sw:%o\n",rpp->p_pid);
#endif
		rip->p_stat = SIDL;
		rpp->p_addr = a1;
		xswap(rpp, 0, 0);
		rip->p_stat = SRUN;
#ifdef	CGL_RTP
		u.u_procp = rip; /* see comments below */
#endif
	} else {
		/*
		 * There is core, so just copy.
		 */
		rpp->p_addr = a2;
#ifdef	CGL_RTP
		/*
		 * Copyseg is now an interruptable kernel process.
		 * The u. area is non-reentrant so copy it first
		 * in non-interruptable mode.
		 */
		{ int i;
		  for(i=0; i<USIZE; i++)
			copy1seg(a1++, a2++);
		}
		n -= USIZE;
		/*
		 * If we are to be interrupted we must insure consistency;
		 * restore current process state now.
		 */
		u.u_procp = rip;
		while(n--)
			copyseg(a1++, a2++);
	}
#else
		while(n--)
			copyseg(a1++, a2++);
	}
	u.u_procp = rip;
#endif
	setrq(rpp);
	rpp->p_flag |= SSWAP;
	return(0);
}
/*
 * Change the size of the data+stack regions of the process.
 * If the size is shrinking, it's easy-- just release the extra core.
 * If it's growing, and there is core, just allocate it
 * and copy the image, taking care to reset registers to account
 * for the fact that the system's stack has moved.
 * If there is no core, arrange for the process to be swapped
 * out after adjusting the size requirement-- when it comes
 * in, enough core will be allocated.
 *
 * After the expansion, the caller will take care of copying
 * the user's stack towards or away from the data area.
 */
expand(newsize)
{
	register i, n;
	register struct proc *p;
	register a1, a2;
/*
	int	state;
*/

	p = u.u_procp;
	n = p->p_size;
	p->p_size = newsize;
	a1 = p->p_addr;
	if(n >= newsize) {
		mfree(coremap, n-newsize, a1+newsize);
		return;
	}
	if (save(u.u_ssav)) {
		sureg();
		return;
	}
	if (u.u_fpsaved==0) {
		savfp(&u.u_fps);
		u.u_fpsaved = 1;
	}
#ifdef UCB_FRCSWAP
	if(idleflg)
		a2 = malloc(coremap, newsize);
	else
		a2 = NULL;
#else
	a2 = malloc(coremap, newsize);
#endif
	if(a2 == NULL) {
#ifdef UCB_FRCSWAP
		if(swpdebug & 040) printf("exsw:%o\n",p->p_pid);
#endif
		xswap(p, 1, n);
		p->p_flag |= SSWAP;
		qswtch();
		/* no return */
	}
/*
	state = p->p_stat;
	p->p_stat = SIDL;
*/
#ifdef	CGL_RTP
	for (i=0; i<USIZE; i++)
		copy1seg(a1+i, a2+i);	/* see comments in newproc() */
	for (; i<n; i++)
		copyseg(a1+i, a2+i);
	p->p_addr = a2;
#else
	p->p_addr = a2;
	for(i=0; i<n; i++)
		copyseg(a1+i, a2+i);
#endif
/*
	p->p_stat = state;
*/
	mfree(coremap, n, a1);
	resume(a2, u.u_ssav);
}
#ifdef	CGL_RTP
/*
 * Test status of the "real time process";
 * preempt the current process if runnable.
 * Use caution when calling this routine, much
 * of the kernel is non-reentrant!
 */
runrtp()
{
	register struct proc *p;

	if ((p=rtpp)==NULL || p==u.u_procp)
		return;
	if (p->p_stat==SRUN && (p->p_flag&SLOAD)!=0) {
		u.u_procp->p_pri = PRTP+1;
		qswtch();
	}
}
#endif
