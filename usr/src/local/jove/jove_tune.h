/*
 *			J O V E _ T U N E . H 
 *
 * $Revision: 1.2 $
 *
 * $Log:	jove_tune.h,v $
 * Revision 1.2  83/12/16  00:10:28  dpk
 * Added distinctive RCS header
 * 
 */

#define TMPFILE		"/tmp/joveXXXXXX"
			/* Where jove should put the tmp file */
#define	STDSHELL	"/bin/sh"	/* /bin/sh or /bin/csh */
					/* getenv("SHELL") will overide */
#define DESCRIBE	"/usr/brl/lib/jove/describe.com"
#define FINDCOM		"/usr/brl/lib/jove/findcom"
#define JOVERC		"/usr/brl/lib/jove/joverc"
			/* Where to search for the describe command */
#define	DFLTMODE	0666

/* #define LSRHS_KLUDGERY		/* Probably ONLY if you are at LS */

#define	NOINTR	1	/* Define this to not allow interrupting the
			 * the editor, signaling subprocs still OK
			 */
#define BIGTMP	1
	/* BIGTMP allows 512k bytes in tmp file.  Otherwise 256k bytes */

#ifdef vax
#define VMUNIX	1		/* Vax unix */
#endif

#ifndef VMUNIX
typedef	short	disk_line;
#ifndef BUFSIZ
#define BUFSIZ	512
#endif
#else
typedef	int	disk_line;
#ifndef BUFSIZ
#define BUFSIZ	4096
#endif
#endif
