/*	@(#)openlp.c	2.2	SCCS id keyword	*/

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
