static char *RCSid = ":   RCS/assert.v  Revision 1.2  83/04/11  04:47:02  adams  Exp$";

/*
 * assert(bool,char,char *)
 *
 * 	program verification...
 *
 *	If bool is true then return, else print a message and exit(-1)
 *	
 *	If char = p then perror(3) is used. 
 *
 */


#include <stdio.h>

assert (bool, c, s)
int     bool;
char    c;
char   *s;
{
    if (bool)
	return;
    if (c == 'p')
	perror (s);
    else
	fprintf (stderr, "%s\n", s);
    exit (-1);
}
