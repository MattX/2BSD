#ifdef SCCS
static char *sccsid = "@(#)cmdl.c	1.31	2/2/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "life.h"
#include <sys/ioctl.h>

/* list of line command routines */
int	c_create(), c_destroy(), c_edit(), c_dumpmacros(), c_quit();
int	c_frequency(), c_listobjects(), c_zero(), c_lock(), c_unlock();
int	c_read(), c_write(), c_ttyinput(), c_gridchar(), c_nogrid();
int	c_insert(), c_copy(), c_copyselect(), c_move(), c_moveselect();
int	c_help(), c_rules(), c_endinputlevel(), c_undo(), c_rename();
int	c_set(), c_variables(), c_type(), c_update(), c_wait();

/* flags for line commands */
#define	F_NONE	0x0		/* no special condition */
#define	F_NOARG 0x1		/* must not have an argument */
#define	F_ARG1	0x2		/* must have at least one argument */
#define	F_NORUN	0x4		/* illegal if running generations */
#define	F_UPDATE 0x8		/* update status if command completes */
#define	F_REDRAW 0x10		/* redraw screen if command completes */
#define	F_ABBR 0x20		/* abbreviation works even if ambiguous */


/*
 * Dispatch table for line commands.  Those commands which are ambiguous
 * must be defined so as to be contiguous in the table.  A spaces delimits
 * the command itself from the help string for the command.
 */
struct	cmdtab {
	char	*c_name;		/* command name */
	int	(*c_func)();		/* function to call */
	int	c_flags;		/* flags for command */
} cmdtab[] = {
	"copy (current object to) obj", c_copy, F_ARG1|F_NORUN|F_ABBR,
	"copyselection (to) obj", c_copyselect, F_ARG1|F_NORUN,
	"create (object named) obj", c_create, F_ARG1|F_NORUN|F_REDRAW,
	"destroy (object named) obj", c_destroy, F_ARG1|F_NORUN,
	"dumpmacros (to) file", c_dumpmacros, F_ARG1|F_NORUN,
	"edit (object named) obj", c_edit, F_NORUN|F_REDRAW|F_ABBR,
	"endinputlevel", c_endinputlevel, F_NOARG|F_UPDATE,
	"frequency (of typeout is) expr", c_frequency, F_UPDATE,
	"gridcharacter (is) char", c_gridchar, F_REDRAW,
	"help", c_help, F_NONE|F_ABBR,
	"insert (object from) obj", c_insert, F_ARG1|F_NORUN|F_REDRAW|F_ABBR,
	"list (all objects)", c_listobjects, F_NORUN|F_ABBR,
	"lock (current object)", c_lock, F_NOARG|F_NORUN|F_UPDATE,
	"move (current object to) obj", c_move, F_ARG1|F_NORUN|F_REDRAW|F_ABBR,
	"moveselection (to) obj",c_moveselect,F_ARG1|F_NORUN|F_REDRAW,
	"nogrid", c_nogrid, F_NOARG|F_REDRAW,
	"quit (program)", c_quit, F_NOARG|F_ABBR,
	"read (commands from) file", c_read, F_ARG1|F_NORUN|F_REDRAW|F_ABBR,
	"rename (current object to) obj", c_rename, F_ARG1|F_UPDATE,
	"rules (for life are) born live", c_rules, F_NORUN|F_UPDATE,
	"set (variable) name (to) expr", c_set, F_ARG1|F_ABBR,
	"ttyinput", c_ttyinput, F_REDRAW,
	"type (value of expression) expr", c_type, F_ARG1|F_UPDATE|F_ABBR,
	"undo (last change)", c_undo, F_NOARG|F_NORUN|F_REDRAW|F_ABBR,
	"unlock (current object)", c_unlock, F_NOARG|F_NORUN|F_UPDATE,
	"updateview (to be current)", c_update, F_NOARG,
	"variables (are listed)", c_variables, F_NOARG|F_REDRAW|F_ABBR,
	"wait (for computations)", c_wait, F_NOARG,
	"write (current object to) file", c_write, F_ARG1|F_NORUN|F_ABBR,
	"zero (current object)", c_zero, F_NOARG|F_NORUN|F_REDRAW|F_ABBR,
	0					/* ends the table */
};


/*
 * Read and execute a command line style command.  This kind of command echoes,
 * and is terminated by an end of line.  Numeric arguments are available.
 */
