#ifdef SCCS
static char *sccsid = "@(#)cmd.c	1.21	2/2/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "life.h"


/*
 * Read commands if available, and execute them.  Since we call scanchar for
 * our characters, the code after the setjmp can be reentered many times in
 * order to finish any command.  This allows commands to be typed without
 * stopping the computation of generations of an object, and allows editing
 * of partially completed commands.
 */
docommand()
{
	register int	defarg;		/* first argument defaulted to one */
	register int	ch;		/* character read */
	register struct	object	*obj;	/* object being manipulated */
	int	arg1;			/* first command argument */
	int	arg2;			/* second command argument */
	int	got1;			/* got first argument flag */
	int	got2;			/* got second argument flag */
	char	*saveloopptr;		/* crock for loop definitions */

	switch (setjmp(ttyjmp)) {
		case SCAN_EDIT:		/* command edited before completion */
			curinput->i_endptr = saveloopptr;
			break;
		case SCAN_EOF:		/* not yet enough chars for a command */
			curinput->i_endptr = saveloopptr;
			return;
		case SCAN_ABORT:	/* normal command completion */
		default:		/* normal entry point */
			saveloopptr = curinput->i_endptr;
			break;
	}
	if (stop) error("Command aborted");
	obj = curobj;
	cmark = 0;
	arg2 = 0;
	got2 = 0;
	ch = readvalue(&arg1, &got1);
	if (ch == ',') ch = readvalue(&arg2, &got2);
	defarg = 1;
	if (got1) defarg = arg1;
	switch (ch) {
#ifdef DEBUG
		case '\005':		/* ^E - show debugging info */
			dumpdata();
			break;
#endif DEBUG
		case '\004':		/* ^D - end terminal input */
			curinput->i_term(curinput);
			break;
		case '\014':		/* refresh screen */
			dpyredraw();
			redraw = 1;
			break;
		case '\n':		/* move to next "line" */
			crow += defarg;
			ccol = pcol;
			update = 1;
			break;
		case '\t':		/* move to next tab stop */
			while ((++ccol - pcol) % 8) ;
			update = 1;
			break;
		case ESC:		/* execute a macro command */
			ch = scanchar();
			if (ch == ESC) break;	/* ignore double escape */
			backup();
			if (setmacro(arg1, arg2, ch)) error("Undefined macro");
			update = 1;
			break;
		case ' ':		/* move to right */
		case '.':
			ccol += defarg;
			update = 1;
			break;
		case ':':		/* execute line style command */
		case ';':
			dolinecommand(arg1, arg2, got1, got2);
			break;
		case '<':		/* begin loop or macro definition */
			if (got1 || got2) {
				if (got2) setloop(defarg, arg2, NULL);
				else setloop(1, defarg, NULL);
				update = 1;
				break;
			}
			ch = scanchar();	/* defining macro */
			if ((ch < 'a') || (ch > 'z')) {
				error("Bad macro character");
			}
			setloop(1, 1, ch);
			update = 1;
			break;
		case '>':		/* end loop */
			endloop();
			break;
		case '+':		/* increment single char variable */
			ch = scanchar();
			if (ch == '$') ch = scanchar();
			setvariable1(ch, getvariable1(ch) + defarg);
			break;
		case 'b':		/* move lower left with action */
			domove(defarg, -defarg);
			break;
		case 'B':		/* shift to lower left */
			doshift(defarg, -defarg);
			break;
		case 'c':		/* pick cell as current location */
			crow = prow + arg1;
			ccol = pcol + arg2;
			update = 1;
			break;
		case 'd':		/* delete selection */
			doselect(ch);
			backup();
			movemarkedobject(obj, deleteobject, MARK_CMD);
			redraw = 1;
			break;
		case 'f':		/* flip selection */
			doselect(ch);
			cmark = MARK_USR;
			flipmarkedobject(obj, MARK_CMD);
			redraw = 1;
			break;
		case 'g':		/* compute generations */
			if (obj->o_lock) error("Object locked");
			if (genleft <= 0) backup();
			genleft = (genleft ? arg1 : defarg);
			freqcount = frequency;
			update = 1;
			break;
		case 'G':		/* compute infinite generations */
			if (obj->o_lock) error("Object locked");
			if (genleft <= 0) backup();
			genleft = INFINITY;
			freqcount = frequency;
			update = 1;
			break;
		case 'h':		/* move left with action */
			domove(0, -defarg);
			break;
		case 'H':		/* shift left lots */
			doshift(0, -defarg);
			break;
		case 'i':		/* toggle insert mode */
			checkrun();
			if (mode == M_MOVE) mode = M_INSERT;
			else if (mode == M_INSERT) mode = M_DELETE;
			else mode = M_MOVE;
			update = 1;
			break;
		case 'j':		/* move down with action */
			domove(defarg, 0);
			break;
		case 'J':		/* shift down lots */
			doshift(defarg, 0);
			break;
		case 'k':		/* move up with action */
			domove(-defarg, 0);
			break;
		case 'K':		/* shift up lots */
			doshift(-defarg, 0);
			break;
		case 'l':		/* move right with action */
			domove(0, defarg);
			break;
		case 'L':		/* shift right lots */
			doshift(0, defarg);
			break;
		case 'm':		/* mark current object */
			doselect(ch);
			copymarks(obj, MARK_CMD, MARK_USR);
			redraw = 1;
			break;
		case 'M':		/* remove all marks */
			checkrun();
			clearmarks(obj, MARK_SEE);
			redraw = 1;
			break;
		case 'n':		/* move lower right with action */
			domove(defarg, defarg);
			break;
		case 'N':		/* shift down and right */
			doshift(defarg, defarg);
			break;
		case 'o':		/* insert new cells */
		case 'O':
			checkrun();
			backup();
			while ((stop == 0) && (defarg-- > 0)) {
				addcell(obj, crow, ccol++);
			}
			redraw = 1;
			break;
		case 'p':		/* place deleted object */
			checkrun();
			backup();
			cmark = MARK_USR;
			addobject(deleteobject, obj, RELATIVE);
			redraw = 1;
			break;
		case 'r':		/* rotate selection */
			doselect(ch);
			cmark = MARK_USR;
			rotatemarkedobject(obj, MARK_CMD);
			redraw = 1;
			break;
		case 's':		/* set scale factor and center object */
			obj->o_autoscale = 0;
			if (got1 == 0) arg1 = obj->o_scale;
			setscale(obj, arg1);
			break;
		case 'S':		/* perform auto-scaling */
			if (got1 == 0) arg1 = obj->o_scale;
			setscale(obj, arg1);
			obj->o_autoscale = 1;
			redraw = 1;
			break;
		case 't':		/* toggle current cell */
			checkrun();
			backup();
			if (delcell(obj, crow, ccol))
				addcell(obj, crow, ccol);
			redraw = 1;
			break;
		case 'u':		/* move upper right with action */
			domove(-defarg, defarg);
			break;
		case 'U':		/* shift to upper right */
			doshift(-defarg, defarg);
			break;
		case 'x':		/* kill current cells */
			checkrun();
			backup();
			while ((stop == 0) && (defarg-- > 0)) {
				delcell(obj, crow, ccol++);
			}
			redraw = 1;
			break;
		case 'y':		/* move upper left with action */
			domove(-defarg, -defarg);
			break;
		case 'Y':		/* shift to upper left */
			doshift(-defarg, -defarg);
			break;
		case 'z':		/* clear or set generation number */
			checkrun();
			obj->o_gen = arg1;
			obj->o_born = 0;
			obj->o_died = 0;
			update = 1;
			break;
		case '/':		/* search for next object */
			{
			int minr, maxr, minc, maxc;

			checkrun();
			if (searchobject(obj, defarg, 0)) error("Empty object");
			clearmarks(obj, MARK_CMD);
			markobject(obj, crow, ccol, MARK_CMD);
			markminmax(obj, MARK_CMD, &minr, &maxr, &minc, &maxc);
			positionview(minr, maxr, minc, maxc);
			update = 1;
			}
			break;
		case '@':		/* point at current location */
			prow = crow;
			pcol = ccol;
			break;
		case '!':		/* comment characters */
		case '#':
			while (scanchar() != '\n') ;
			break;
		default:		/* unknown commands */
			error("Unknown command");
	}
	scanabort();			/* completed command */
}


