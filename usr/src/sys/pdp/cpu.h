/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)cpu.h	1.2 (2.11BSD GTE) 12/26/92
 */

/*
 * Define others as needed.  The old practice of defining _everything_
 * for _all_ models and then attempting to 'ifdef' the mess on a particular
 * cputype was simply too cumbersome (and didn't work when moving kernels
 * between cpu types).
*/
#define	PDP1170_LEAR	((physadr) 0177740)
