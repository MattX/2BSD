#include <sys/types.h>
#include <sys/time.h>

/*
 * this code is supposed to imitate the utimes(2) call
 */

utimes(file,tvp)
char	*file;
struct timeval	*tvp[2];
{
	time_t	timep[2];

	timep[0] = tvp[0]->tv_sec;
	timep[1] = tvp[1]->tv_sec;

	return(utime(file,timep));
}
