#if	!defined(lint) && defined(DOSCCS)
static char sccsid[] = "@(#)wwprintf.c	3.6 (2.11BSD) 2025/12/26";
#endif

/*
 * Copyright (c) 1983 Regents of the University of California,
 * All rights reserved.  Redistribution permitted subject to
 * the terms of the Berkeley Software License Agreement.
 */

#include "ww.h"
#include <stdio.h>
#include <string.h>

wwprintf(struct ww *w, char *fmt, ...)
{
	va_list ap;
	char buf[1024];

	va_start(ap, fmt);
	vsnprintf(buf, sizeof buf, fmt, ap);
	va_end(ap);
	(void) wwwrite(w, buf, strlen(buf));
}
