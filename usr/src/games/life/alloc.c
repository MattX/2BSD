#ifdef SCCS
static char *sccsid = "@(#)alloc.c	1.5	2/1/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "life.h"


/*
 * Allocate a new object structure.  It contains one row element, the termrow.
 */
struct object *
allocobject()
{
	register struct	object	*obj;

	obj = freeobjects;
	if (obj)
		freeobjects = obj->o_next;
	else {
		obj = newobjects;
		if (obj >= endobjects) {	/* allocate new storage */
			obj = (struct object *)
				sbrk(sizeof(struct object) * ALLOCOBJ);
			if ((int)obj == -1) {
				perror("sbrk");
				exit(1);
			}
			newobjects = obj;
			endobjects = obj + ALLOCOBJ;
		}
		newobjects++;
	}
	obj->o_next = NULL;
	obj->o_firstrow = termrow;
	obj->o_lastrow = NULL;
	obj->o_reserved = reserve;
	obj->o_name[0] = '\0';
	obj->o_count = 0;
	obj->o_gen = 0;
	obj->o_lock = 0;
	obj->o_currow = 0;
	obj->o_curcol = 0;
	obj->o_prow = 0;
	obj->o_pcol = 0;
	obj->o_mark = 0;
	obj->o_autoscale = 0;
	setscale(obj, 1);
	return(obj);
}


/*
 * Allocate a new row structure.  It contains one row element, the termcell.
 */
struct row *
allocrow()
{
	register struct	row	*rp;

	rp = freerows;
	if (rp)
		freerows = rp->r_next;
	else {
		rp = newrows;
		if (rp >= endrows) {		/* allocate new storage */
			rp = (struct row *) sbrk(sizeof(struct row) * ALLOCROW);
			if ((int)rp == -1) {
				perror("sbrk");
				exit(1);
			}
			newrows = rp;
			endrows = rp + ALLOCROW;
		}
		newrows++;
	}
	rp->r_next = NULL;
	rp->r_firstcell = termcell;
	rp->r_lastcell = NULL;
	rp->r_count = 0;
	return(rp);
}


/* Allocate a new cell structure */
struct cell *
alloccell()
{
	register struct	cell	*cp;

	cp = freecells;
	if (cp) {
		freecells = cp->c_next;
		cp->c_next = NULL;
		cp->c_marks = MARK_ANY;
		return(cp);
	}
	cp = newcells;
	if (cp >= endcells) {		/* allocate new storage */
		cp = (struct cell *) sbrk(sizeof(struct cell) * ALLOCCELL);
		if ((int)cp == -1) {
			perror("sbrk");
			exit(1);
		}
		newcells = cp;
		endcells = cp + ALLOCCELL;
	}
	newcells++;
	cp->c_next = NULL;
	cp->c_marks = MARK_ANY;
	return(cp);
}
