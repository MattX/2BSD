#include <sys/param.h>
#include <sys/systm.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/reg.h>
#include <sys/seg.h>
#ifdef UCB_METER
#include <sys/vm.h>
#endif

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)trap.c	3.3";
#endif


#define	EBIT	1		/* user error bit in PS: C-bit */
#define	SETD	0170011		/* SETD instruction */
#define	SYS	0104400		/* sys (trap) instruction */
#define	USER	020		/* user-mode flag added to dev */
#define	MEMORY	((physadr) 0177740) /* 11/70 "memory" subsystem */
#define	CPUER	((physadr) 0177766) /* 11/70 cpu error register */

#ifdef MENLO_KOV
extern	int _ovno;
#endif
#ifdef UCB_BUFOUT
extern	int hasmap;
int	savhasmap;
#endif

/*
 * In order to stop the system from destroying
 * kernel data space any further, allow only one
 * terminal trap. After once_thru is set, any
 * futher traps will be handled by looping in place.
 */
int	once_thru = 0;

/*
 * Offsets of the user's registers relative to
 * the saved r0. See reg.h
 */
char	regloc[] = {
#ifdef	MENLO_KOV
	R0, R1, R2, R3, R4, R5, R6, R7, RPS, ROV
#else
	R0, R1, R2, R3, R4, R5, R6, R7, RPS
#endif
};

/*
 * Called from low.s when a processor trap occurs.
 * The arguments are the words saved on the system stack
 * by the hardware and software during the trap processing.
 * Their order is dictated by the hardware and the details
 * of C's calling sequence. They are peculiar in that
 * this call is not 'by value' and changed user registers
 * get copied back on return.
 * dev is the kind of trap that occurred.
 */
