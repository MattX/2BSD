/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)cpu.h	1.3 (2.11BSD GTE) 1/14/95
 */

/*
 * Define others as needed.  The old practice of defining _everything_
 * for _all_ models and then attempting to 'ifdef' the mess on a particular
 * cputype was simply too cumbersome (and didn't work when moving kernels
 * between cpu types).
*/
#define	PDP1170_LEAR	((physadr) 0177740)

/*
 * CTL_MACHDEP definitions.
 */
#define	CPU_CONSDEV		1	/* dev_t: console terminal device */
#define	CPU_MAXID		2	/* number of valid machdep ids */

#ifndef	KERNEL
#define CTL_MACHDEP_NAMES { \
	{ 0, 0 }, \
	{ "console_device", CTLTYPE_STRUCT }, \
}
#endif
