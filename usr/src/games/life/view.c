#ifdef SCCS
static char *sccsid = "@(#)view.c	1.5	2/2/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "life.h"


/*
 * Set the scaling factor for the specified object.  This also centers
 * the view around the current cursor location.
 */
setscale(obj, sf)
	register struct	object	*obj;	/* object to set scale of */
	register int	sf;		/* scaling factor */
{
	if (sf <= 0) sf = 1;
	if (sf > MAXSCALE) sf = MAXSCALE;
	obj->o_scale = sf;
	obj->o_minrow = obj->o_currow - (rowradius * sf) + (sf / 2);
	obj->o_maxrow = obj->o_minrow + (2 * rowradius * sf);
	obj->o_mincol = obj->o_curcol - (colradius * sf) + (sf / 2);
	obj->o_maxcol = obj->o_mincol + (2 * colradius * sf);
	if (obj == curobj) redraw = 1;		/* update if object visible */
}


/*
 * Perform auto-scaling of the current object.  This implies picking a
 * scaling factor such that the whole object fits in the screen.  The
 * scale factor is never decreased.  When the scale factor is large,
 * convenient ones are picked.  Returns the new scale factor.
 */
autoscale()
{
	register struct	object	*obj;		/* current object */
	register int	sf;			/* scaling factor */
	int	minrow, maxrow, mincol, maxcol;	/* limits of object */

	obj = curobj;
	minmax(obj, &minrow, &maxrow, &mincol, &maxcol);
	sf = obj->o_scale;
	if (mincol > maxcol) return(sf);
	while ((sf <= MAXSCALE) &&
		((minrow < obj->o_minrow) || (maxrow > obj->o_maxrow) ||
		(mincol < obj->o_mincol) || (maxcol > obj->o_maxcol))) {
			sf++;
			if (sf > 20) sf += (5 - (sf % 5));
			if (sf > 50) sf += (10 - (sf % 10));
			if (sf > 200) sf += (100 - (sf % 100));
			setscale(obj, sf);
	}
	return(obj->o_scale);
}


/*
 * Position the view of the current object to show both the given region and
 * the current cursor location.  If this is impossible, just the cursor
 * location will be positioned.
 */
positionview(minrow, maxrow, mincol, maxcol)
	register long	minrow, maxrow, mincol, maxcol;	/* region to show */
{
	register struct	object	*obj;	/* current object */
	register int	sf;		/* current scale factor */

	obj = curobj;
	sf = obj->o_scale;
	if (minrow > obj->o_currow) minrow = obj->o_currow;
	if (maxrow < obj->o_currow) maxrow = obj->o_currow;
	if (mincol > obj->o_curcol) mincol = obj->o_curcol;
	if (maxcol < obj->o_curcol) maxcol = obj->o_curcol;
	if ((maxrow - minrow) > (2 * sf * rowradius)) {	/* too many rows */
		minrow = obj->o_currow;
		maxrow = obj->o_currow;
	}
	if ((maxcol - mincol) > (2 * sf * colradius)) {	/* too many columns */
		mincol = obj->o_curcol;
		maxcol = obj->o_curcol;
	}
	if (minrow < obj->o_minrow) {
		obj->o_minrow = minrow;
		obj->o_maxrow = minrow + (rowradius * sf * 2) + sf - 1;
		redraw = 1;
	}
	if (maxrow > obj->o_maxrow) {
		obj->o_maxrow = maxrow;
		obj->o_minrow = maxrow - (rowradius * sf * 2) + sf - 1;
		redraw = 1;
	}
	if (mincol < obj->o_mincol) {
		obj->o_mincol = mincol;
		obj->o_maxcol = mincol + (colradius * sf * 2) + sf  - 1;
		redraw = 1;
	}
	if (maxcol > obj->o_maxcol) {
		obj->o_maxcol = maxcol;
		obj->o_mincol = maxcol - (colradius * sf * 2) + sf - 1;
		redraw = 1;
	}
}


