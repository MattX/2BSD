/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)talk.h	5.1 (Berkeley) 6/6/85
 */

#ifdef BSD2_10
#define	current_line		c_line
#define	current_state		c_state
#define	daemon_addr		d_addr
#define	daemon_port		d_port
#define	his_machine_name	hs_mnam
#define	his_machine_addr	hs_madd
#define	my_machine_name		my_mnam
#define	my_machine_addr		my_madd
#endif

#include <curses.h>
#include <utmp.h>

#define forever		for(;;)

#define BUF_SIZE	512

FILE	*popen();
int	quit();
int	sleeper();

extern	int sockt;
extern	int curses_initialized;
extern	int invitation_waiting;

extern	char *current_state;
extern	int current_line;

typedef struct xwin {
	WINDOW	*x_win;
	int	x_nlines;
	int	x_ncols;
	int	x_line;
	int	x_col;
	char	kill;
	char	cerase;
	char	werase;
} xwin_t;

extern	xwin_t my_win;
extern	xwin_t his_win;
extern	WINDOW *line_win;
