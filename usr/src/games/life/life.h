/*
 * @(#)life.h	1.9	1/31/85
 * @(#)Copyright (C) 1985 by D Bell
 */

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

#define	LIFELIB	"/usr/games/lib/life"	/* directory for general life library */
#define	LIFEVAR	"LIFELIB"		/* environment name for user's lib */
#define	ALLOCOBJ 10			/* how many new objects to allocate */
#define	ALLOCROW 50			/* how many new rows to allocate */
#define	ALLOCCELL 500			/* how many new cells to allocate */
#define	FILESIZE (1024*4)		/* file buffer size */
#define	LOOPSIZE 5000			/* characters in command loops */
#define	SCAN_SIZE 100			/* maximum command length */
#define	MAXINPUT 20			/* maximum nesting of command inputs */
#define	MAXNAME	32			/* maximum object name size */
#define	MAXSCALE 1000			/* maximum scale factor */
#define	WRITEROWS 100			/* default maximum rows for writing */
#define	WRITECOLS 79			/* default maximum cols for writing */
#define	ESC	'\033'			/* escape character */
#define	INFINITY 0x7fffffff		/* infinite value */
#define	LIFE	9			/* live cell value in neighbor count */
#define	STDIN	0			/* standard input */
#define	STDOUT	1			/* standard output */
#define	STDERR	2			/* standard error */
#define	RELATIVE 0			/* do object additions relatively */
#define	ABSOLUTE 1			/* do object additions absolutely */
#define	M_MOVE	0			/* movement mode */
#define	M_INSERT 1			/* insertion mode */
#define	M_DELETE 2			/* deletion mode */
#define	MARK_ANY 0x1			/* mark always set */
#define	MARK_USR 0x2			/* marked due to user specification */
#define	MARK_CMD 0x4			/* marked only for current command */
#define	MARK_SRC 0x8			/* marked by searching */
#define	MARK_SEE (MARK_USR)		/* marks seen by user */
#define	MARK_ALL (MARK_ANY|MARK_USR|MARK_CMD|MARK_SRC)	/* all non-null marks */
#define	INP_TTY	0			/* input is terminal */
#define	INP_FILE 1			/* input is file */
#define	INP_LOOP 2			/* input is command loop */
#define	INP_MACRO 3			/* input is command macro */
#define	SCAN_ABORT 1			/* setjmp value for aborted command */
#define	SCAN_EDIT 2			/* setjmp value for edited command */
#define	SCAN_EOF 3			/* setjmp value for no command data */
#define	crow	curobj->o_currow	/* current row of current object */
#define	ccol	curobj->o_curcol	/* current column of current object */
#define	cmark	curobj->o_mark		/* current mark being applied */
#define	cscale	curobj->o_scale		/* current scale of current object */
#define	prow	curobj->o_prow		/* current pointer row */
#define	pcol	curobj->o_pcol		/* current pointer column */

/* macro to detect reserved names */
#define	BADNAME(s) ((s[0] == '.') && ((s[1] == '\0') || (s[1] == '.')))


struct	object	{			/* structure for each object */
	struct	row	*o_firstrow;	/* first row */
	struct	row	*o_lastrow;	/* last row */
	struct	object	*o_next;	/* next object */
	long	o_count;		/* number of live cells */
	long	o_born;			/* number of cells born */
	long	o_died;			/* number of cells died */
	long	o_currow;		/* current row */
	long	o_curcol;		/* current column */
	long	o_minrow;		/* minimum row seen in window */
	long	o_maxrow;		/* maximum row seen in window */
	long	o_mincol;		/* minimum column seen in window */
	long	o_maxcol;		/* maximum column seen in window */
	long	o_prow;			/* currently pointed at row */
	long	o_pcol;			/* currently pointed at column */
	long	o_gen;			/* current generation */
	short	o_scale;		/* current scaling factor for view */
	char	o_autoscale;		/* doing autoscaling */
	char	o_mark;			/* mark value for new cells */
	char	o_reserved;		/* reserved object */
	char	o_lock;			/* locked against computing gens */
	char	o_name[MAXNAME+1];	/* name of object */
};