/*
 * Show the view around the current window location if the view has changed.
 * The update flag indicates that the status line and cursor need updating.
 * The redraw flag indicates that the view of the cells also needs updating.
 */
updateview()
{
	register struct	object	*obj;		/* current object */

	if ((interact | redraw | update) == 0) return;
	obj = curobj;
	positionview(obj->o_currow,obj->o_currow,obj->o_curcol,obj->o_curcol);
	if (obj->o_autoscale) autoscale();
	if (redraw) {			/* show visible cells */
		freqcount = frequency;
		dpywindow(1, -1, 0, -1);
		if (obj->o_scale <= 1)
			viewnormal();
		else
			viewscale(obj->o_scale);
		dpyclearwindow();
	}
	if (redraw || update) {		/* show status and position cursor */
		viewstatus();
		dpywindow(1, -1, 0, -1);
		dpymove((obj->o_currow - obj->o_minrow) / obj->o_scale,
			(obj->o_curcol - obj->o_mincol) / obj->o_scale);
		dpyupdate();
	}
	update = 0;			/* no more updates until prodded */
	redraw = 0;
	interact = 0;
}



/*
 * Update the status line for the object.
 */
viewstatus()
{
	register struct	object	*obj;	/* current object */

	dpywindow(0, 0, 0, -1);		/* output in top line */
	if (errorstring) {		/* show error string if present */
		dpystr(errorstring);
		dpyclearline();
		return;
	}
	obj = curobj;
	dpyprintf("Gen:%d cells:%d", obj->o_gen, obj->o_count);
	if (obj->o_count > seecount)
		dpyprintf("(%du)", obj->o_count - seecount);
	if (obj->o_born) dpyprintf(" born:%d", obj->o_born);
	if (obj->o_died) dpyprintf(" died:%d", obj->o_died);
	if (frequency > 1) dpyprintf(" freq:%d", frequency);
	if ((obj->o_scale > 1) || (obj->o_autoscale))
		dpyprintf(" %scale:%d",
			(obj->o_autoscale ? "autos" : "s") , obj->o_scale);
	if (strcmp(rulestring, "3,23")) dpyprintf(" rules:%s", rulestring);
	if (obj->o_lock) dpystr(" locked");
	if (curinput > inputs) dpyprintf(" cmd-nest:%d", curinput - inputs);
	switch (curinput->i_type) {
		case INP_TTY:			/* reading from terminal */
			if (curinput != inputs) dpystr(" tty-wait");
			break;
		case INP_FILE:			/* reading from file */
			dpystr(" cmd-file");
			break;
		case INP_LOOP:			/* reading from loop */
			if (curinput->i_macro) {
				dpyprintf(" macro-define-%c",curinput->i_macro);
				break;
			}
			dpyprintf(" loop%s (curval:%d end:%d)",
				curinput->i_first ? "-define" : "",
				curinput->i_curval, curinput->i_endval);
			break;
		case INP_MACRO:			/* reading from macro */
			dpyprintf(" macro-%c", curinput->i_macro);
			break;
	}
	if (mode == M_INSERT) dpystr(" inserting");
	if (mode == M_DELETE) dpystr(" deleting");
	if (curobj != mainobject) dpyprintf(" \"%s\"", curobj->o_name);
	dpyclearwindow();
}


