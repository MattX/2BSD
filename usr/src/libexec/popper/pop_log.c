/*
 * Copyright (c) 1989 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if	!defined(lint) && defined(DOSCCS)
static char copyright[] = "Copyright (c) 1990 Regents of the University of California.\nAll rights reserved.\n";
static char SccsId[] = "@(#)@(#)pop_log.c	2.2  (2.11BSD) 2025/12/26";
#endif

#include <stdio.h>
#include <sys/types.h>
#include "popper.h"

/* 
 *  log:    Make a log entry
 */

static char msgbuf[MAXLINELEN];

pop_log(POP *p, int stat, char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vsprintf(msgbuf, format, ap);
	va_end(ap);

        vsprintf(msgbuf,format,ap);

    if (p->debug && p->trace) {
        (void)fprintf(p->trace,"%s\n",msgbuf);
        (void)fflush(p->trace);
    }
    else {
        syslog (stat,"%s",msgbuf);
    }

    return(stat);
}
