/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)ts.c	1.1 (2.10BSD Berkeley) 12/1/86
 */

/*
 *	Stand-alone TS11/TU80/TS05/TK25 1600 BPI magtape driver.
 */

#include "../h/param.h"
#include "../h/inode.h"
#include "../pdpuba/tsreg.h"
#include "saio.h"

extern int tapemark;	/* flag to indicate tapemark encountered
			   (see sys.c as to how its used) */

caddr_t tsptr;
struct	ts_char	chrbuf;		/* characteristics buffer */
struct	ts_sts	mesbuf;		/* message buffer */
struct	ts_cmd	*combuf;	/* command packet buffer */
char	softspace[sizeof(struct ts_cmd)+3];

#define	TSADDR	((struct tsdevice *)0172520)


	/* bit definitions for Command mode field during read command */
#define	TS_RPREV	0400	/* read previous (reverse) */


tsopen(io)
	register struct iob *io;
{
	register skip;

	/* combuf must be alligned on a mod 4 byte boundary */
	combuf = (struct ts_cmd *)((u_short)softspace + 3 & ~3);
	tsptr = (caddr_t)((int)&combuf->c_cmd | (int)segflag);
	combuf->c_cmd = (TS_ACK|TS_CVC|TS_INIT);
	TSADDR->tsdb = (u_short) tsptr;
	while ((TSADDR->tssr & TS_SSR) == 0)
		continue;
	chrbuf.char_bptr = (u_short) &mesbuf;
	chrbuf.char_bae = segflag;
	chrbuf.char_size = 016;
	chrbuf.char_mode = 0;
	combuf->c_cmd = (TS_ACK|TS_CVC|TS_SETCHR);
	combuf->c_loba = (u_short) &chrbuf;
	combuf->c_hiba = segflag;
	combuf->c_size = 010;
	TSADDR->tsdb = (u_short) tsptr;
	while ((TSADDR->tssr & TS_SSR) == 0)
		continue;
	tsstrategy(io, TS_REW);
	skip = io->i_boff;
	while (skip--) {
		io->i_cc = 1;
		while (tsstrategy(io, TS_SFORW))
			continue;
	}
}
tsclose(io)
	register struct iob *io;
{
	tsstrategy(io, TS_REW);
}

tsstrategy(io, func)
	register struct iob *io;
{
	register unit, errcnt;

	unit = io->i_unit;
	errcnt = 0;
	combuf->c_loba = (u_short) io->i_ma;
	combuf->c_hiba = segflag;
	combuf->c_size = io->i_cc;
	if (func == READ)
		combuf->c_cmd = TS_ACK|TS_RCOM;
	else if (func == WRITE)
		combuf->c_cmd = TS_ACK|TS_WCOM;
	else
		combuf->c_cmd = TS_ACK|func;
	TSADDR->tsdb = (u_short) tsptr;
retry:
	while ((TSADDR->tssr & TS_SSR) == 0)
		continue;
	if (mesbuf.s_xs0 & TS_TMK) {
		tapemark = 1;
		return(0);
	}
	if (TSADDR->tssr & TS_SC) {
		if (errcnt == 0)
		    printf("\nTS tape error: sr=%o xs0=%o xs1=%o xs2=%o xs3=%o",
			TSADDR->tssr,
			mesbuf.s_xs0, mesbuf.s_xs1,
			mesbuf.s_xs2, mesbuf.s_xs3);
		if (errcnt++ == 10) {
			printf("\n(FATAL ERROR)\n");
			return(-1);
		}
		if (func == READ)
			combuf->c_cmd = (TS_ACK|TS_RPREV|TS_RCOM);
		else if (func == WRITE)
			combuf->c_cmd = (TS_ACK|TS_RETRY|TS_WCOM);
		else {
			printf("\n");
			return(-1);
		}
		TSADDR->tsdb = (u_short) tsptr;
		goto retry;
	}
	if (errcnt)
		printf("\n(RECOVERED by retry)\n");
	return (io->i_cc+mesbuf.s_rbpcr);
}
