/* $Header: getwd.c,v 1.2 85/05/15 18:59:47 rick Exp $ */
/* from: @(#)gwd.c	5.1 (Berkeley) 7/2/83 */

#include "uucp.h"

/*
 *	get working directory
 *
 *	return codes  0 = FAIL
 *		      wkdir = SUCCES
 */

char *
getwd(wkdir)
register char *wkdir;
{
	register FILE *fp;
	extern FILE *rpopen();
	extern int rpclose();
	register char *c;

	*wkdir = '\0';
	if ((fp = rpopen("PATH=/bin:/usr/bin:/usr/ucb;pwd 2>&-", "r")) == NULL)
		return 0;
	if (fgets(wkdir, 100, fp) == NULL) {
		rpclose(fp);
		return 0;
	}
	if (*(c = wkdir + strlen(wkdir) - 1) == '\n')
		*c = '\0';
	rpclose(fp);
	return wkdir;
}
