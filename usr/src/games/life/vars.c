#ifdef SCCS
static char *sccsid = "@(#)vars.c	1.11	2/2/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "life.h"

enum	vars	{		/* ordering of variables */
	v_minx, v_miny, v_maxx, v_maxy, v_cells, v_cx, v_cy, v_px, v_py,
	v_vminx, v_vmaxx, v_vminy, v_vmaxy, v_vcells, v_vx, v_vy,
	v_sminx, v_smaxx, v_sminy, v_smaxy, v_scells, v_gen, v_scale,
	v_freq, v_born, v_died, v_endlist
};


/*
 * Table of multi-character variables.  These are arranged in this table
 * in rows of 3 so that you can see what the display will look like for
 * the listvariables routine.
 */
struct	vartab	{
	char	*v_name;	/* name of variable */
	enum	vars v_type;	/* variable id */
} vartab[] = {
	"cx", v_cx,		"vx", v_vx,		"px", v_px,
	"cy", v_cy,		"vy", v_vy,		"py", v_py,
	"minx", v_minx,		"vminx", v_vminx,	"sminx", v_sminx,
	"maxx", v_maxx,		"vmaxx", v_vmaxx,	"smaxx", v_smaxx,
	"maxy", v_maxy,		"vmaxy", v_vmaxy,	"smaxy", v_smaxy,
	"miny", v_miny,		"vminy", v_vminy,	"sminy", v_sminy,
	"cells", v_cells,	"vcells", v_vcells,	"scells", v_scells,
	"gen", v_gen,		"born", v_born,		"died", v_died,
	"scale", v_scale,	"freq", v_freq,		NULL, v_endlist
};


static	char	*curcp;		/* current character to parse */
static	long	lowervars[26];	/* lower case single-char variable values */
static	long	uppervars[26];	/* upper case single-char variable values */


/*
 * Return the value of a multiple character variable name.  This can be
 * either a single character name, or else one of a fixed set of multi-
 * character names.  All variable names must start with either a letter
 * or a dollar sign followed by a letter.
 */
getvariable(cp)
	register char	*cp;			/* name of variable */
{
	register struct	vartab	*vp;		/* variable pointer */

	if (*cp == '$') cp++;			/* skip any dollar sign */
	if (cp[1] == '\0') {			/* single character name */
		return(getvariable1(*cp));
	}
	if (((*cp < 'a') || (*cp > 'z')) && ((*cp < 'A') || (*cp > 'Z'))) {
		error("Bad variable name");
	}
	for (vp = vartab; ; vp++)  {		/* find name in table */
		if (vp->v_name == NULL) error("Unknown variable");
		if (strcmp(vp->v_name, cp) == 0) break;
	}
	return(getvariablebytype(vp->v_type));	/* get value */
}


/*
 * Return the value of a variable given its enum value.
 */
getvariablebytype(type)
	enum	vars	type;			/* variable type to get */
{
	register struct	object	*obj;		/* current object */
	register long	value;			/* value to return */
	register long	*ptr;			/* pointer to minmax value */
	register long	*sptr;			/* another one */
	long	sign;				/* sign for result */
	long	minrow, maxrow, mincol, maxcol;	/* results of minmax */

	obj = curobj;
	value = 0;
	sign = 1;
	ptr = NULL;
	sptr = NULL;

	switch (type) {			/* collect value */
	case v_cx:	value = obj->o_curcol; break;
	case v_cy:	value = -obj->o_currow; break;
	case v_px:	value = obj->o_pcol; break;
	case v_py:	value = -obj->o_prow; break;
	case v_vx:	value = (obj->o_mincol + obj->o_maxcol) / 2; break;
	case v_vy:	value = -(obj->o_minrow + obj->o_maxrow) / 2; break;
	case v_vminx:	value = obj->o_mincol; break;
	case v_vmaxx:	value = obj->o_maxcol; break;
	case v_vminy:	value = -obj->o_maxrow; break;
	case v_vmaxy:	value = -obj->o_minrow; break;
	case v_vcells:	value = markregion(obj, MARK_ANY, obj->o_minrow,
				obj->o_maxrow, obj->o_mincol, obj->o_maxcol);
			break;
	case v_cells:	value = obj->o_count; break;
	case v_gen:	value = obj->o_gen; break;
	case v_born:	value = obj->o_born; break;
	case v_died:	value = obj->o_died; break;
	case v_freq:	value = frequency; break;
	case v_scale:	value = obj->o_scale; break;
	case v_minx:	ptr = &mincol; break;
	case v_maxx:	ptr = &maxcol; break;
	case v_miny:	ptr = &maxrow; sign = -1; break;
	case v_maxy:	ptr = &minrow; sign = -1; break;
	case v_scells:	value = countmarks(obj, MARK_SEE); break;
	case v_sminx:	sptr = &mincol; break;
	case v_smaxx:	sptr = &maxcol; break;
	case v_sminy:	sptr = &maxrow; sign = -1; break;
	case v_smaxy:	sptr = &minrow; sign = -1; break;
	}
	/*
	 * Call proper minmax routines if we need to
	 */
	if (ptr && (minmax(obj, &minrow, &maxrow, &mincol, &maxcol) == 0))
		value = *ptr;
	if (sptr&&(markminmax(obj,MARK_SEE,&minrow,&maxrow,&mincol,&maxcol)==0))
		value = *sptr;
	return(sign * value);
}


/*
 * Return the value of a single character variable name (a-z or A-Z).
 */
getvariable1(ch)
	register int	ch;		/* variable character */
{
	if ((ch >= 'a') && (ch <= 'z'))
		return(lowervars[ch - 'a']);
	if ((ch >= 'A') && (ch <= 'Z'))
		return(lowervars[ch - 'A']);
	error("Bad variable name");
}


