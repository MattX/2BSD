#include "dr.h"
#if NDR > 0

#include "param.h"
#include <sys/drreg.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/dir.h>
#include <sys/proc.h>
#include <sys/seg.h>
#include <sys/user.h>

/*
 *	DR11-W device information
 */
struct	dr11w {
	int	i_flags;		/* interface flags */
	int	i_req;			/* request number (for timeout) */
	int	i_unit;			/* unit number of device */
	int	i_prev;			/* previous request number (timeout) */
	short	i_fun;			/* function bits */
	struct	proc *i_proc;		/* process pointer of opening process */
	int	i_sig;			/* signal to send on ATTN */
	int	i_tsig;			/* signal to send on timeout */
	struct	buf i_tab;		/* buffer for device */
	struct	buf i_buf;		/* raw device buffer */
	struct	drdevice *i_addr;	/* address of DR11-W interface */
};

struct dr11w dr11[NDR];

#define	TTIME	60			/* timeout time (in hz.) */

drattach(addr, unit)
struct drdevice *addr;
{
	if(unit > NDR)
		return(0);
	if((addr != (struct drdevice *)NULL) && (fioword(addr) != -1))
	{
		dr11[unit].i_addr = addr;
		dr11[unit].i_flags = DR_ALIVE;	/* mark as active */
		dr11[unit].i_unit = unit;
		return(1);
	}
	return(0);
}

dropen(dev)
dev_t dev;
{
	register int unit;
	register struct dr11w *drptr;
	extern int drtimeout();

	unit = minor(dev) & 07;		/* get minor device number */
	drptr = &dr11[unit];
	if((unit > NDR) || !(drptr->i_flags & DR_ALIVE))
	{
		u.u_error = ENXIO;	/* not attatched or present */
		return;
	}
	drptr->i_flags |= DR_OPEN;
	drptr->i_flags &= ~(DR_IGNORE | DR_TIMEOUT);
	drptr->i_proc = u.u_procp;	/* process pointer of opener */
	drptr->i_sig = 0;		/* clear signals (set by ioctl) */
	drptr->i_tsig = 0;
	drptr->i_fun = 0;		/* clear function */
	timeout(drtimeout, (caddr_t)drptr, TTIME);
}

drclose(dev)
dev_t dev;
{
	register int unit;
	register struct drdevice *addr;

	unit = minor(dev) & 07;
	dr11[unit].i_flags &= ~DR_OPEN;		/* clear opened status */
	addr = dr11[unit].i_addr;		/* get address of DR11-W */
	addr->csr = dr11[unit].i_fun;		/* clear IE and GO bits */
}

drstrategy(bp)
register struct buf *bp;
{
	register struct buf *dp;
	register struct dr11w *drptr;
	int unit;

	drptr = &dr11[unit = minor(bp->b_dev) & 07];
	if(!(drptr->i_flags & DR_OPEN))
	{
		bp->b_flags |= B_ERROR;	/* unit not open */
		iodone(bp);
		return;
	}
	dp = &(drptr->i_tab);		/* point to buffer */
	bp->av_forw = NULL;
	(void) _spl5();			/* lock out interrupts */

#ifdef	UNIBUS_MAP
	mapalloc(bp);
#endif	UNIBUS_MAP

	if(dp->b_actf == NULL)		/* if nothing in current buffer */
		dp->b_actf = bp;	/* this request is first */
	dp->b_actl = bp;		/* set last request */
	if(dp->b_active == 0)		/* if not active now */
		drstart(drptr);		/* start the DR11-W */
	(void) _spl0();			/* return to normal state */
}

drstart(drptr)
register struct dr11w *drptr;
{
	register struct buf *bp, *dp;
	register struct drdevice *addr;
	short com;

	if(!drptr)
		return;
	dp = &(drptr->i_tab);		/* point dp to device buffer */
	if((bp = dp->b_actf) == NULL)	/* if nothing in queue */
		return;			/* return */

	drptr->i_req++;			/* increment request number */
	if(drptr->i_flags & DR_TIMEOUT)	/* do we need timeout checking */
	{
		drptr->i_flags |= DR_TACTIVE;	/* mark active timeout */
	}
	dp->b_active = 1;		/* set active flag */
	addr = drptr->i_addr;		/* get device pointer */
	/*
	 *	Set up DR11-W for specific operation
	 */
	addr->bar = bp->b_un.b_addr;
	addr->wcr = -(bp->b_bcount >> 1);	/* DR deals in words */
	com = ((bp->b_xmem & 3) << 4) | drptr->i_fun;
	addr->csr = com;			/* set csr fun and address */
	com |= (DR_IE | DR_GO);			/* set IE and GO bits (also) */
	addr->csr = com;
}

