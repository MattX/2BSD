#include <sys/types.h>
#include <sys/time.h>

/*
 * this code is supposed to imitate the gettimeofday(3) call
 */

gettimeofday(tp,tzp)
struct timeval	*tp;
struct timezone	*tzp;
{

#include <sys/timeb.h>

	struct timeb	tb;

	if (!ftime(&tb)) return(-1);
	tp->tv_sec = tb.time;
	tp->tv_usec = tb.millitm * 1000;
	tzp->tz_minuteswest = tb.b_timezone;
	tzp->tz_dsttime = tb.dstflag;
	return(0);
}
