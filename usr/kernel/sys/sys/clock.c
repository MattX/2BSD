#include <sys/param.h>
#include <sys/systm.h>
#include <sys/callo.h>
#include <sys/seg.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/reg.h>
#ifdef	UCB_DKEXT
#include <sys/dk.h>
extern	struct	dk	dk;
#endif
#ifdef UCB_METER
#include <sys/text.h>
#include <sys/vm.h>
#endif

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)clock.c	3.2";
#endif

#ifdef UCB_FRCSWAP
extern int	swpdebug;
extern int	idleflg;	/* If set, allow incore forks and expands */
				/* Set before idle(), cleared in clock.c */
#endif

#define	SCHMAG	8/10

/*
 * clock is called straight from
 * the real time clock interrupt.
 *
 * Functions:
 *	reprime clock
 *	copy *switches to display
 *	implement callouts
 *	maintain user/system times
 *	maintain date
 *	profile
 *	lightning bolt wakeup (every second)
 *	alarm clock signals
 *	jab the scheduler
 */

clock(dev, sp, r1, nps, r0, pc, ps)
dev_t dev;
caddr_t pc;
{
	register struct callo *p1, *p2;
	register struct proc *pp;
# ifdef	UCB_DKEXT
	int	st_t;
# endif
	int a;
	extern caddr_t waitloc;

	/*
	 * restart clock
	 */
#ifdef	CHEAP1123
	if(lks)
#endif
	lks->r[0] = 0115;

	/*
	 * display register
	 */

	display();
	/*
	 * callouts
	 * if none, just continue
	 * else update first non-zero time
	 */
	if(callout[0].c_func == NULL)
		goto out;
	p2 = &callout[0];
	while(p2->c_time<=0 && p2->c_func!=NULL)
		p2++;
	p2->c_time--;

	/*
	 * if ps is high, just return
	 */
	if (BASEPRI(ps))
		goto out;

	/*
	 * callout
	 */
	spl6();
	if(callout[0].c_time <= 0) {
		p1 = &callout[0];
		while(p1->c_func != 0 && p1->c_time <= 0) {
			(*p1->c_func)(p1->c_arg);
			p1++;
		}
		p2 = &callout[0];
		while(p2->c_func = p1->c_func) {
			p2->c_time = p1->c_time;
			p2->c_arg = p1->c_arg;
			p1++;
			p2++;
		}
	}
	/*
	 * lightning bolt time-out
	 * and time of day
	 */
out:
# ifdef	UCB_DKEXT
	dk.dk_0busy &= 077;
	dk.dk_1busy &= 077;
	st_t = 0;
# endif

#ifdef	UCB_DKEXT
	a = dk_busy&047;
#else
	a = dk_busy&07;
#endif
	if (USERMODE(ps)) {
		u.u_utime++;
		if(u.u_prof.pr_scale)
			addupc(pc, &u.u_prof, 1);
		if(u.u_procp->p_nice > NZERO)
#ifdef	UCB_DKEXT
			st_t = 1,a += 8;
#else
			a += 8;
#endif
	} else {
#ifdef	UCB_DKEXT
		st_t = 2;
#endif
		a += 16;
		if (pc == waitloc)
#ifdef	UCB_DKEXT
			st_t = 3,a += 8;
#else
			a += 8;
#endif
		u.u_stime++;
	}
	dk_time[a] += 1;
#ifdef	UCB_DKEXT
	dk.dk_0time[dk.dk_0busy] +=1;
	dk.dk_1time[dk.dk_1busy] += 1;
	dk.sys_time[st_t] += 1;
#endif
	pp = u.u_procp;
	if(++pp->p_cpu == 0)
		pp->p_cpu--;
	if(++lbolt >= HZ) {
		if (BASEPRI(ps))
			return;
		lbolt -= HZ;
#ifdef	UCB_FRCSWAP
		idleflg = 0;
#endif
		++time;
#if UCB_LOAD || UCB_METER
		meter();
#endif
		spl1();
		runrun++;
		wakeup((caddr_t)&lbolt);
		for(pp = &proc[0]; pp < &proc[NPROC]; pp++)
		if (pp->p_stat && pp->p_stat<SZOMB) {
			if(pp->p_time != 127)
				pp->p_time++;
#ifdef	UCB_METER
			if (pp->p_stat == SSLEEP || pp->p_stat == SSTOP)
				if (pp->p_slptime != 127)
					pp->p_slptime++;
#endif	UCB_METER
			if(pp->p_clktim)
				if(--pp->p_clktim == 0)
					psignal(pp, SIGALRM);
			a = (pp->p_cpu & 0377)*SCHMAG + pp->p_nice - NZERO;
			if(a < 0)
				a = 0;
			if(a > 255)
				a = 255;
			pp->p_cpu = a;
			if(pp->p_pri >= PUSER)
				setpri(pp);
		}
		if(runin!=0) {
			runin = 0;
			wakeup((caddr_t)&runin);
		}
	}
}

