#ifdef SCCS
static char *sccsid = "@(#)mark.c	1.7	2/2/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "life.h"

struct	loc	{		/* structure to hold row and column pairs */
	long	l_row;
	long	l_col;
};

char	rowwalk[9] = {-1,0,0,1,1,0,0,-1,0};	/* row deltas to walk a cell */
char	colwalk[9] = {-1,1,1,0,0,-1,-1,0,1};	/* col deltas to walk a cell */


/*
 * Mark the object at a given location as specified.  Returns nonzero if
 * no object exists there.  An object for this purpose is considered as a
 * king-wise connected set of live cells.
 */
markobject(obj, row, col, mark)
	register struct	object	*obj;	/* object to mark up */
{
	register struct	cell	*cp;	/* object being marked */

	cp = findcell(obj, row, col);
	if (cp == NULL) return(1);
	cp->c_marks |= mark;
	markloop(obj, row, col, mark);
	return(0);
}


/* Recursive subroutine called from markobject */
markloop(obj, row, col, mark)
	struct	object	*obj;		/* object begin marked */
	register int	row;		/* current row */
	register int	col;		/* current column */
	register int	mark;		/* marking value */
{
	register struct	cell	*cp;	/* current cell */
	register struct	loc	*rp;	/* pointer into list table */
	int	i;			/* to iterate over directions */
	struct	loc	rclist[8];	/* row and column list */

	while (1) {
		if (stop) return;
		rp = rclist;
		for (i = 0; i < 8; i++) {	/* find neighbors */
			row += rowwalk[i];
			col += colwalk[i];
			cp = findcell(obj, row, col);
			if (cp == NULL) continue;
			if (cp->c_marks & mark) continue;
			cp->c_marks |= mark;
			rp->l_row = row;
			rp->l_col = col;
			rp++;
		}
		if (--rp != rclist) {		/* recurse if more than one */
			for (; rp >= rclist; rp--) {
				markloop(obj, rp->l_row, rp->l_col, mark);
			}
			return;
		}
		row = rp->l_row;		/* else follow single cell */
		col = rp->l_col;
	}
}


/* Mark a whole object as specified. */
setmarks(obj, mark)
	struct	object	*obj;		/* object to mark */
	register int	mark;		/* mark to be applied */
{
	register struct	row	*rp;	/* row pointer */
	register struct	cell	*cp;	/* cell pointer */

	mark |= MARK_ANY;
	for (rp = obj->o_firstrow; rp != termrow; rp = rp->r_next) {
		for (cp = rp->r_firstcell; cp != termcell; cp = cp->c_next) {
			cp->c_marks |= mark;
		}
	}
}


/*
 * Copy the marks from one type to another for an object.  Returns nonzero
 * if there were no cells with the given mark.
 */
copymarks(obj, srcmark, destmark)
	struct	object	*obj;		/* object to mark */
	register int	srcmark;	/* mark to be found */
	register int	destmark;	/* mark to be set */
{
	register struct	row	*rp;	/* row pointer */
	register struct	cell	*cp;	/* cell pointer */
	int	error;

	error = 1;
	for (rp = obj->o_firstrow; rp != termrow; rp = rp->r_next) {
		for (cp = rp->r_firstcell; cp != termcell; cp = cp->c_next) {
			if ((cp->c_marks & srcmark) == 0) continue;
			cp->c_marks |= destmark;
			error = 0;
		}
	}
	return(error);
}


/* Clear marks for a whole object as specified. */
clearmarks(obj, mark)
	struct	object	*obj;		/* object to mark */
	register int	mark;		/* mark to be cleared */
{
	register struct	row	*rp;	/* row pointer */
	register struct	cell	*cp;	/* cell pointer */

	mark = ~mark;
	mark |= MARK_ANY;
	for (rp = obj->o_firstrow; rp != termrow; rp = rp->r_next) {
		for (cp = rp->r_firstcell; cp != termcell; cp = cp->c_next) {
			cp->c_marks &= mark;
		}
	}
}


/*
 * Mark the cells in a specified rectangular region as desired.
 * Returns the number of cells which were marked.
 */
markregion(obj, mark, minrow, maxrow, mincol, maxcol)
	struct	object	*obj;			/* object to mark */
	register int	mark;			/* value to mark with */
	long	minrow, maxrow, mincol, maxcol;	/* range to mark */
{
	register struct	row	*rp;		/* current row */
	register struct	cell	*cp;		/* current cell */
	register long	count;			/* count of cells */

	count = 0;
	for (rp = obj->o_firstrow; rp != termrow; rp = rp->r_next) {
		if (rp->r_row < minrow) continue;
		if (rp->r_row > maxrow) break;
		for (cp = rp->r_firstcell; cp != termcell; cp = cp->c_next) {
			if (cp->c_col < mincol) continue;
			if (cp->c_col > maxcol) break;
			cp->c_marks |= mark;
			count++;
		}
	}
	return(count);
}


/*
 * Find the range of all marked cells for an object.  Returns nonzero if
 * no cells were marked.
 */