/*
 * Read a numeric value (if any) to be used as an argument for a command.
 * Pointers to the returned value and returned flag are given.
 * The returned value is zero if no value is read.
 * The returned flag is nonzero if a value was read.
 * Return value is the first non-argument character read.
 */
readvalue(valueptr, flagptr)
	register int	*valueptr;	/* pointer to returned value */
	register int	*flagptr;	/* pointer to got value flag */
{
	register int	ch;		/* character being read */
	register struct	input	*ip;	/* input structure */
	int	sign;			/* sign of result */

	*valueptr = 0;
	*flagptr = 0;
	sign = 1;
	ch = scanchar();
	if (ch == '-') {			/* negative value */
		sign = -1;
		ch = scanchar();
	}
	if (ch == '$') {			/* get variable value */
		*valueptr = sign * getvariable1(scanchar());
		*flagptr = 1;
		return(scanchar());
	}
	if (ch == '(') {			/* get expression */
		*valueptr = sign * scanexpr();
		*flagptr = 1;
		return(scanchar());
	}
	while ((ch >= '0') && (ch <= '9')) {	/* get numeric value */
		*valueptr = (*valueptr * 10) + ch - '0';
		*flagptr = 1;
		ch = scanchar();
	}
	if (ch == '%') {			/* get loop value */
		ch = 1;
		if (*flagptr) ch = *valueptr;
		if (ch <= 0) error("Bad nest value");
		ip = curinput + 1;
		while (ch > 0) {
			if (--ip < inputs) error("Bad nest value");
			if (ip->i_type == INP_LOOP) ch--;
		}
		*valueptr = ip->i_curval;
		*flagptr = 1;
		ch = scanchar();
	}
	*valueptr *= sign;
	return(ch);
}


