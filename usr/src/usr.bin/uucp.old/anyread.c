/* $Header: anyread.c,v 1.5 85/05/20 20:00:13 rick Exp $ */
/* from: @(#)anyread.c	5.1 (Berkeley) 7/2/83 */

#include "uucp.h"
#include <sys/stat.h>

/*LINTLIBRARY*/

/*
 *	anyread		check if anybody can read
 *	return SUCCESS ok: FAIL not ok
 */
anyread(file)
char *file;
{
	struct stat s;

	if (stat(subfile(file), &s) < 0)
		/* for security check a non existant file is readable */
		return SUCCESS;
	if (!(s.st_mode & ANYREAD))
		return FAIL;
	return SUCCESS;
}