/*
 * timeout is called to arrange that
 * fun(arg) is called in tim/HZ seconds.
 * An entry is sorted into the callout
 * structure. The time in each structure
 * entry is the number of HZ's more
 * than the previous entry.
 * In this way, decrementing the
 * first entry has the effect of
 * updating all entries.
 *
 * The panic is there because there is nothing
 * intelligent to be done if an entry won't fit.
 */
timeout(fun, arg, tim)
int (*fun)();
caddr_t arg;
{
	register struct callo *p1, *p2;
	register int t;
	int s;

	t = tim;
	p1 = &callout[0];
	s = spl7();
	while(p1->c_func != 0 && p1->c_time <= t) {
		t -= p1->c_time;
		p1++;
	}
	if (p1 >= &callout[NCALL-1])
		panic("Timeout table overflow");
	p1->c_time -= t;
	p2 = p1;
	while(p2->c_func != 0)
		p2++;
	while(p2 >= p1) {
		(p2+1)->c_time = p2->c_time;
		(p2+1)->c_func = p2->c_func;
		(p2+1)->c_arg = p2->c_arg;
		p2--;
	}
	p1->c_time = t;
	p1->c_func = fun;
	p1->c_arg = arg;
	splx(s);
}

#if UCB_LOAD || UCB_METER
/*
 * Count up various things once a second
 */
short avenrun[3];	/* internal load average in psuedo-floating point */
#define ave(smooth, new, time)	(smooth) = (((time)-1) * (smooth) + (new))/ (time)

meter()
{
#ifdef UCB_METER
	register long *cp, *rp, *sp;

	ave(avefree, freemem, 5);
	cp = &cnt.v_first; rp = &rate.v_first; sp = &sum.v_first;
	while (cp <= &cnt.v_last) {
		ave(*rp, *cp, 5);
		*sp += *cp;
		*cp = 0;
		rp++, cp++, sp++;
	}
#endif
	if (time % 5 == 0) {
		vmtotal();
#ifdef UCB_METER
		rate.v_swpin = cnt.v_swpin;
		sum.v_swpin += cnt.v_swpin;
		cnt.v_swpin = 0;
		rate.v_swpout = cnt.v_swpout;
		sum.v_swpout += cnt.v_swpout;
		cnt.v_swpout = 0;
#endif
	}
}