/*
 * Set the value of a variable name.  Multi-character names cannot be set.
 */
setvariable(cp, value)
	register char	*cp;		/* name of variable to set */
{
	if (*cp == '$') cp++;		/* skip any dollar sign */
	if (cp[1] != '\0') {
		error("Cannot set multi-character variables");
	}
	setvariable1(*cp, value);	/* do it */
}


/*
 * Set the value of a single-character variable (a-z or A-Z).
 */
setvariable1(ch, value)
	register int	ch;		/* variable character */
{
	if ((ch >= 'a') && (ch <= 'z')) {
		lowervars[ch - 'a'] = value;
		return;
	}
	if ((ch >= 'A') && (ch <= 'Z')) {
		lowervars[ch - 'A'] = value;
		return;
	}
	error("Bad variable name");
}


/*
 * Display the current values of the variables.  Show all multi-character
 * variable values, and those single character variables which have a
 * nonzero value.  The output is given three variables per line.
 */
listvariables()
{
	register struct	vartab	*vp;		/* variable table pointer */
	register long	*var;			/* simple variable pointer */
	register int	count;			/* counter for formatting */

	dpywindow(0, -1, 0, -1);
	dpyprintf("Variable names and values:");
	count = 0;
	for (vp = vartab; vp->v_name; vp++) {
		dpyprintf("%s%s\t%d", (count++ % 3) ? "\t\t" : "\n",
			vp->v_name, getvariablebytype(vp->v_type));
	}
	count = 0;			/* create blank line */
	for (var = uppervars; var < &uppervars[26]; var++) {
		if (*var == 0) continue;
		if (count == 0) dpychar('\n');
		dpyprintf("%s%c\t%d", (count++ % 3) ? "\t\t" : "\n",
			'A' + (var - uppervars), *var);
	}
	for (var = lowervars; var < &lowervars[26]; var++) {
		if (*var == 0) continue;
		if (count == 0) dpychar('\n');
		dpyprintf("%s%c\t%d", (count++ % 3) ? "\t\t" : "\n",
			'a' + (var - lowervars), *var);
	}
	dpyprintf("\n\nMany names starting with 'v' refer to visible cells\n");
	dpyprintf("Many names starting with 's' refer to selected cells\n");
	spacewait();			/* wait for space before clearing */
}


/*
 * Evaluate an expression and return its value.  The expression can
 * contain numbers, variables, the normal arithmetic operators, and
 * parenthesized expressions.  The usual precedence rules are used.
 */
getexpression(str)
	register char	*str;		/* string to parse */
{
	long	value;			/* resulting value */

	while (*str == ' ') str++;
	if (*str == '\0') error("Null expression");
	curcp = str;
	value = parsesum();
	if (*curcp != '\0') error("Bad expression");
	return(value);
}


/* Parse the sum of products */
parsesum()
{
	register long	value;		/* value to return */

	while (*curcp == ' ') curcp++;
	switch (*curcp) {		/* check for uninary operators */
		case '-':
			curcp++;
			value = -parseproduct();
			break;
		case '+':
			curcp++;
			/* proceed into default case */
		default:
			value = parseproduct();
	}
	while (1) switch (*curcp++) {
		case '+':		/* sum of products */
			value += parseproduct();
			continue;
		case '-':		/* difference of products */
			value -= parseproduct();
			continue;
		case ' ':		/* space */
			continue;
		default:		/* end of sum */
			curcp--;
			return(value);
	}
}


/* Parse the product of terms */
parseproduct()
{
	register long	value;		/* value to return */
	register long	value2;		/* temporary value */

	value = parseterm();
	while (1) switch (*curcp++) {
		case '*':		/* product of terms */
			value *= parseterm();
			continue;
		case '/':		/* division of terms */
			value2 = parseterm();
			if (value2 == 0) error("division by zero");
			value /= value2;
			continue;
		case '%':		/* modulo of terms */
			value2 = parseterm();
			if (value2 == 0) error("division by zero");
			value %= value2;
			continue;
		case ' ':		/* space */
			continue;
		default:		/* end of product */
			curcp--;
			return(value);
	}
}


/* Parse a single term */
parseterm()
{
	register long	value;		/* value to return */
	register int	ch;		/* current character */

	while (*curcp == ' ') curcp++;
	ch = *curcp;
	if ((ch >= '0') && (ch <= '9')) {	/* number */
		value = 0;
		do
			value = (value * 10) + *curcp++ - '0';
		while ((*curcp >= '0') && (*curcp <= '9'));
		return(value);
	}
	if (ch == '(') {			/* parenthesized expression */
		curcp++;
		while (*curcp == ' ') curcp++;
		if (*curcp == ')') error("Null expression");
		value = parsesum();
		while (*curcp == ' ') curcp++;
		if (*curcp != ')') error("Unmatched parenthesis");
		*curcp++;
		return(value);
	}
	if (ch == ')') error("Unmatched parenthesis");
	return(parsename());
}


/*
 * Parse a variable name and return its value.
 */
parsename()
{
	register char	*cp;		/* current character */
	register long	value;		/* value of variable */
	char	oldch;			/* old character after name */

	cp = curcp;
	if (*cp == '$') cp++;
	while (((*cp >= 'a') && (*cp <= 'z')) ||
		((*cp >= 'A') && (*cp <= 'Z')) ||
		((*cp >= '0') && (*cp <= '9'))) cp++;
	oldch = *cp;
	*cp = '\0';
	value = getvariable(curcp);
	*cp = oldch;
	curcp = cp;
	return(value);
}