markminmax(obj, mark, minrow, maxrow, mincol, maxcol)
	struct	object	*obj;		/* object to search */
	register int	mark;		/* mark to search for */
	long	*minrow, *maxrow, *mincol, *maxcol;	/* results */
{
	register struct	row	*rp;	/* current row */
	register struct	cell	*cp;	/* current cell */
	register int	row;		/* current row */
	long	minr, maxr, minc, maxc;	/* temp variables */

	minr = INFINITY;
	maxr = -INFINITY;
	minc = INFINITY;
	maxc = -INFINITY;
	for (rp = obj->o_firstrow; rp != termrow; rp = rp->r_next) {
		row = rp->r_row;
		for (cp = rp->r_firstcell; cp != termcell; cp = cp->c_next) {
			if ((cp->c_marks & mark) == 0) continue;
			if (row < minr) minr = row;
			if (row > maxr) maxr = row;
			if (cp->c_col < minc) minc = cp->c_col;
			if (cp->c_col > maxc) maxc = cp->c_col;
		}
	}
	if (minr > maxr) return(1);
	*minrow = minr;
	*maxrow = maxr;
	*mincol = minc;
	*maxcol = maxc;
	return(0);
}


/* Count the number of marked cells in an object */
countmarks(obj, mark)
	struct	object	*obj;		/* object to check */
	register int	mark;		/* mark to be counted */
{
	register struct	row	*rp;	/* row pointer */
	register struct	cell	*cp;	/* cell pointer */
	register long	count;		/* number of cells */

	count = 0;
	for (rp = obj->o_firstrow; rp != termrow; rp = rp->r_next) {
		for (cp = rp->r_firstcell; cp != termcell; cp = cp->c_next) {
			if (cp->c_marks & mark) count++;
		}
	}
	return(count);
}


/*
 * Move marked cells to another object.  If the destination object is NULL
 * the cells are just deleted.  The previous cells of the destination object
 * are deleted.
 */
movemarkedobject(sobj, dobj, mark)
	register struct	object	*sobj;	/* source object */
	struct	object	*dobj;		/* destination object */
{
	register struct	row	*rp;	/* row pointer */
	register struct	cell	*cp;	/* current cell pointer */
	register struct	cell	*pcp;	/* previous cell pointer */
	register struct	cell	*ncp;	/* next cell pointer */

	if (sobj == dobj) error("Moving object to itself");
	if (dobj) {
		zeroobject(dobj);
		dobj->o_currow = sobj->o_currow;
		dobj->o_curcol = sobj->o_curcol;
	}
	for (rp = sobj->o_firstrow; rp != termrow; rp = rp->r_next) {
		pcp = NULL;
		cp = rp->r_firstcell;
		while (cp != termcell) {
			if ((cp->c_marks & mark) == 0) {
				pcp = cp;
				cp = cp->c_next;
				continue;
			}
			if (dobj) addcell(dobj, rp->r_row, cp->c_col);
			ncp = cp->c_next;
			if (pcp == NULL)
				rp->r_firstcell = ncp;
			else
				pcp->c_next = ncp;
			if (ncp == termcell) rp->r_lastcell = pcp;
			cp->c_next = freecells;
			freecells = cp;
			cp = ncp;
			rp->r_count--;
			sobj->o_count--;
		}
	}
}


/*
 * Copy marked cells to another object. The previous cells of the destination
 * object are deleted.  The source object is unaffected.
 */
copymarkedobject(sobj, dobj, mark)
	register struct	object	*sobj;	/* source object */
	register struct	object	*dobj;	/* destination object */
	register int	mark;		/* mark value */
{
	register struct	row	*rp;	/* row pointer */
	register struct	cell	*cp;	/* current cell */

	if (sobj == dobj) error("Copying object to itself");
	zeroobject(dobj);
	dobj->o_currow = sobj->o_currow;
	dobj->o_curcol = sobj->o_curcol;
	for (rp = sobj->o_firstrow; rp != termrow; rp = rp->r_next) {
		for (cp = rp->r_firstcell; cp != termcell; cp = cp->c_next) {
			if ((cp->c_marks & mark) == 0) continue;
			addcell(dobj, rp->r_row, cp->c_col);
		}
	}
}


/*
 * Rotate the marked cells in the given object around the current cursor
 * location.  This deletes the marked cells, and reinserts them after
 * their position has been rotated by 90 degrees clockwise.
 */
rotatemarkedobject(obj, mark)
	register struct	object	*obj;		/* current object */
{
	register struct	row	*rp;		/* current row */
	register struct	cell	*cp;		/* current cell */
	register int	row, col;		/* current row and column */

	if (obj == tempobject) error("Using temp object");
	movemarkedobject(obj, tempobject, mark);
	for (rp = tempobject->o_firstrow; rp != termrow; rp = rp->r_next) {
		row = rp->r_row - obj->o_currow;
		for (cp = rp->r_firstcell; cp != termcell; cp = cp->c_next) {
			col = cp->c_col - obj->o_curcol;
			addcell(obj, obj->o_currow + col, obj->o_curcol - row);
		}
	}
}


/*
 * Flip the marked cells in the given object around the column of the current
 * cursor location.  This deletes the marked cells, and reinserts them after
 * their position has been flipped around the vertical axis.
 */
flipmarkedobject(obj, mark)
	register struct	object	*obj;		/* current object */
{
	register struct	row	*rp;		/* current row */
	register struct	cell	*cp;		/* current cell */
	register int	row, col;		/* current row and column */

	if (obj == tempobject) error("Using temp object");
	movemarkedobject(obj, tempobject, mark);
	for (rp = tempobject->o_firstrow; rp != termrow; rp = rp->r_next) {
		row = rp->r_row;
		for (cp = rp->r_firstcell; cp != termcell; cp = cp->c_next) {
			col = cp->c_col - obj->o_curcol;
			addcell(obj, row, obj->o_curcol - col);
		}
	}
}
