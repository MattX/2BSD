#ifdef SCCS
static char *sccsid = "@(#)cell.c	1.3	2/1/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "life.h"


/*
 * Return a row structure for a given row number, returning NULL if not there.
 */
struct row *
findrow(obj, row)
	struct	object	*obj;
	register int	row;
{
	register struct	row	*rp;

	for (rp = obj->o_firstrow; (row > rp->r_row); rp = rp->r_next) ;
	if (rp->r_row != row) return(NULL);
	return(rp);
}


/*
 * Return a row structure for a given row number, creating it if needed.
 */
struct row *
getrow(obj, row)
	register struct	object	*obj;
	register int	row;
{
	register struct	row	*rp;		/* current row */
	register struct	row	*nrp;		/* next row */
	register struct	row	*prp;		/* previous row */

	rp = obj->o_firstrow;
	if (row < rp->r_row) {			/* at front */
		nrp = allocrow();
		nrp->r_row = row;
		nrp->r_next = obj->o_firstrow;
		obj->o_firstrow = nrp;
		if (nrp->r_next == termrow) obj->o_lastrow = nrp;
		return(nrp);
	}
	if (row >= obj->o_lastrow->r_row) rp = obj->o_lastrow;
	while (row > rp->r_row) {
		prp = rp;
		rp = rp->r_next;
	}
	if (row == rp->r_row) {
		return(rp);
	}
	nrp = allocrow();
	nrp->r_row = row;
	nrp->r_next = rp;
	prp->r_next = nrp;
	if (nrp->r_next == termrow) obj->o_lastrow = nrp;
	return(nrp);
}


/*
 * Find a cell given its coordinates, returning NULL if not found.
 */
struct cell *
findcell(obj, row, col)
	register struct object	*obj;
	register int	col;
{
	register struct	row	*rp;
	register struct	cell	*cp;

	rp = findrow(obj, row);
	if (rp == NULL) return(NULL);
	for (cp = rp->r_firstcell; col > cp->c_col; cp = cp->c_next) ;
	if (col != cp->c_col) return(NULL);
	return(cp);
}


/*
 * Create a cell at a given row and column.  Returns nonzero if the cell
 * already existed.  If the cell is new, it is marked with the current
 * mark value of the object.
 */
addcell(obj, row, col)
	struct	object	*obj;
	register int	col;
{
	register struct	row	*rp;
	register struct	cell	*cp;		/* current cell */
	register struct	cell	*ncp;		/* next cell */
	register struct	cell	*pcp;		/* previous cell */

	rp = getrow(obj, row);
	cp = rp->r_firstcell;
	if ((cp != termcell) && (col >= rp->r_lastcell->c_col)) {
		pcp = rp->r_lastcell;			/* at end */
		if (col == pcp->c_col) return(1);
		ncp = alloccell();
		ncp->c_col = col;
		ncp->c_marks |= obj->o_mark;
		ncp->c_next = termcell;
		pcp->c_next = ncp;
		rp->r_lastcell = ncp;
		rp->r_count++;
		obj->o_count++;
		return(0);
	}
	if (col < cp->c_col) {
		ncp = alloccell();			/* at front */
		ncp->c_col = col;
		ncp->c_marks |= obj->o_mark;
		ncp->c_next = cp;
		rp->r_firstcell = ncp;
		if (cp == termcell) rp->r_lastcell = ncp;
		rp->r_count++;
		obj->o_count++;
		return(0);
	}
	while (col > cp->c_col) {			/* in middle */
		pcp = cp;
		cp = pcp->c_next;
	}
	if (col == cp->c_col) {
		return(1);
	}
	ncp = alloccell();
	ncp->c_col = col;
	ncp->c_marks |= obj->o_mark;
	ncp->c_next = cp;
	pcp->c_next = ncp;
	if (cp == termcell) rp->r_lastcell = ncp;
	rp->r_count++;
	obj->o_count++;
	return(0);
}


/*
 * Delete a cell at a given coordinate.  Returns nonzero if it did not exist.
 */
delcell(obj, row, col)
	register struct	object	*obj;
	register int	col;
{
	register struct	row	*rp;
	register struct	cell	*pcp;		/* previous cell */
	register struct	cell	*cp;		/* current cell */

	rp = findrow(obj, row);
	if (rp == NULL) return(1);
	pcp = NULL;
	cp = rp->r_firstcell;
	while (col > cp->c_col) {
		pcp = cp;
		cp = cp->c_next;
	}
	if (col != cp->c_col) return(1);
	if (pcp)
		pcp->c_next = cp->c_next;
	else
		rp->r_firstcell = cp->c_next;
	if (cp->c_next == termcell) rp->r_lastcell = pcp;
	cp->c_next = freecells;
	freecells = cp;
	rp->r_count--;
	obj->o_count--;
	return(0);
}
