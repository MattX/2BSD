#define _WHOAMI			/* so param.h won't include us again */

#define REL
#define MYNAME	"carto"		/* for uucp */
#define sysname	"carto"		/* for news and others */
#define PDP11	44
/* #define	NONFP		/* if no floating point unit */
/* #define	ENABLE34	/* support the ENABLE/34 board */

#ifdef	KERNEL
#    include "localopts.h"
#else
#    include <sys/localopts.h>
#endif
