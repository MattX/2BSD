/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if !defined(lint) && !defined(NOSCCS)
static char sccsid[] = "@(#)printw.c	5.2 (2.11BSD) 2025/12/26";
#endif

/*
 * printw and friends
 *
 */

# include	"curses.ext"

/*
 *	This routine implements a printf on the standard screen.
 */
printw(fmt, args)
char	*fmt;
int	args; {

	return _sprintw(stdscr, fmt, &args);
}

/*
 *	This routine implements a printf on the given window.
 */
wprintw(win, fmt, args)
WINDOW	*win;
char	*fmt;
int	args; {

	return _sprintw(win, fmt, &args);
}
/*
 *	This routine actually executes the printf and adds it to the window
 *
 *	This is really a modified version of "sprintf".  As such,
 * it assumes that sprintf interfaces with the other printf functions
 * in a certain way.  If this is not how your system works, you
 * will have to modify this routine to use the interface that your
 * "sprintf" uses.
 */
_sprintw(win, fmt, args)
WINDOW	*win;
char	*fmt;
int	*args; {

	FILE	junk;
	char	buf[512];

	junk._flags = __SWR + __SSTR;
	junk._bf._base = junk._p = (unsigned char *)buf;
	junk._bf._size = junk._w = 32767;
	__svfprintf(&junk, fmt, (char *)args);
	putc('\0', &junk);
	return waddstr(win, buf);
}
