h51377
s 00001/00000/00032
d D 2.2 80/09/02 13:51:41 ucb 2 1
c UCB version, SCCS id keyword added (mss)
e
s 00032/00000/00000
d D 2.1 80/09/02 13:46:59 ucb 1 0
c date and time created 80/09/02 13:46:59 by ucb
e
u
U
f b 
f n 
t
T
I 2
/*	%W%	SCCS id keyword	*/
E 2
I 1

#include <errno.h>
extern	errno;
char *makefp();

/*
 *	openlp(buffer, path, list1,....0)
 */


openlp(buffer, mode, list)
char *buffer;
int mode;
char *list;
{
	char *bestpath = 0;
	int besterr = ENOENT;
	int fd;
	char *p;
	resetfp();
	while (p = makefp(bestpath, buffer, &list)) {
		fd = open(buffer, mode);
		if (fd >= 0)
			return(fd);
		if (errno != ENOENT && besterr == ENOENT) {
			besterr = errno;
			bestpath = p;
		}
	}
	errno = besterr;
	return(-1);
}
E 1
