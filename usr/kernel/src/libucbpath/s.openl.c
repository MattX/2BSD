h32345
s 00001/00000/00016
d D 2.2 80/09/02 13:51:35 ucb 2 1
c UCB version, SCCS id keyword added (mss)
e
s 00016/00000/00000
d D 2.1 80/09/02 13:46:56 ucb 1 0
c date and time created 80/09/02 13:46:56 by ucb
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
E 1
