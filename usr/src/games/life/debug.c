#ifdef SCCS
static char *sccsid = "@(#)debug.c	1.2	1/14/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#ifdef DEBUG
#include "life.h"

/*
 * Subroutine to dump out all object data structures for debugging.
 */
dumpdata()
{
	register struct	object	*obj;

	printf("\ntermrow %x termcell %x\n", termrow, termcell);
	for (obj = objects; obj; obj = obj->o_next) {
		dumpobj(obj);
	}
	printf("done\n");
}


/*
 * Dump out an object.
 */
dumpobj(obj)
	register struct	object	*obj;
{
	register struct	row	*rp;

	printf("object %s (%x) firstrow %x lastrow %x\n", 
		obj->o_name, obj, obj->o_firstrow, obj->o_lastrow);
	rp = obj->o_firstrow;
	while (1) {
		dumprow(rp);
		if (rp == termrow) break;
		rp = rp->r_next;
	}
}


/*
 * Dump out a row
 */
dumprow(rp)
	register struct	row	*rp;
{
	register struct	cell	*cp;

	if (rp == termrow) {
		printf("  termrow\n");
		return;
	}
	printf("  row %d (%x) firstcell %x lastcell %x\n",
		rp->r_row, rp, rp->r_firstcell, rp->r_lastcell);
	cp = rp->r_firstcell;
	while (1) {
		dumpcell(cp);
		if (cp == termcell) break;
		cp = cp->c_next;
	}
}


/*
 * Dump out a cell
 */
dumpcell(cp)
	register struct	cell	*cp;
{
	if (cp == termcell) {
		printf("    termcell\n");
		return;
	}
	printf("    cell %d (%x)\n", cp->c_col, cp);
}

#endif DEBUG