drintr(unit)
int unit;
{
	register struct buf *bp;
	register struct drdevice *dr;
	register struct dr11w *drptr;
	mapinfo map;

	drptr = &dr11[unit];		/* point to struct for device */
	dr = drptr->i_addr;		/* get address of device */
	if(drptr->i_tab.b_active == 0)	/* if not active, return */
		return;
	bp = drptr->i_tab.b_actf;	/* point to current buffer */
	if(dr->csr & DR_ERR)		/* if error */
	{
		/*
		 * The error bit can be set in one of two ways:
		 * a 'real' error (timing, non-existant memory) or
		 * by the interface setting ATTN true.  This is
		 * not considered an 'error' but cause to send
		 * a signal to the parent process.  He (hopefully)
		 * will know what to do then.
		 */
		if(dr->csr & DR_ATTN)
		{
			dr->csr = drptr->i_fun;
			savemap(map);
			if(drptr->i_sig)
				psignal(drptr->i_proc, drptr->i_sig);
			restormap(map);
		}
		else
		{
#ifdef UCB_DEVERR
			printf("dr%d: error ", unit);
			printf("csr=%b, ", dr->csr, DRCSR_BITS);
			dr->csr = DR_ERR | drptr->i_fun;
			printf("eir=%b\n", dr->csr, DREIR_BITS);
#else !UCB_DEVERR
			unit = dr->csr;
			dr->csr = DR_ERR | drptr->i_fun;
			deverror(bp, unit, dr->csr);
#endif UCB_DEVERR
			/* now reset the DR11-W interface */
			dr->csr = DR_MANT | drptr->i_fun;
			dr->csr = drptr->i_fun;
			bp->b_flags |= B_ERROR;
		}
	}
	drptr->i_flags &= ~DR_TACTIVE;		/* we beat the timeout */
	drptr->i_tab.b_active = 0;		/* clear global stuff */
	drptr->i_tab.b_errcnt = 0;
	drptr->i_tab.b_actf = bp->b_forw;	/* link to next request */
	bp->b_resid = -(dr->wcr) << 1;		/* change words to bytes */
	iodone(bp);				/* tell system we are done */
	if(drptr->i_tab.b_actf)			/* start next request */
		drstart(drptr);
}

drread(dev)
dev_t dev;
{
	physio(drstrategy, &dr11[minor(dev) & 07].i_buf, dev, B_READ);
}

drwrite(dev)
dev_t dev;
{
	physio(drstrategy, &dr11[minor(dev) & 07].i_buf, dev, B_WRITE);
}

