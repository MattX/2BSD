static char sccsid[] = "@(#)tmsrvr.c	1.0 (SRI) 6/6/83";
/*
 * Network TCP time server
 */
#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>

#define	SINCE1970	2208988800L	/* seconds from Jan 1, 1900 to 1970 */

time_t timebuf;

long time(), ntohl();


main()
{
	time(&timebuf);
	timebuf += SINCE1970;
	timebuf = ntohl(timebuf);
	write(1, &timebuf, sizeof(timebuf));
	fflush(stdout);
}