vmtotal()
{
	register struct proc *p;
	register struct text *xq;
	int nrun = 0;
#ifdef UCB_METER
	/*
	 * Array of flags so we don't count a text more than once.
	 * Could be NTEXT/8 if I wanted to mess with shifting.
	 */
	char counted[NTEXT];
	int nt;

	total.t_vmtxt = 0;
	total.t_avmtxt = 0;
	total.t_rmtxt = 0;
	total.t_armtxt = 0;
	for (xq = &text[0]; xq < &text[NTEXT]; xq++) {
		counted[xq-text]=0;
		if (xq->x_iptr) {
			total.t_vmtxt += xq->x_size;
			if (xq->x_ccount)
				total.t_rmtxt += xq->x_caddr;
		}
	}
	total.t_vm = 0;
	total.t_avm = 0;
	total.t_rm = 0;
	total.t_arm = 0;
	total.t_rq = 0;
	total.t_dw = 0;
	total.t_pw = 0;
	total.t_sl = 0;
	total.t_sw = 0;
#endif
	for (p = &proc[0]; p < &proc[NPROC]; p++) {
		if (p->p_flag & SSYS)
			continue;
		if (p->p_stat) {
#ifdef UCB_METER
			total.t_vm += p->p_size;
			if (p->p_textp)
				total.t_rm += p->p_size;
#endif
			switch (p->p_stat) {

			case SSLEEP:
			case SSTOP:
				if (p->p_pri <= PZERO)
					nrun++;
#ifdef UCB_METER
				if (p->p_flag & SLOAD) {
					if (p->p_pri <= PZERO)
						total.t_dw++;
					else if (p->p_slptime < MAXSLP)
						total.t_sl++;
				} else if (p->p_slptime < MAXSLP)
					total.t_sw++;
				if (p->p_slptime < MAXSLP)
					goto active;
#endif
				break;

			case SRUN:
			case SIDL:
				nrun++;
#ifdef UCB_METER
				if (p->p_flag & SLOAD)
					total.t_rq++;
				else
					total.t_sw++;
active:
				total.t_avm += p->p_size;
				if (p->p_textp) {
					total.t_arm += p->p_size;
					nt = p->p_textp-text;
					if (counted[nt]==0) {
						counted[nt]=1;
						if (p->p_textp->x_ccount)
							total.t_avmtxt += p->p_textp->x_size;
					}
				}
#endif
				break;
			}
		}
	}
#ifdef UCB_METER
	total.t_vm += total.t_vmtxt;
	total.t_avm += total.t_avmtxt;
	total.t_rm += total.t_rmtxt;
	total.t_arm += total.t_armtxt;
	total.t_free = avefree;
#endif
	loadav(avenrun, nrun);
}

/*
 * Compute Tenex style load average.  This code is adapted from similar
 * code by Bill Joy on the Vax system.  The major change is that we
 * avoid floating point since not all pdp-11's have it.  This makes
 * the code quite hard to read - it was derived with some algebra.
 *
 * "floating point" numbers here are stored in a 16 bit short, with
 * 8 bits on each side of the decimal point.  Some partial products
 * will have 16 bits to the right.
 */

	/*
	 * The Vax algorithm is:
	 *
	 * /*
	 *  * Constants for averages over 1, 5, and 15 minutes
	 *  * when sampling at 5 second intervals.
	 *  * /
	 * double	cexp[3] = {
	 * 	0.9200444146293232,	/* exp(-1/12) * /
	 * 	0.9834714538216174,	/* exp(-1/60) * /
	 * 	0.9944598480048967,	/* exp(-1/180) * /
	 * };
	 * 
	 * /*
	 *  * Compute a tenex style load average of a quantity on
	 *  * 1, 5 and 15 minute intervals.
	 *  * /
	 * loadav(avg, n)
	 * 	register double *avg;
	 * 	int n;
	 * {
	 * 	register int i;
	 * 
	 * 	for (i = 0; i < 3; i++)
	 * 		avg[i] = cexp[i] * avg[i] + n * (1.0 - cexp[i]);
	 * }
	 */

long cexp[3] = {
	0353,	/* 256*exp(-1/12) */
	0373,	/* 256*exp(-1/60) */
	0376,	/* 256*exp(-1/180) */
};

loadav(avg, n)
	register short *avg;
	int n;
{
	register int i;

	for (i = 0; i < 3; i++)
		avg[i] = (cexp[i] * (avg[i]-(n<<8)) + (((long)n)<<16)) >> 8;
}
#endif
