/* $Header: setline.c,v 1.8 85/05/20 20:48:37 rick Exp $ */
/* from: @(#)setline.c	5.1 (Berkeley) 7/2/83 */

#include "uucp.h"
#ifdef	USG
#include <termio.h>
#endif

#define PACKSIZE	64
#define SNDFILE	'S'
#define RCVFILE 'R'
#define RESET	'X'

/*LINTLIBRARY*/

/*
 *	optimize line setting for sending or receiving files
 *
 *	return code - none
 */
/*ARGSUSED*/
setupline(type)
char type;
{
#ifdef	USG
	static struct termio tbuf, sbuf;
	static int set = 0;

	DEBUG(2, "setline - %c\n", type);
	if (IsTcpIp)
		return;
	switch(type) {
	case SNDFILE:
		break;
	case RCVFILE:
		ioctl(Ifn, TCGETA, &tbuf);
		sbuf = tbuf;
		tbuf.c_cc[VMIN] = PACKSIZE;
		ioctl(Ifn, TCSETAW, &tbuf);
		set++;
		break;
	case RESET:
		if (set == 0) break;
		set = 0;
		ioctl(Ifn, TCSETAW, &sbuf);
		break;
	}
#endif
}
