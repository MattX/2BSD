#if     defined(DOSCCS) && !defined(lint)
static char *sccsid = "@(#)fpfcom.c 1.0 (2.11BSD) 2025/12/25";
#endif

#include <stdio.h>
#include "local.h"

/*
 * Format floating point numbers.
 * A formatted number is but in a buffer of size pi->nallo, ending at 
 * address pi->bend. Beginning of number buffer is returned, and end
 * of buffer must be pi->bend.
 */
char *
__pfcom(int ch, struct prinfo *pi)
{
	return pi->bend;
}
