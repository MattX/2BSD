static char *sccsid = "@(#)main.c	1.12	2/2/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";

/*
 * The game of life on an infinite board (by David I. Bell).
 * These life sources are in the public domain, and can be copied
 * or used as desired, with the following restrictions:
 * 1.	All copyright notices (and this notice) must be preserved.
 * 2.	The life sources (even if modified) cannot be sold for profit.
 * 3.	If any sources are modified, a sentence must exist by the
 *	copyright notice of each modified source file which says that
 *	the file has been modified.
 */

#include "life.h"
#include <sgtty.h>

int	intint(), readchar();		/* routines */
char	*getenv();			/* another one */


main(argc, argv)
	char	**argv;			/* argument is a life object */
{
	userlib = getenv(LIFEVAR);
	strcpy(rulestring, "3,23");
	termcell = alloccell();
	termcell->c_next = termcell;
	termcell->c_col = INFINITY;
	termrow = allocrow();
	termrow->r_next = termrow;
	termrow->r_firstcell = termcell;
	termrow->r_row = INFINITY;
	mode = M_MOVE;
	gridchar = ' ';
	frequency = 1;
	freqcount = 1;
	rowradius = 1;				/* temp until find real sizes */
	colradius = 1;
	reserve = 1;				/* creating special objects */
	deleteobject = getobject("..delete");
	tempobject = getobject("..temp");
	backupobject = getobject("..backup");
	mainobject = getobject("main");
	curobj = mainobject;
	prevobj = mainobject;
	reserve = 0;				/* no more special objects */
	curinput = &inputs[-1];			/* initialize for tty input */
	settty();
	if ((argc > 1) && setfile(argv[1])) {	/* set to get input file */
		perror(argv[1]);
		exit(1);
	}
	if (dpyinit(NULL, CBREAK, ECHO)) {	/* home up and clear screen */
		exit(1);
	}
	dpymove(-1, -1);			/* get screen size */
	rowradius = (dpygetrow() - 1) / 2;
	colradius = (dpygetcol() - 1) / 2;
	setscale(deleteobject, 1);		/* fix scale factors now */
	setscale(tempobject, 1);
	setscale(backupobject, 1);
	setscale(mainobject, 1);
	signal(SIGINT, intint);
	scaninit(readchar, ttyjmp);
	while (1) {
		docommand();
		dogeneration(curobj);
		updateview();
	}
}


/*
 * Here on an interrupt character.  Remember to stop what we are doing soon.
 * We cannot just longjmp away since things may be in an inconsistent state.
 */
intint()
{
	signal(SIGINT, intint);		/* not needed in 4.2, but so what */
	genleft = 0;
	stop = 1;
	redraw = 1;
#ifdef DEBUG
	dumpdata();
#endif DEBUG
}


/*
 * Here on an error.  Close all but the top input level, cancel the
 * current command, beep, and set up to display the indicated message.
 * The message will remain until the next command is typed by the user.
 */
error(str)
	char	*str;		/* message to type */
{
	while (curinput > inputs) curinput->i_term(curinput);
	errorstring = str;
	redraw = 1;
	stop = 0;
	dowait = 0;
	write(STDERR, "\007", 1);
	scanabort();
}
