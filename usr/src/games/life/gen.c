#ifdef SCCS
static char *sccsid = "@(#)gen.c	1.4	2/2/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "life.h"

char	rules[18] = {0,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0};	/* life rules */


/*
 * Compute one full generation of the current configuration.
 * This is done by calling computerow with each triple of rows which
 * contain any live cells, and remembering the result.  When all rows
 * are finished, we change the object.
 */
dogeneration(obj)
	struct	object	*obj;			/* object to compute */
{
	register struct	row	*prp;		/* previous row */
	register struct	row	*crp;		/* current row */
	register struct	row	*nrp;		/* next row */
	register struct	row	*newrp;		/* current row of new list */
	register struct	row	*srp;		/* saved row pointer */
	int	row;				/* current row number */

	if (genleft <= 0) return;
	if ((--genleft == 0) || (--freqcount <= 0)) redraw = 1;
	obj->o_gen++;
	obj->o_born = 0;
	obj->o_died = 0;
	newrp = &initrow;
	prp = termrow;
	crp = termrow;
	nrp = obj->o_firstrow;
	srp = nrp->r_next;
	row = nrp->r_row - 1;
	while (1) {			/* loop over each triple of rows */
		if (row >= (INFINITY-1)) break;
		prp = computerow(obj, prp, crp, nrp);
		if (prp) {		/* have something in current row */
			newrp->r_next = prp;
			newrp = prp;
			newrp->r_row = row;
			obj->o_count += prp->r_count;
		}
		row++;
		prp = crp;
		crp = nrp;
		nrp = srp;
		if (nrp->r_row == row + 1) {
			srp = nrp->r_next;
			continue;
		}
		nrp = termrow;
		if ((prp != termrow) || (crp != termrow)) continue;
		nrp = srp;
		srp = nrp->r_next;
		row = nrp->r_row - 1;
	}
	zeroobject(obj);
	if (newrp == &initrow) {			/* died off */
		genleft = 0;
		redraw = 1;
		return;
	}
	obj->o_firstrow = initrow.r_next;
	newrp->r_next = termrow;
	obj->o_lastrow = newrp;
	if ((obj->o_born == 0) && (obj->o_died == 0)) {	/* no change */
		genleft = 0;
		redraw = 1;
	}
}


/*
 * Compute the result of three adjacent rows, and return a row structure
 * containing the new middle row, or NULL if no live cells are produced.
 * When determining if a cell is dead or alive, each live neighbor counts
 * as a 1, but the current cell counts as 9 when alive.  Indexing the rules
 * table with the sum then automatically produces the correct result.
 */
struct row *
computerow(obj, prevrow, currow, nextrow)
	struct	object	*obj;
	struct	row	*prevrow, *currow, *nextrow;
{
	register struct	cell	*pcp;	/* head of previous row of cells */
	register struct	cell	*ccp;	/* head of current row of cells */
	register struct	cell	*ncp;	/* head of next row of cells */
	register struct	cell	*tcp;	/* temporary cell */
	register struct	cell	*newcp;	/* new row of cells */
	register int	i;		/* sum of live cells and other uses */
	struct	row	*rp;		/* new row pointer */
	int	col;			/* current column being examined */
	int	colp1;			/* one more than column */
	int	count;			/* live cells */

	pcp = prevrow->r_firstcell;
	ccp = currow->r_firstcell;
	ncp = nextrow->r_firstcell;
	newcp = &initcell;
	count = 0;
	col = -INFINITY;
	while (1) {			/* loop over cells of all 3 rows */
		/*
		 * Find next column where a cell exists on any row
		 */
		i = col - 1;
		while (i > pcp->c_col) pcp = pcp->c_next;
		while (i > ccp->c_col) ccp = ccp->c_next;
		while (i > ncp->c_col) ncp = ncp->c_next;
		i = ccp->c_col;
		if (pcp->c_col < i) i = pcp->c_col;
		if (ncp->c_col < i) i = ncp->c_col;
		if (i == INFINITY) break;
		i--;
		if (col < i) col = i;
		i = 0;
		colp1 = col + 1;
		if (pcp->c_col <= colp1) {	/* add cells in previous row */
			i++;
			tcp = pcp->c_next;
			i += (tcp->c_col <= colp1);
			tcp = tcp->c_next;
			i += (tcp->c_col <= colp1);
		}
		if (ccp->c_col <= colp1) {	/* add cells on our row */
			i++;
			tcp = ccp->c_next;
			if ((ccp->c_col == col) || (tcp->c_col == col)) {
				i += (LIFE - 1);
			}
			i += (tcp->c_col <= colp1);
			tcp = tcp->c_next;
			i += (tcp->c_col <= colp1);
		}
		if (ncp->c_col <= colp1) {	/* add cells in next row */
			i++;
			tcp = ncp->c_next;
			i += (tcp->c_col <= colp1);
			tcp = tcp->c_next;
			i += (tcp->c_col <= colp1);
		}
		if (rules[i]) {			/* cell is alive */
			obj->o_born += (i < LIFE);
			tcp = alloccell();
			tcp->c_col = col;
			newcp->c_next = tcp;
			newcp = tcp;
			count++;
		} else				/* cell is dead */
			obj->o_died += (i >= LIFE);
		col++;
	}
	if (newcp == &initcell) return(NULL);
	newcp->c_next = termcell;
	rp = allocrow();
	rp->r_firstcell = initcell.c_next;
	rp->r_lastcell = newcp;
	rp->r_count = count;
	return(rp);
}