dolinecommand(arg1, arg2, got1, got2)
{
	register char	*name;		/* command name */
	register char	*args;		/* arguments for command */
	register struct	cmdtab *cmd;	/* command structure */
	register int	flag;		/* flags for command */

	name = readstring("command: ");
	if ((*name == '\0') || (*name == '\n')) return;
	for (args = name; *args && (*args != ' ') && (*args != '\t'); args++) ;
	while ((*args == ' ') || (*args == '\t')) *args++ = '\0';
	for (cmd = cmdtab; ; cmd++) {
		if (cmd->c_name == NULL) error("Unknown line command");
		if (abbrev(name, cmd[0].c_name) == 0) continue;
		if (cmd->c_flags & F_ABBR) break;
		if (abbrev(name, cmd[1].c_name) == 0) break;
		if (strcmp(name, cmd[0].c_name) == 0) break;
		if (cmd[1].c_flags & F_ABBR) continue;
		error("Ambiguous line command");
	}
	flag = cmd->c_flags;
	if (flag & F_NORUN) checkrun();
	if ((flag & F_ARG1) && (*args == '\0')) error("Missing argument");
	if ((flag & F_NOARG) && *args) error("Argument not allowed");
	cmd->c_func(args, arg1, arg2, got1, got2);
	if (flag & F_UPDATE) update = 1;
	if (flag & F_REDRAW) redraw = 1;
}



/* Copy the current object into another object */
c_copy(cp)
	char	*cp;			/* destination object name */
{
	copyobject(curobj, getobject(cp));
}


/* Copy the current selection into another object */
c_copyselect(cp)
	char	*cp;			/* destination object name */
{
	copymarkedobject(curobj, getobject(cp), MARK_USR);
}


/* Edit an object, creating it if necessary */
c_create(cp)
	char	*cp;			/* object to create */
{
	setobject(getobject(cp));
}


/* Destroy an existing object */
c_destroy(cp)
	char	*cp;			/* object name */
{
	register struct	object	*obj;	/* object to destroy */

	obj = findobject(cp);
	if (obj == NULL) error("No such object");
	destroyobject(obj);
}


/* Dump list of macros to file */
c_dumpmacros(cp)
	char	*cp;			/* file name */
{
	writemacros(cp);
}


/* Edit an existing object.  A null argument implies the previous object. */
c_edit(cp)
	char	*cp;			/* object name */
{
	register struct	object	*obj;	/* object to edit */

	obj = prevobj;
	if (*cp) obj = findobject(cp);
	if (obj == NULL) error("No such object");
	setobject(obj);
}


/* Undo the last change made to the current object */
c_undo()
{
	moveobject(curobj, tempobject);
	moveobject(backupobject, curobj);
	moveobject(tempobject, backupobject);
}


/* End current input level */
c_endinputlevel()
{
	if (curinput <= inputs) error("Cannot end top level input");
	curinput->i_term(curinput);
}


/* Update the view even if inside of a loop or macro */
c_update()
{
	update = 1;
	updateview();
}


/* Wait until all outstanding generation computations are finished */
c_wait()
{
	while ((stop == 0) && (genleft > 0)) {
		dogeneration(curobj);
		updateview();
	}
}


/* Set output frequency */
c_frequency(cp)
	register char	*cp;		/* frequency string */
{
	register int	freq;		/* new frequency */

	freq = 1;
	if (*cp) freq = getexpression(cp);
	if (freq <= 0) error("Illegal value");
	frequency = freq;
	freqcount = freq;
}


/* Select the character used for the background of the screen */
c_gridchar(cp)
	register char	*cp;		/* grid character string */
{
	if (*cp == '\0') cp = ".";
	if ((*cp < ' ') || (cp[1] != '\0')) error("Bad grid character");
	gridchar = *cp;
}


/* Set so we don`t see a grid on the screen */
c_nogrid()
{
	gridchar = ' ';
}


/* Type list of line commands */
c_help()
{
	register struct	cmdtab *cmd;	/* command structure */
	register int	count;

	dpywindow(0, -1, 0, -1);
	dpystr("\
The following table lists all line mode commands.  Unique abbreviations are\n\
allowed.  Commands shown with '*' can be abbreviated even when ambiguous.\n");
	count = 0;
	for (cmd = cmdtab; cmd->c_name; cmd++) {
		if ((count++ % 2) == 0) dpychar('\n');
		dpyprintf("%c %-35s", ((cmd->c_flags & F_ABBR) ? '*' : ' '),
			cmd->c_name);
	}
	dpychar('\n');
	spacewait();
}


/* Insert another object into this one */
c_insert(cp)
	char	*cp;			/* object name */
{
	register struct	object	*obj;	/* object to insert */

	obj = findobject(cp);
	if (obj == NULL) error("No such object");
	cmark = MARK_USR;
	backup();
	addobject(obj, curobj, RELATIVE);
	cmark = MARK_ANY;
}


/* Show list of objects */
c_listobjects(cp)
	register char	*cp;		/* option string */
{
	int	all;			/* true if want to see all objects */

	all = ((*cp == '-') || (*cp == 'a'));
	listobjects(all);
}


/* Lock object so it can't be run */
c_lock()
{
	curobj->o_lock = 1;
	genleft = 0;
	freqcount = frequency;
}


/* Unlock object so it can be run */
c_unlock()
{
	curobj->o_lock = 0;
}