struct	row	{			/* structure for each row of cells */
	struct	row	*r_next;	/* link to next row */
	struct	cell	*r_firstcell;	/* link to first cell */
	struct	cell	*r_lastcell;	/* link to last real cell */
	long	r_row;			/* row number */
	long	r_count;		/* number of cells in this row */
};

struct	cell	{			/* structure for each cell */
	struct	cell	*c_next;	/* link to next cell */
	long	c_col;			/* column number */
	int	c_marks;		/* marking values for cell */
};


/*
 * The following structure holds all data necessary for processing
 * characters from some source.
 */
struct	input	{
	int	(*i_getchar)();		/* routine to read next character */
	int	(*i_term)();		/* routine to terminate reading */
	char	*i_begptr;		/* beginning of command data */
	char	*i_endptr;		/* end of command data */
	char	*i_curptr;		/* current character */
	char	i_type;			/* type of input */
			/* following data for file reading only */
	short	i_file;			/* file descriptor */
	struct	object	*i_obj;		/* object to restore on reentry */
	long	i_row;			/* row to restore */
	long	i_col;			/* column to restore */
	long	i_prow;			/* pointer row to restore */
	long	i_pcol;			/* pointer column to restore */
			/* following data for loop or macro reading only */
	long	i_curval;		/* current iteration value */
	long	i_endval;		/* ending iteration value */
	int	i_first;		/* processing new chars */
	char	i_macro;		/* macro being defined */
};


struct	macro	{			/* structure for command macros */
	char	*m_begptr;		/* beginning of data (NULL if none) */
	char	*m_endptr;		/* end of data */
};


struct	object	*objects;		/* list of active objects */
struct	object	*curobj;		/* currently selected object */
struct	object	*prevobj;		/* previously selected object */
struct	object	*mainobject;		/* the main object */
struct	object	*deleteobject;		/* object last deleted */
struct	object	*backupobject;		/* backup object */
struct	object	*tempobject;		/* temporary object */
struct	object	*freeobjects;		/* list of free objects */
struct	object	*newobjects;		/* top of new object allocation */
struct	object	*endobjects;		/* end of new objects */

struct	row	*freerows;		/* list of free row structures */
struct	row	*newrows;		/* top of new row allocation */
struct	row	*endrows;		/* end of new rows */
struct	row	*termrow;		/* terminus row */
struct	row	initrow;		/* row to initialize list */

struct	cell	*freecells;		/* list of free cell structures */
struct	cell	*newcells;		/* top of new cell allocation */
struct	cell	*endcells;		/* end of new cells */
struct	cell	*termcell;		/* terminus cell */
struct	cell	initcell;		/* cell to initialize list */

struct	input	*curinput;		/* current input being read from */

int	seecount;			/* number of cells we can see */
int	frequency;			/* typeout frequency */
int	freqcount;			/* current count */
int	genleft;			/* generations left before stopping */
short	rowradius;			/* half of length of screen */
short	colradius;			/* half of width of screen */
char	reserve;			/* reserved object names allowed */
char	dowait;				/* must wait for input */
char	update;				/* status or position needs updating */
char	redraw;				/* whole screen needs updating */
char	interact;			/* still being interactive */
char	stop;				/* user wants to stop current action */
char	mode;				/* mode of movement */
char	gridchar;			/* character to use for grid */
char	*errorstring;			/* error string to type */
char	*userlib;			/* user's life library if any */
jmp_buf	ttyjmp;				/* jump buffer */
struct	input	inputs[MAXINPUT];	/* list of input environments */
struct	macro	macros[26];		/* list of macros */
char	stringbuf[FILESIZE];		/* characters for string value */
char	rulestring[20];			/* string describing rules */

extern	char	rules[];		/* life rules */

struct	object	*allocobject();		/* allocate new object */
struct	object	*findobject();		/* find object with certain name */
struct	object	*getobject();		/* get new object with certain name */
struct	row	*allocrow();		/* allocate new row */
struct	row	*findrow();		/* find a row */
struct	row	*getrow();		/* get a new row */
struct	row	*computerow();		/* compute a new row */
struct	cell	*alloccell();		/* allocate new cell */
struct	cell	*findcell();		/* find a cell */

char	*readstring();			/* read input */