/*
 * Routine called from above to scan and evaluate a parenthesized expression.
 * This routines knows that one parenthesis has already been read.  Stops
 * reading on the matching parenthesis.
 */
scanexpr()
{
	register char	*cp;		/* current character */
	register int	nest;		/* nesting depth */
	char	buf[100];		/* expression buffer */

	cp = buf;
	*cp++ = '(';			/* start with parenthesis */
	nest = 1;
	while (nest > 0) {
		if (cp >= &buf[sizeof(buf)-2]) error("expression too long");
		*cp = scanchar();
		if (*cp == '(') nest++;
		if (*cp == ')') nest--;
		cp++;
	}
	*cp = '\0';
	return(getexpression(buf));
}


/*
 * Select a set of cells to be used for some command.  This involves reading
 * the next character and marking cells based on that character.  Repeating
 * the command character is equivilant to selecting the current object.  On a
 * successful return, exactly those cells specified are marked with MARK_CMD.
 * If no cells are found, an error is generated.
 */
doselect(cmd)
{
	register struct	object	*obj;	/* object to examine */
	register long	minrow, maxrow, mincol, maxcol;	/* range for marks */
	register struct	cell	*cp;	/* current cell */
	int	ch;			/* character to select on */

	checkrun();
	ch = scanchar();
	if (ch == cmd) ch = 'o';	/* repeated char is connected object */
	minrow = -INFINITY;
	maxrow = -minrow;
	mincol = minrow;
	maxcol = maxrow;
	obj = curobj;
	clearmarks(obj, MARK_CMD);
	switch (ch) {
		case 'a':		/* all of object */
			break;
		case 'b':		/* below and left of cursor */
			minrow = crow;
			maxcol = ccol;
			break;
		case 'c':		/* current cell */
			cp = findcell(obj, crow, ccol);
			if (cp == NULL) error("No cell at current location");
			cp->c_marks |= MARK_CMD;
			return;
		case 'h':		/* left of cursor */
			maxcol = ccol;
			break;
		case 'j':		/* below cursor */
			minrow = crow;
			break;
		case 'k':		/* above cursor */
			maxrow = crow;
			break;
		case 'l':		/* right of cursor */
			mincol = ccol;
			break;
		case 'm':		/* marked cells */
			if (copymarks(obj, MARK_USR, MARK_CMD))
				error("No object marked");
			return;
		case 'n':		/* below and right of cursor */
			minrow = crow;
			mincol = ccol;
			break;
		case 'o':		/* connected object */
			if (markobject(obj, crow, ccol, MARK_CMD))
				error("No object at current location");
			return;
		case 'p':		/* rectangle to pointer */
			minrow = crow;
			maxrow = prow;
			if (minrow > maxrow) {
				minrow = prow;
				maxrow = crow;
			}
			mincol = ccol;
			maxcol = pcol;
			if (mincol > maxcol) {
				mincol = pcol;
				maxcol = ccol;
			}
			break;
		case 'u':		/* above and right of cursor */
			maxrow = crow;
			mincol = ccol;
			break;
		case 'v':		/* things visible in window */
			minrow = obj->o_minrow;
			maxrow = obj->o_maxrow;
			mincol = obj->o_mincol;
			maxcol = obj->o_maxcol;
			break;
		case 'y':		/* above and left of cursor */
			maxrow = crow;
			maxcol = ccol;
			break;
		default:		/* unknown */
			error("Unknown selection command");
	}
	if (markregion(obj, MARK_CMD, minrow, maxrow, mincol, maxcol) == 0)
		error("No cells in region");
}


