#ifdef SCCS
static char *sccsid = "@(#)object.c	1.17	2/2/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "life.h"


/*
 * Find the given named object.  Returns NULL if nonexistant.  The special
 * name of "." means the current object.  The special name of ".." means
 * the previous object.
 */
struct object *
findobject(str)
	register char	*str;		/* name to find */
{
	register struct	object	*obj;	/* current object */

	if (str[0] == '.') {		/* check for "." or ".." */
		if (str[1] == '\0') return(curobj);
		if ((str[1] == '.') && (str[2] == '\0')) return(prevobj);
	}
	for (obj = objects; obj; obj = obj->o_next) {
		if (strcmp(obj->o_name, str) == 0) return(obj);
	}
	return(NULL);
}


/* Create the given named object, or return it if it already exists. */
struct object *
getobject(str)
	register char	*str;		/* name to find */
{
	register struct	object	*obj;	/* current object */

	for (obj = objects; obj; obj = obj->o_next) {
		if (strcmp(obj->o_name, str) == 0) return(obj);
	}
	if (strlen(str) > MAXNAME) error("Object name too long");
	if ((reserve==0) && BADNAME(str)) error("Cannot create reserved name");
	obj = allocobject();
	obj->o_next = objects;
	objects = obj;
	strcpy(obj->o_name, str);
	return(obj);
}


/*
 * Set an object as the current one.  The old current object is remembered
 * so that it can be referenced using "..".  This cancels any insert mode.
 */
setobject(obj)
	register struct	object	*obj;		/* new object to set */
{
	mode = M_MOVE;
	if (obj == curobj) return;
	prevobj = curobj;
	curobj = obj;
	redraw = 1;
}


/* Delete all cells of an object */
zeroobject(obj)
	register struct	object	*obj;
{
	register struct	row	*rp;

	rp = obj->o_firstrow;
	if (rp == termrow) return;
	for (; rp != termrow; rp = rp->r_next) {
		if (rp->r_firstcell == termcell) continue;
		rp->r_lastcell->c_next = freecells;
		freecells = rp->r_firstcell;
		obj->o_count -= rp->r_count;
	}
	obj->o_lastrow->r_next = freerows;
	freerows = obj->o_firstrow;
	obj->o_firstrow = termrow;
	obj->o_lastrow = NULL;
}


/*
 * Destroy the existence of an object.  If it is the current object,
 * switch the current object back to the previous object.
 */
destroyobject(obj)
	register struct	object	*obj;		/* object to delete */
{
	register struct	object	*pobj;		/* previous object */

	if (obj == NULL) return;
	if (obj->o_reserved) error("Cannot destroy reserved object");
	if (obj == prevobj) prevobj = mainobject;
	if (obj == curobj) {
		curobj = prevobj;
		prevobj = mainobject;
		redraw = 1;
	}
	zeroobject(obj);
	if (objects == obj) {		/* first object in list */
		objects = obj->o_next;
		obj->o_next = freeobjects;
		freeobjects = obj;
		return;
	}
	for (pobj = objects; pobj->o_next != obj; pobj = pobj->o_next) ;
	pobj->o_next = obj->o_next;
	obj->o_next = freeobjects;
	freeobjects = obj;
}


/*
 * Move one object to another.  The source object is zeroed, and the
 * previous contents of the destination object are lost.
 */
moveobject(sobj, dobj)
	register struct	object	*sobj;		/* source object */
	register struct	object	*dobj;		/* destination object */
{
	if (sobj == dobj) error("Moving object to itself");
	zeroobject(dobj);
	dobj->o_currow = sobj->o_currow;
	dobj->o_curcol = sobj->o_curcol;
	dobj->o_minrow = sobj->o_minrow;
	dobj->o_maxrow = sobj->o_maxrow;
	dobj->o_mincol = sobj->o_mincol;
	dobj->o_maxcol = sobj->o_maxcol;
	dobj->o_scale = sobj->o_scale;
	dobj->o_autoscale = sobj->o_autoscale;
	dobj->o_prow = sobj->o_prow;
	dobj->o_pcol = sobj->o_pcol;
	dobj->o_firstrow = sobj->o_firstrow;
	dobj->o_lastrow = sobj->o_lastrow;
	dobj->o_count = sobj->o_count;
	sobj->o_firstrow = termrow;
	sobj->o_lastrow = NULL;
	sobj->o_count = 0;
}


/*
 * Add one object to another.  The source object is unchanged.  The 
 * destination object will get all cells from both objects.  If disp is
 * RELATIVE, the object is displaced as specified by the two object's cursor
 * positions. Otherwise, the addition is performed with absolute coordinates.
 */
addobject(sobj, dobj, disp)
	register struct	object	*sobj;		/* source object */
	register struct	object	*dobj;		/* destination object */
{
	register struct	row	*rp;		/* current row */
	register struct	cell	*cp;		/* current cell */
	register int	newrow;			/* new row number */
	int	rowdisp, coldisp;		/* displacements */

	if (sobj == dobj) error("Adding object to itself");
	rowdisp = 0;
	coldisp = 0;
	if (disp == RELATIVE) {
		rowdisp = dobj->o_currow - sobj->o_currow;
		coldisp = dobj->o_curcol - sobj->o_curcol;
	}
	for (rp = sobj->o_firstrow; rp != termrow; rp = rp->r_next) {
		newrow = rp->r_row + rowdisp;
		for (cp = rp->r_firstcell; cp != termcell; cp = cp->c_next) {
			addcell(dobj, newrow, cp->c_col + coldisp);
		}
	}
}


