
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/conf.h>
#include <sys/seg.h>
#include <sys/time.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)rc.c	3.1";
#endif


#define RTADDR ((struct device *) 0160770)

	struct device  {
	int  rtymd;
	int  rthm;
	int  rtsec;
	int  rtcsr;
};
/*
 * rcread() : read real time clock
 *
 */

rcread(dev) 
{

	struct tm t;

	if(u.u_count != sizeof(t))
	{
		u.u_error = EFAULT;
		return;
	}
	t.tm_year = (((RTADDR->rtymd) >> 9) & 0177);
	t.tm_mon = (((RTADDR->rtymd) >> 5) & 017);
	t.tm_mday = ((RTADDR->rtymd) & 037);
	t.tm_hour = (((RTADDR->rthm) >> 8) & 037);
	t.tm_min = ((RTADDR->rthm) & 077);
	t.tm_sec = ((RTADDR->rtsec) & 077);
	t.tm_wday = -1;
	t.tm_yday = -1;
	t.tm_isdst = -1;

	if (copyout((caddr_t)&t, (caddr_t)u.u_base, sizeof t) <0)
		  u.u_error = EINVAL;
	u.u_count -= sizeof t;
}
/*
 * rcwrite() : set real time clock
 */

rcwrite(dev)
{
	struct tm t;


	if(u.u_count != sizeof(t))
	{
		u.u_error=EINVAL;
		return;
	}
	if (copyin((caddr_t)u.u_base, (caddr_t)&t, sizeof(t)) <0)
	{
		u.u_error=EINVAL;
		return;
	}
	if (suser()) {
		register int ymd;
		register int hm;
		if((t.tm_year < 0) || (t.tm_year > 99) ||
		   (t.tm_mon < 1) || (t.tm_mon > 12) ||
		   (t.tm_mday < 1) || (t.tm_mday > 31) ||
		   (t.tm_hour < 0) || (t.tm_hour > 23) ||
		   (t.tm_min < 0) || (t.tm_min > 59))
		{
			u.u_error=EINVAL;
			return;
		}
		ymd = ((((t.tm_year) <<4) | t.tm_mon) <<5) | t.tm_mday;
		hm = (t.tm_hour<<8) | t.tm_min;

		do {					/*set rtymd field*/
	        		RTADDR->rtymd = ymd;
			while (RTADDR->rtymd != ymd)
				;
			RTADDR->rtsec = ymd;
		}  while (RTADDR->rtymd!=ymd);

		do {					/*set rthm field*/
	        		RTADDR->rthm = hm;
			while (RTADDR->rthm != hm)
				;
			RTADDR->rtsec = hm;
		}  while (RTADDR->rthm!=hm);
	}
}