trap(dev, sp, r1, nps, r0, pc, ps)
int *pc;
dev_t dev;
{
	register i;
	register *a;
	register struct sysent *callp;
	int (*fetch)();
	time_t syst;
#ifdef	UCB_BUFOUT
	extern struct buf *hasmap;
#endif
#ifdef	MENLO_KOV
	int savovno;
#endif

	syst = u.u_stime;
	u.u_fpsaved = 0;
	if ((ps&UMODE) == UMODE)
		dev |= USER;
	u.u_ar0 = &r0;
#ifdef UCB_METER
	cnt.v_trap++;
#endif
	switch(minor(dev)) {

	/*
	 * Trap not expected.
	 * Usually a kernel mode bus error.
	 * The numbers printed are used to
	 * find the hardware PS/PC as follows.
	 * (all numbers in octal 18 bits)
	 *	address_of_saved_ps =
	 *		(ka6*0100) + aps - 0140000;
	 *	address_of_saved_pc =
	 *		address_of_saved_ps - 2;
	 */
	default:
		if( once_thru )
			for(;;);
		once_thru++;
#ifdef	UCB_BUFOUT
		/*
		 * Clear hasmap if we attempt to sync the fs.
		 * Elsewise it might fail if we faulted with a mapped
		 * buffer.
		 */
		savhasmap = hasmap;
		hasmap = 0;
#endif
#ifdef MENLO_KOV
		/* must save __ovno before printf call changes it. */
		savovno = _ovno;
#endif
		printf("ka6 = %o\n", ka6->r[0]);
		printf("aps = %o\n", &ps);
		printf("pc = %o ps = %o\n", pc, ps);
#ifdef	MENLO_KOV
		printf("__ovno = %d\n", savovno);
#endif
		if((cputype == 70) || (cputype == 44))
			printf("cpuer = %o\n", CPUER->r[0]);
		printf("trap type %o\n", dev);
		panic("trap");

	case 0+USER: /* bus error */
		i = SIGBUS;
		break;

	/*
	 * If illegal instructions are not
	 * being caught and the offending instruction
	 * is a SETD, the trap is ignored.
	 * This is because C produces a SETD at
	 * the beginning of every program which
	 * will trap on CPUs without 11/45 FPU.
	 */
	case 1+USER: /* illegal instruction */
		if(fuiword((caddr_t)(pc-1)) == SETD && u.u_signal[SIGILL] == 0)
			goto out;
		i = SIGILL;
		break;

	case 2+USER: /* bpt or trace */
		i = SIGTRAP;
		ps &= ~TBIT;
		break;

	case 3+USER: /* iot */
		i = SIGIOT;
		break;

	case 5+USER: /* emt */
#ifdef MENLO_OVLY
		if(u.u_ovdata.uo_ovbase != 0 && r0 <= 7 && r0 > 0)
		{
#ifdef UCB_METER
			cnt.v_ovly++;
#endif
			ps &= ~EBIT;
			save(u.u_qsav);
			u.u_ovdata.uo_curov = r0;
			estabur(u.u_tsize,u.u_dsize,u.u_ssize,u.u_sep,RO);
			goto out;
		}
		else
			i = SIGEMT;
#else
		i = SIGEMT;
#endif
		break;

	case 6+USER: /* sys call */
		u.u_error = 0;
		ps &= ~EBIT;
		a = pc;
#ifdef UCB_METER
		cnt.v_syscall++;
#endif
		callp = &sysent[fuiword((caddr_t)(a-1))&077];
		if (callp == &sysent[SYSINDIR]) { /* indirect */
			a = (int *)fuiword((caddr_t)(a));
			pc++;
			i = fuword((caddr_t)a);
			a++;
			if ((i & ~077) != SYS)
				i = 077;	/* illegal */
			callp = &sysent[i&077];
			fetch = fuword;
		} else if (callp == &sysent[SYSLOCAL]) { /* local */
			a = (int *)fuiword((caddr_t)(a));
			pc++;
			i = fuword((caddr_t)a);
			a++;
			if ((i & ~037) != SYS)
				i = 037;	/* illegal */
			callp = &syslocal[i&037];
			fetch = fuword;
		} else {
			pc += callp->sy_narg - callp->sy_nrarg;
			fetch = fuiword;
		}
		for (i=0; i<callp->sy_nrarg; i++)
			u.u_arg[i] = u.u_ar0[regloc[i]];
		for(; i<callp->sy_narg; i++)
			u.u_arg[i] = (*fetch)((caddr_t)a++);
		u.u_dirp = (caddr_t)u.u_arg[0];
		u.u_r.r_val1 = u.u_ar0[R0];
		u.u_r.r_val2 = u.u_ar0[R1];
		u.u_ap = u.u_arg;
		if (save(u.u_qsav)) {
			if (u.u_error==0)
				u.u_error = EINTR;
		} else {
			(*callp->sy_call)();
#ifdef	UCB_BUFOUT
			if(hasmap != (struct buf *) 0)
				panic("hasmap");
#endif
		}
		if(u.u_error) {
			ps |= EBIT;
			u.u_ar0[R0] = u.u_error;
		} else {
			u.u_ar0[R0] = u.u_r.r_val1;
			u.u_ar0[R1] = u.u_r.r_val2;
		}
		goto out;

	/*
	 * Since the floating exception is an
	 * imprecise trap, a user generated
	 * trap may actually come from kernel
	 * mode. In this case, a signal is sent
	 * to the current process to be picked
	 * up later.
	 */
	case 8: /* floating exception */
		stst(&u.u_fperr);	/* save error code and address */
		psignal(u.u_procp, SIGFPE);
		runrun++;
		return;

	case 8+USER:
		i = SIGFPE;
		stst(&u.u_fperr);
		break;

	/*
	 * If the user SP is below the stack segment,
	 * grow the stack automatically.
	 * This relies on the ability of the hardware
	 * to restart a half executed instruction.
	 * On the 11/40 this is not the case and
	 * the routine backup/l40.s may fail.
	 * The classic example is on the instruction
	 *	cmp	-(sp),-(sp)
	 */
	case 9+USER: /* segmentation exception */
	{
	int	osp;

		osp = sp;
		if(backup(u.u_ar0) == 0)
			if(grow((unsigned)osp))
				goto out;
		i = SIGSEGV;
		break;
	}

	/*
	 * The code here is a half-hearted
	 * attempt to do something with all
	 * of the 11/70 parity registers.
	 * In fact, there is little that
	 * can be done.
	 */
	case 10:
	case 10+USER:
		printf("parity\n");
#ifdef PARITY
		if((cputype == 70) || (cputype == 44)) {
			for(i=0; i<4; i++)
				printf("%o ", MEMORY->r[i]);
			printf("\n");
			MEMORY->r[2] = -1;
			if(dev & USER) {
				i = SIGBUS;
				break;
			}
		}
#endif
		panic("parity");

	/*
	 * Allow process switch
	 */
	case USER+12:
		goto out;

	/*
	 * Locations 0-2 specify this style trap, since
	 * DEC hardware often generates spurious
	 * traps through location 0.  This is a
	 * symptom of hardware problems and may
	 * represent a real interrupt that got
	 * sent to the wrong place.  Watch out
	 * for hangs on disk completion if this message appears.
	 */
	case 15:
	case 15+USER:
		printf("Random interrupt ignored\n");
		return;
	}
	psignal(u.u_procp, i);

out:
	if(issig()) {
		psig();
	}
	curpri = setpri(u.u_procp);
	if (runrun)
		qswtch();
	if(u.u_prof.pr_scale)
		addupc((caddr_t)pc, &u.u_prof, (int)(u.u_stime-syst));
	if (u.u_fpsaved)
		restfp(&u.u_fps);
}

/*
 * nonexistent system call-- set fatal error code.
 */
nosys()
{
	u.u_error = EINVAL;
}

/*
 * Ignored system call
 */
nullsys()
{
}