/*
 * Copy one object to another.  The source object is unchanged.
 * The current contents of the destination object are lost.
 */
copyobject(sobj, dobj)
	register struct	object	*sobj;		/* source object */
	register struct	object	*dobj;		/* destination object */
{
	if (sobj == dobj) error("Copying object to itself");
	zeroobject(dobj);
	addobject(sobj, dobj, ABSOLUTE);
	dobj->o_currow = sobj->o_currow;
	dobj->o_curcol = sobj->o_curcol;
	dobj->o_minrow = sobj->o_minrow;
	dobj->o_maxrow = sobj->o_maxrow;
	dobj->o_mincol = sobj->o_mincol;
	dobj->o_maxcol = sobj->o_maxcol;
	dobj->o_scale = sobj->o_scale;
	dobj->o_autoscale = sobj->o_autoscale;
	dobj->o_prow = sobj->o_prow;
	dobj->o_pcol = sobj->o_pcol;
}


/*
 * Show the list of objects.  If all is nonzero, all objects will be
 * shown.  Otherwise, only objects not starting with a period are shown.
 */
listobjects(all)
{
	register struct	object	*obj;		/* current object */
	register int	ch;			/* current character */
	int	minrow, maxrow, mincol, maxcol;	/* current bounds */

	dpywindow(0, -1, 0, -1);
	dpystr("cells	height	width	gen	scale	object\n");
	dpystr("-----	------	-----	---	-----	------\n");
	for (obj = objects; obj; obj = obj->o_next) {
		if ((all == 0) && (obj->o_name[0] == '.')) continue;
		ch = ' ';
		if (obj == prevobj) ch = '+';
		if (obj == curobj) ch = '*';
		minmax(obj, &minrow, &maxrow, &mincol, &maxcol);
		dpyprintf("%d\t%d\t%d\t%d\t%d\t%c %s\n",
			obj->o_count, (maxrow - minrow + 1),
			(maxcol - mincol + 1), obj->o_gen,
			obj->o_scale, ch, obj->o_name);
	}
	dpyprintf("\n\
In object column, '*' = current object, '+' = previous object\n");
	if (all == 0)
		dpyprintf("Use -a to show objects beginning with '.'\n");
	spacewait();
}


/*
 * Find the minimum and maximum row and column numbers for an object.
 * If there are no cells in the object, the mins will be one more than
 * the maxes.  Returns nonzero if the object has no cells.
 */
minmax(obj, minrow, maxrow, mincol, maxcol)
	struct	object	*obj;				/* object to examine */
	long	*minrow, *maxrow, *mincol, *maxcol;	/* pointers to result */
{
	register struct	row	*rp;		/* current row */
	register int	maxr, minr, maxc, minc;	/* current results */
	int	err;				/* return value */

	minr = INFINITY;
	maxr = -INFINITY;
	minc = INFINITY;
	maxc = -INFINITY;
	err = 1;
	for (rp = obj->o_firstrow; rp != termrow; rp = rp->r_next) {
		if (rp->r_firstcell == termcell) continue;
		if (rp->r_row < minr) minr = rp->r_row;
		maxr = rp->r_row;
		if (rp->r_firstcell->c_col<minc) minc = rp->r_firstcell->c_col;
		if (rp->r_lastcell->c_col>maxc) maxc = rp->r_lastcell->c_col;
		err = 0;
	}
	if (err) {			/* no cells in object */
		minr = 1;
		maxr = 0;
		minc = 1;
		maxc = 0;
	}
	*minrow = minr;
	*maxrow = maxr;
	*mincol = minc;
	*maxcol = maxc;
	return(err);
}


/*
 * Search forwards for the nth next object, restarting at the top if necessary.
 * If all is nonzero, or if wrap around occurs, the search will be over all
 * objects.  Otherwise, objects found in previous searches will be skipped.
 * Returns nonzero if nothing was found.
 */
searchobject(obj, count, all)
	register struct	object	*obj;		/* object to search through */
{
	register struct	row	*rp;	/* current row being examined */
	register struct	cell	*cp;	/* current cell begin examined */
	register long	row;		/* current row */
	register long	col;		/* current column */

	if (all) clearmarks(obj, MARK_SRC);
	row = obj->o_currow;
	col = obj->o_curcol;
	for (rp = obj->o_firstrow; row > rp->r_row; rp = rp->r_next) ;
	for (cp = rp->r_firstcell; col > cp->c_col; cp = cp->c_next) ;
	if ((row == rp->r_row) && (col == cp->c_col) &&
		((cp->c_marks & MARK_SRC) == 0)) count++;
	while (1) {
		if (stop) return(0);
		if (cp == termcell) {
			rp = rp->r_next;
			if (rp == termrow) {
				clearmarks(obj, MARK_SRC);
				rp = obj->o_firstrow;
			}
			cp = rp->r_firstcell;
			continue;
		}
		if ((cp->c_marks & MARK_SRC) == 0) {
			markobject(obj, rp->r_row, cp->c_col, MARK_SRC);
			if (--count <= 0) break;
		}
		cp = cp->c_next;
	}
	obj->o_currow = rp->r_row;
	obj->o_curcol = cp->c_col;
	return(0);
}
