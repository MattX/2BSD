#define _WHOAMI			/* so param.h won't include us again */

#define PROBUILD
#define MYNAME	"probuild"	/* for uucp */
#define PDP11	21
/* #define NONFP		/* if no floating point unit */

#ifdef	KERNEL
#    include "localopts.h"
#else
#    include <sys/localopts.h>
#endif
