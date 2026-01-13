/* Copyright (c) 1979 Regents of the University of California */

#if !defined(lint) && defined(DOSCCS)
static	char sccsid[] = "@(#)version.c	(2.11BSD) 2.2 2025/12/26";
#endif

    /*
     *	this writes the declaration of the character string version
     *	onto standard output.
     *	useful for makeing Version.c give the correct date for pi.
     */

#include	<time.h>

long		clock;
char		*cstring;

main()
    {
	time( &clock );
	cstring = ctime( &clock );
	cstring[ 24 ] = '\0';
	printf( "char	version[] = \"%s\";\n" , cstring );
	return 0;
    }

