#define _WHOAMI			/* so param.h won't include us again */

#define engdahl
#define MYNAME	"engdahl"	/* for uucp */
#define PDP11	23
#define NONFP		/* if no floating point unit */

#ifdef	KERNEL
#    include "localopts.h"
#else
#    include <sys/localopts.h>
#endif
