/*
 * Various subroutines neede to make netnews work on v6.
 */

#include <stdio.h>
#include <grp.h>
#include <sys/timeb.h>
static char sccsid = "@(#)v6.c	1.2	9/14/80";

/*
 * Compare strings (at most n bytes):  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */

strncmp(s1, s2, n)
register char *s1, *s2;
register n;
{

	while (--n >= 0 && *s1 == *s2++)
		if (*s1++ == '\0')
			return(0);
	return(n<0 ? 0 : *s1 - *--s2);
}

/*
 * Copy s2 to s1, truncating or null-padding to always copy n bytes
 * return s1
 */

char *
strncpy(s1, s2, n)
register char *s1, *s2;
{
	register i;
	register char *os1;

	os1 = s1;
	for (i = 0; i < n; i++)
		if ((*s1++ = *s2++) == '\0') {
			while (++i < n)
				*s1++ = '\0';
			return(os1);
		}
	return(os1);
}

struct group *
getgrnam(name)
register char *name;
{
	register struct group *p;
	struct group *getgrent();

	setgrent();
	while( (p = getgrent()) && strcmp(p->gr_name,name) );
	endgrent();
	return(p);
}

#define	CL	':'
#define	CM	','
#define	NL	'\n'
#define	MAXGRP	100

static char GROUP[] = "/etc/group";
static FILE *grf = NULL;
static char line[BUFSIZ+1];
static struct group group;
static char *gr_mem[MAXGRP];

setgrent()
{
	if( !grf )
		grf = fopen( GROUP, "r" );
	else
		rewind( grf );
}

endgrent()
{
	if( grf ){
		fclose( grf );
		grf = NULL;
	}
}

static char *
grskip(p,c)
register char *p;
register c;
{
	while( *p && *p != c ) ++p;
	if( *p ) *p++ = 0;
	return( p );
}

struct group *
getgrent()
{
	register char *p, **q;

	if( !grf && !(grf = fopen( GROUP, "r" )) )
		return(NULL);
	if( !(p = fgets( line, BUFSIZ, grf )) )
		return(NULL);
	group.gr_name = p;
	group.gr_passwd = p = grskip(p,CL);
	group.gr_gid = atoi( p = grskip(p,CL) );
	group.gr_mem = gr_mem;
	p = grskip(p,CL);
	grskip(p,NL);
	q = gr_mem;
	while( *p ){
		*q++ = p;
		p = grskip(p,CM);
	}
	*q = NULL;
	return( &group );
}

umask(newmask)
int newmask;
{
	return 022;
}

geteuid()
{
	int uid;

	uid = getuid();
	uid = (uid >> 8) & 0377;
	return uid;
}

ftime(tp)
struct timeb *tp;
{
	time(&(tp->time));
	tp->millitm = 0;
	tp -> timezone = TIMEZONE;
	tp -> dstflag = 0;
}
