/* $Header: index.c,v 1.3 85/01/03 16:57:20 rick Exp $ */
/* from: @(#)index.c	5.1 (Berkeley) 7/2/83 */

#include <stdio.h>

/*
 *	return pointer to character c
 *
 *	return codes:
 *		NULL  -  character not found
 *		pointer  -  pointer to character
 */

char *
index(str, c)
register char c, *str;
{
	for (; *str != '\0'; str++) {
		if (*str == c)
			return str;
	}

	return NULL;
}
