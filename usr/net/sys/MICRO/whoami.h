#define _WHOAMI			/* so param.h won't include us again */

#define micro11
#define MYNAME	"Lmicro11"	/* for uucp */
#define PDP11	22
/* #define NONFP		/* if no floating point unit */

#ifdef	KERNEL
#    include "localopts.h"
#else
#    include <sys/localopts.h>
#endif
