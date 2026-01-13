#if	!defined(lint) && defined(DOSCCS)
static char sccsid[] = "@(#)bigram.c	4.3	(2.11BSD)	2026/1/5";
#endif

/*
 *  bigram < text > bigrams
 * 
 * List bigrams for 'updatedb' script.
 * Use 'code' to encode a file using this output.
 */

#include <stdio.h>
#include <strings.h>

#define MAXPATH	1024		/* maximum pathname length */

char path[MAXPATH];
char oldpath[MAXPATH] = " ";	

main ( )
{
  	register int count, j;
	char *cp;

     	while ( fgets(path, sizeof path, stdin ) != NULL ) {
		cp = index(path, '\n');
		if (cp) *cp = '\0';
		count = prefix_length ( oldpath, path );
		/*
		   output post-residue bigrams only
		*/
		for ( j = count; path[j] != NULL; j += 2 ) {
			if ( path[j + 1] == NULL ) 
				break;
			putchar ( path[j] );
			putchar ( path[j + 1] );
			putchar ( '\n' );
		}
		strcpy ( oldpath, path );
   	}
}

prefix_length ( s1, s2 )	/* return length of longest common prefix */
	char *s1, *s2;		/* ... of strings s1 and s2 */
{
	register char *start;

    	for ( start = s1; *s1 == *s2; s1++, s2++ )	
		if ( *s1 == NULL )		
	    		break;
    	return ( s1 - start );
}