drioctl(dev, cmd, addr, flag)
dev_t dev;
int cmd, flag;
register caddr_t addr;
{
	register struct dr11w *drptr;
	register struct drdevice *dr;
	int sgbuf[3];

	drptr = &dr11[minor(dev) & 07];
	dr = drptr->i_addr;
	if(drptr->i_tab.b_active)
	{
		u.u_error = EINVAL;	/* no ioctl's when we are active */
		return;
	}

	switch(cmd)
	{
		case DRGTTY:
			sgbuf[0] = drptr->i_flags;
			sgbuf[1] = drptr->i_fun >> 1;
			if(copyout((caddr_t)sgbuf, addr, sizeof(sgbuf)))
				u.u_error = EFAULT;
			return;

		case DRSTTY:
			if(copyin(addr, (caddr_t)sgbuf, sizeof(sgbuf)))
			{
				u.u_error = EFAULT;
				return;
			}
			drptr->i_fun = (sgbuf[1] & 07) << 1;
			dr->csr = drptr->i_fun;
			drptr->i_flags &= ~(DR_TIMEOUT | DR_IGNORE);
			drptr->i_flags |= sgbuf[0] & (DR_TIMEOUT | DR_IGNORE);
			return;

		case DRSFUN:
			if(copyin(addr, (caddr_t)sgbuf, sizeof(sgbuf)))
			{
				u.u_error = EFAULT;
				return;
			}
			drptr->i_fun = (sgbuf[1] & 07) << 1;
			dr->csr = drptr->i_fun;
			return;

		case DRSFLAG:
			if(copyin(addr, (caddr_t)sgbuf, sizeof(sgbuf)))
			{
				u.u_error = EFAULT;
				return;
			}
			drptr->i_flags &= ~(DR_TIMEOUT | DR_IGNORE);
			drptr->i_flags |= sgbuf[0] & (DR_TIMEOUT | DR_IGNORE);
			return;

		case DRGCSR:
			sgbuf[0] = dr->csr;
			sgbuf[1] = dr->wcr;
			if(copyout((caddr_t)sgbuf, addr, sizeof(sgbuf)))
				u.u_error = EFAULT;
			return;

		case DRSSIG:
			if(copyin(addr, (caddr_t)sgbuf, sizeof(sgbuf)))
			{
				u.u_error = EFAULT;
				return;
			}
			drptr->i_sig = sgbuf[0];
			if((drptr->i_sig < 0) || (drptr->i_sig > 15))
			{
				drptr->i_sig = 0;
				u.u_error = EINVAL;
			}
			return;

		case DRESET:
			dr->csr = DR_MANT | drptr->i_fun;
			dr->csr = drptr->i_fun;
			return;

		case DRSTIME:
			if(copyin(addr, (caddr_t)sgbuf, sizeof(sgbuf)))
			{
				u.u_error = EFAULT;
				return;
			}
			drptr->i_tsig = sgbuf[1];
			if((drptr->i_tsig < 0) || (drptr->i_tsig > 15))
			{
				drptr->i_tsig = 0;
				u.u_error = EINVAL;
			}
			drptr->i_flags |= DR_TIMEOUT;
			drptr->i_flags &= ~DR_IGNORE;
			return;

		case DRCTIME:
			drptr->i_flags &= ~(DR_TIMEOUT | DR_IGNORE);
			return;

		case DRITIME:
			drptr->i_flags |= DR_IGNORE;
			return;

		case DROUTPUT:
			if(copyin(addr, (caddr_t)sgbuf, sizeof(sgbuf)))
			{
				u.u_error = EFAULT;
				return;
			}
			dr->dar = sgbuf[0];
			return;

		case DRINPUT:
			sgbuf[0] = dr->dar;
			if(copyout((caddr_t)sgbuf, addr, sizeof(sgbuf)))
				u.u_error = EFAULT;
			return;

		default:
			u.u_error = EINVAL;
	}
}

drtimeout(ptr)
caddr_t ptr;
{
	register struct dr11w *drptr;
#ifndef	NOKA5
	mapinfo map;
#endif	NOKA5

	drptr = (struct dr11w *)ptr;
	if((drptr->i_flags & DR_TACTIVE) && (drptr->i_req == drptr->i_prev))
	{
		printf("dr%d: timeout error\n", drptr->i_unit);
#ifndef	NOKA5
		savemap(map);
#endif	NOKA5
		if(drptr->i_tsig)
			psignal(drptr->i_proc, drptr->i_tsig);

#ifndef	NOKA5
		restormap(map);
#endif	NOKA5
		drabort(drptr);
#ifndef	NOKA5
		savemap(map);
#endif	NOKA5
		if(drptr->i_tab.b_actf)
			drstart(drptr);		/* start next request */
#ifndef	NOKA5
		restormap(map);
#endif	NOKA5
	}
	if(drptr->i_flags & (DR_TACTIVE | DR_OPEN))
	{
		drptr->i_prev = drptr->i_req;	/* arm timeout */
		timeout(drtimeout, ptr, TTIME);
	}
}

drabort(drptr)
register struct dr11w *drptr;
{
	register struct buf *bp;
	register struct drdevice *dr;
#ifndef	NOKA5
	mapinfo map;

	savemap(map);
#endif	NOKA5
	dr = drptr->i_addr;			/* point to device */
	bp = drptr->i_tab.b_actf;		/* point to current buffer */
	drptr->i_flags &= ~DR_TACTIVE;		/* turn off timeout active */
	drptr->i_tab.b_active = 0;		/* clean up global stuff */
	drptr->i_tab.b_errcnt = 0;
	drptr->i_tab.b_actf = bp->b_forw;	/* link to next request */
	bp->b_resid = -(dr->wcr) << 1;		/* make it bytes */
	if(!(drptr->i_flags & DR_IGNORE))
		bp->b_flags |= B_ERROR;		/* set an error condition */
	dr->csr = DR_MANT | drptr->i_fun;	/* clear IE bit in csr */
	dr->csr = drptr->i_fun;			/* restore function bits */
#ifndef	NOKA5
	restormap(map);
#endif	NOKA5
	iodone(bp);				/* done with that request */
}
#endif	NDR