/* Show the cells around the cursor normally (scale factor of 1) */
viewnormal()
{
	register struct	row	*rp;		/* current row */
	register struct	cell	*cp;		/* current cell */
	register int	row;			/* current row number */
	register int	col;			/* current column number */
	register char	*str;			/* characters for line */
	register char	*endstr;		/* end of characters for line */
	register struct	object	*obj;		/* current object */

	obj = curobj;
	rp = obj->o_firstrow;
	row = obj->o_minrow;
	seecount = 0;
	while (row > rp->r_row) rp = rp->r_next;
	for (; row <= obj->o_maxrow; row++) {
		if (row != rp->r_row) {		/* blank row */
			if (gridchar == ' ') {
				dpychar('\n');
				continue;
			}
			str = stringbuf;
			for (col = obj->o_mincol; col <= obj->o_maxcol; col++) {
				*str++ = gridchar;
			}
			*str++ = '\n';
			dpywrite(stringbuf, str - stringbuf);
			continue;
		}
		str = stringbuf;
		endstr = str;
		cp = rp->r_firstcell;
		col = obj->o_mincol;
		while (col > cp->c_col) cp = cp->c_next;
		for (; col <= obj->o_maxcol; col++) {
			if (col != cp->c_col) {		/* blank cell */
				*str++ = gridchar;
				if (gridchar != ' ') endstr = str;
				continue;
			}
			*str = '#';
			if ((cp->c_marks & MARK_SEE) == 0) *str = 'O';
			endstr = ++str;
			seecount++;
			cp = cp->c_next;
		}
		*endstr++ = '\n';
		dpywrite(stringbuf, endstr - stringbuf);
		rp = rp->r_next;
	}
}


/*
 * Show the view around the cursor with an arbitrary scale factor.
 * When in this mode, characters from 1 to 9 (or * if 10 or more)
 * are used to indicate how many cells are in each n by n square.
 */
viewscale(sf)
	register int	sf;		/* scale factor */
{
	register int	row;		/* current row number */
	register int	col;		/* current column number */
	register int	sum;		/* number of cells in square */
	register struct	cell	*cp;	/* current cell structure */
	register struct	object	*obj;	/* current object */
	struct	cell	**cpp;		/* pointer into cell table */
	struct	cell	**endcpp;	/* end of cell table */
	struct	row	*rp;		/* row pointer */
	char	*str;			/* buffer pointer */
	char	*endstr;		/* end of buffer */
	struct	cell	*cptab[MAXSCALE];	/* table of rows */

	obj = curobj;
	row = obj->o_minrow;
	col = obj->o_mincol;
	endcpp = &cptab[sf];
	seecount = 0;
	for (rp = curobj->o_firstrow; (rp->r_row < row); rp = rp->r_next) ;
	while (row <= obj->o_maxrow) {
		/*
		 * If there is a large gap to the next row number then
		 * the terminal line is empty.
		 */
		if (rp->r_row >= (row + sf)) {		/* no rows here */
			if (gridchar == ' ') {
				dpychar('\n');
				row += sf;
				continue;
			}
			str = stringbuf;
			for (col=obj->o_mincol; col<=obj->o_maxcol; col+=sf) {
				*str++ = gridchar;
			}
			*str++ = '\n';
			dpywrite(stringbuf, str - stringbuf);
			row += sf;
			continue;
		}
		/*
		 * Collect the rows to be searched for one terminal line.
		 * Dummy up empty rows if necessary.
		 */
		for (cpp = cptab; cpp < endcpp; cpp++) {
			*cpp = termcell;
			if (rp->r_row > row++) continue;
			*cpp = rp->r_firstcell;
			rp = rp->r_next;
		}
		str = stringbuf;
		endstr = str;
		/*
		 * Advance along each row to the next range of columns,
		 * adding cells found to get the result for each square.
		 */
		for (col = obj->o_mincol; col <= obj->o_maxcol; col += sf) {
			sum = 0;
			for (cpp = cptab; cpp < endcpp; cpp++) {
				cp = *cpp;
				while (col > cp->c_col) cp = cp->c_next;
				while ((col + sf) >= cp->c_col) {
					sum++;
					cp = cp->c_next;
				}
				*cpp = cp;
			}
			if (sum == 0) {		/* no cells in square */
				*str++ = gridchar;
				if (gridchar != ' ') endstr = str;
				continue;
			}
			*str = '*';		/* show number of cells */
			if (sum <= 9) *str = '0' + sum;
			endstr = ++str;
			seecount += sum;
		}
		*endstr++ = '\n';
		dpywrite(stringbuf, endstr - stringbuf);
	}
}
