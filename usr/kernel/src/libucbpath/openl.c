/*	@(#)openl.c	2.2	SCCS id keyword	*/
#include <errno.h>
extern	errno;

/*
 *	openl(buffer, mode, list1,....0)
 */


openl(buffer, mode, list)
char *buffer;
int mode;
char *list;
{
	_concat(buffer, &list);
	return(open(buffer, mode));
}