/*
 * Move the current position by the indicated deltas, performing the
 * current action to the configuration.  The movement is scaled by
 * the current scaling factor.
 */
domove(rowdelta, coldelta)
	register int	rowdelta;	/* amount to change row by */
	register int	coldelta;	/* amount to change column by */
{
	register int	row1;		/* increment for row */
	register int	col1;		/* increment for column */

	rowdelta *= curobj->o_scale;
	coldelta *= curobj->o_scale;
	if (mode == M_MOVE) {		/* just want to move */
		crow += rowdelta;
		ccol += coldelta;
		update = 1;
		return;
	}
	checkrun();
	backup();
	row1 = 0;			/* need to loop for insert or delete */
	col1 = 0;
	if (rowdelta > 0) row1 = 1;
	if (rowdelta < 0) row1 = -1;
	if (coldelta > 0) col1 = 1;
	if (coldelta < 0) col1 = -1;
	rowdelta += crow;
	coldelta += ccol;
	while ((stop == 0) && ((crow != rowdelta) || (ccol != coldelta))) {
		crow += row1;
		ccol += col1;
		switch (mode) {
			case M_INSERT:
				addcell(curobj, crow, ccol);
				break;
			case M_DELETE:
				delcell(curobj, crow, ccol);
		}
	}
	redraw = 1;
}


/*
 * Shift the window lots in the indicated direction, and also shift the
 * cursor location the same amount so that the cursor location on the
 * screen doesn't change.  "Lots" is 1/4 of the screen width or height.
 * Special case: if both x and y are being shifted, we shift both by the
 * same amount, which is the minimum of the two shifts.
 */
doshift(rowdelta, coldelta)
	register int	rowdelta;	/* amount to change row by */
	register int	coldelta;	/* amount to change column by */
{
	register struct	object	*obj;	/* current object */
	register int	rowsign;	/* sign of row */
	register int	colsign;	/* sign of column */

	obj = curobj;
	rowdelta *= ((rowradius * obj->o_scale) / 2);
	coldelta *= ((colradius * obj->o_scale) / 2);
	if (rowdelta && coldelta) {	/* take minimums of absolute values */
		rowsign = 1;
		colsign = 1;
		if (rowdelta < 0) {
			rowsign = -1;
			rowdelta = -rowdelta;
		}
		if (coldelta < 0) {
			colsign = -1;
			coldelta = -coldelta;
		}
		if (rowdelta > coldelta) rowdelta = coldelta;
		if (coldelta > rowdelta) coldelta = rowdelta;
		rowdelta *= rowsign;
		coldelta *= colsign;
	}
	obj->o_currow += rowdelta;
	obj->o_minrow += rowdelta;
	obj->o_maxrow += rowdelta;
	obj->o_curcol += coldelta;
	obj->o_mincol += coldelta;
	obj->o_maxcol += coldelta;
	redraw = 1;
}


/*
 * Perform a backup of the current object.  This is only done if reading
 * from the terminal, since it is from the point of view of the user.
 */
backup()
{
	if (curinput->i_type == INP_TTY) copyobject(curobj, backupobject);
}


/*
 * Check to see that generations are not being computed before proceeding
 * with the current command.
 */
checkrun()
{
	if (genleft > 0) error("Illegal while running");
}