/* Rename the current object to something else */
c_rename(cp)
	register char	*cp;			/* new name */
{
	if (curobj->o_reserved) error("Cannot rename reserved object");
	if (strlen(cp) > MAXNAME) error("Name too long");
	if (findobject(cp)) error("Name already exists");
	if (BADNAME(cp)) error("Cannot create reserved name");
	strcpy(curobj->o_name, cp);
}


/* Move current object into another object */
c_move(cp)
	char	*cp;			/* destination object name */
{
	moveobject(curobj, getobject(cp));
}


/* Move current selection into another object */
c_moveselect(cp)
	char	*cp;			/* destination object name */
{
	movemarkedobject(curobj, getobject(cp), MARK_USR);
}


/* Set the value of a variable */
c_set(cp)
	register char	*cp;		/* variable name */
{
	register char	*exp;		/* expression */

	for (exp = cp; *exp && (*exp != ' ') && (*exp != '='); exp++) ;
	if (*exp == ' ') {		/* skip spaces */
		*exp++ = '\0';
		while (*exp == ' ') exp++;
	}
	if (*exp == '\0') {		/* no expression, set to zero */
		setvariable(cp, 0);
		return;
	}
	if (*exp == '=') *exp++ = '\0';
	setvariable(cp, getexpression(exp));
}


/* Type the value of an expression */
c_type(cp)
	char	*cp;				/* expression */
{
	static	char	buf[20];		/* storage for string */

	sprintf(buf, "%d\n", getexpression(cp));/* store value */
	errorstring = buf;			/* make it seen */
}


/* Display the values of all the variables */
c_variables()
{
	listvariables();
}


/* Quit program */
c_quit()
{
	dpyclose();
	exit(0);
}


/* Read commands or object from file, defaulting extension if needed */
c_read(cp)
	register char	*cp;		/* file name */
{
	backup();
	if (setfile(cp)) error("Cannot open input file");
}


/* Set new life rules */
c_rules(cp)
	register char	*cp;		/* life rules string */
{
	register char	*bp;		/* born string */
	register char	*lp;		/* live string */

	if (*cp == '\0') cp = "3,23";
	bp = cp;
	while ((*cp >= '0') && (*cp <= '8')) cp++;
	if (*cp == '\0') error("Missing rule string");
	if ((*cp != ',') && (*cp != ' ') && (*cp != '\t')) {
		error("Bad born string");
	}
	*cp++ = '\0';
	while ((*cp == ',') || (*cp == ' ') || (*cp == '\t')) cp++;
	lp = cp;
	while ((*cp >= '0') && (*cp <= '8')) cp++;
	if (*cp != '\0') error("Bad live string");
	for (cp = rules; cp < &rules[18]; cp++) *cp = 0;
	while (*bp) rules[*bp++ - '0'] = 1;
	while (*lp) rules[*lp++ - '0' + LIFE] = 1;
	bp = rulestring;
	for (cp = rules; cp < &rules[LIFE]; cp++) {
		if (*cp) *bp++ = '0' + (cp - rules);
	}
	*bp++ = ',';
	for (cp = &rules[LIFE]; cp < &rules[18]; cp++) {
		if (*cp) *bp++ = ('0' - LIFE) + (cp - rules);
	}
	*bp = '\0';
}


/*
 * Read commands from the terminal.  Useful in loops or command files.
 * If the -c argument is given, we don't do it if no input is ready.
 */
c_ttyinput(cp)
	register char	*cp;		/* pointer to argument string */
{
	int	count;			/* character count */

	if ((*cp == '-') || (*cp == 'c')) {	/* check for input */
		count = 0;
		ioctl(STDIN, FIONREAD, &count);
		if (count == 0) return;
	}
	if (settty()) error("Nesting too deep");
}


/* Write object to file */
c_write(cp, arg1, arg2, got1, got2)
	register char	*cp;		/* pointer to argument string */
{
	if (got1 == 0) arg1 = WRITECOLS;
	writeobject(curobj, cp, arg1?WRITEROWS:0, arg1);
}


/* Zero out current object */
c_zero()
{
	register struct	object	*obj;	/* current object */

	obj = curobj;
	if (obj->o_lock) error("Object is locked");
	backup();
	zeroobject(obj);
	obj->o_gen = 0;
	obj->o_born = 0;
	obj->o_died = 0;
	obj->o_currow = 0;
	obj->o_curcol = 0;
	obj->o_prow = 0;
	obj->o_pcol = 0;
	obj->o_autoscale = 0;
	setscale(obj, 1);
	mode = M_MOVE;
	frequency = 1;
	freqcount = 1;
}


/*
 * See if one string is an abbreviation of another.  This knows that
 * the second string contains spaces which terminate the comparison.
 * Returns nonzero if first string is an abbreviation.
 */
abbrev(str1, str2)
	register char	*str1, *str2;	/* strings */
{
	if ((str1 == NULL) || (str2 == NULL)) return(0);
	while (*str1) if (*str1++ != *str2++) return(0);
	return(1);
}
