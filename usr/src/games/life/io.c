#ifdef SCCS
static char *sccsid = "@(#)io.c	1.10	2/1/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include "life.h"
#include <errno.h>
#include <sys/ioctl.h>

extern	int	errno;			/* error return value */

int	tty_char(), tty_term();		/* terminal routines */
int	file_char(), file_term();	/* file routines */
int	loop_char(), loop_term();	/* loop routines */
int	macro_char(), macro_term();	/* macro routines */
int	getdpychar();			/* for dpyread to call */


/*
 * Read the next character from the appropriate input source.
 * Negative answer from source indicates end of file.
 * This routine is usually called indirectly by scanchar.
 */
readchar()
{
	register struct	input	*ip;	/* input structure being used */
	register int	ch;		/* current character */

	while (1) {			/* continue until not end of file */
		ip = curinput;
		ch = ip->i_getchar(ip);	/* read next character */
		if (ch >= 0) break;
		ip->i_term(ip);		/* end of file, clean up */
	}
	return(ch);
}


/*
 * Determine if the next input character to be read is from the terminal.
 * Returns nonzero if so.
 */
ttyisinput()
{
	register struct	input	*ip;	/* input structure being checked */

	ip = curinput;
	while ((ip->i_type == INP_LOOP) && (ip->i_first)) ip--;
	return(ip->i_type == INP_TTY);
}


/*
 * Setup to read commands from the terminal.  The lowest level of input
 * never quits, and is unusual in that it doesn't usually block waiting for
 * input.  Returns nonzero if failed.
 */
settty()
{
	register struct	input	*ip;		/* current input */

	ip = curinput + 1;		/* allocate next structure */
	if (ip >= &inputs[MAXINPUT]) {
		return(1);
	}
	ip->i_getchar = tty_char;	/* set up for I/O */
	ip->i_term = tty_term;
	ip->i_type = INP_TTY;
	update = 1;
	curinput = ip;
	return(0);
}


/*
 * Read next character from the terminal if it is ready.  If nothing is
 * going on we will wait for it anyway, to prevent excessive runtimes.
 * We set the interactive flag to indicate we are talking to user.
 */
tty_char()
{
	long	n;			/* char count */
	int	ch;			/* char to return */

	interact = 1;
	if ((dowait == 0) && (redraw || update || (genleft > 0))) {
		if ((ioctl(STDIN, FIONREAD, &n) == 0) && (n <= 0)) {
			scaneof();		/* no char available now */
		}
	}
	do {
		if (stop) return('\0');		/* stop will be seen later */
		errno = 0;
		n = read(STDIN, &ch, 1);	/* read one char */
	} while ((n < 0) && (errno == EINTR));
	if (n <= 0) {
		return(-1);			/* error or end of file */
	}
	if (errorstring) {			/* disable error message */
		errorstring = NULL;
		update = 1;
	}
	return(ch &= 0x7f);
}


/*
 * Terminate reading from the terminal.  If we are reading from the lowest
 * level of terminal input, this is a no-op.
 */
tty_term(ip)
	register struct	input	*ip;	/* input structure */
{
	if (ip > inputs) ip--;
	curinput = ip;
	update = 1;
}


/*
 * Setup to read commands from a given file name.   When the file is complete,
 * some parameters (like the current cursor position) will be restored to their
 * original value.  Returns nonzero if cannot set up to read the file.
 */
setfile(name)
	register char	*name;		/* file name to read from */
{
	register struct	input	*ip;	/* current input structure */

	ip = curinput + 1;			/* use next structure */
	if (ip >= &inputs[MAXINPUT]) {
		return(1);
	}
	ip->i_file = openlibfile(name, 0);	/* open the file */
	if (ip->i_file < 0) {
		return(1);
	}
	ip->i_begptr = (char *) malloc(FILESIZE); /* allocate data buffer */
	if (ip->i_begptr == NULL) {
		close(ip->i_file);
		return(1);
	}
	ip->i_endptr = ip->i_begptr;		/* set up rest of io */
	ip->i_curptr = ip->i_begptr;
	ip->i_getchar = file_char;
	ip->i_term = file_term;
	ip->i_type = INP_FILE;
	ip->i_obj = curobj;			/* save current state */
	ip->i_row = crow;
	ip->i_col = ccol;
	ip->i_prow = prow;
	ip->i_pcol = pcol;
	prow = crow;
	pcol = ccol;
	update = 1;
	curinput = ip;				/* this is now current input */
	return(0);
}


/*
 * Open a life library file, trying various transformed names if necessary.
 * The order of the transformations which are tried is:
 *   1.	Name exactly as given.
 *   2.	Name with ".l" appended to it.
 *   3.	Name in the user's library directory.
 *   4.	Name with ".l" appended to it in the user's library.
 *   5  Name in the system's library directory.
 *   6.	Name with ".l" appended to it in the system library.
 * Returns the file descriptor if the open is successful, or -1 if all the
 * open attempts failed.
 */
openlibfile(name, mode)
	register char	*name;		/* original file name */
	register int	mode;		/* desired open mode */
{
	register int	fd;		/* file descriptor */
	char	buf[2000];		/* transformed names */

	fd = open(name, mode);			/* try name straight */
	if (fd >= 0) return(fd);
	sprintf(buf, "%s.l", name);		/* try name with .l appended */
	fd = open(buf, mode);
	if (fd >= 0) return(fd);
	if (*name == '/') return(-1);		/* quit now if absolute name */
	if (userlib) {				/* look in user's lib area */
		sprintf(buf, "%s/%s", userlib, name);
		fd = open(buf, mode);
		if (fd >= 0) return(fd);
		strcat(buf, ".l");		/* try with .l */
		fd = open(buf, mode);
		if (fd >= 0) return(fd);
	}
	sprintf(buf, "%s/%s", LIFELIB, name);	/* look in general lib area */
	fd = open(buf, mode);
	if (fd >= 0) return(fd);
	strcat(buf, ".l");			/* last try with .l */
	return(open(buf, mode));
}


/*
 * Here to read next character from a file.  Called by readchar when
 * input source is a file.
 */
file_char(ip)
	register struct	input	*ip;		/* input structure */
{
	if (ip->i_curptr >= ip->i_endptr) {	/* get next chunk of file */
		ip->i_curptr = ip->i_begptr;
		ip->i_endptr = ip->i_begptr;
		ip->i_endptr += read(ip->i_file, ip->i_begptr, FILESIZE);
		if (ip->i_endptr <= ip->i_begptr) {
			return(-1);		/* end of file */
		}
	}
	return(*ip->i_curptr++ & 0x7f);
}


/*
 * Here on end of file or error to close the input file and restore some
 * of the previous state such as the cursor location.
 */
file_term(ip)
	register struct	input	*ip;	/* input structure */
{
	close(ip->i_file);
	free(ip->i_begptr);
	curobj = ip->i_obj;
	crow = ip->i_row;
	ccol = ip->i_col;
	prow = ip->i_prow;
	pcol = ip->i_pcol;
	update = 1;
	curinput = (ip - 1);
}


/*
 * Setup for execution of a loop.  This remembers the initial and final
 * loop values, and sets up to remember commands as they are given.
 * This is also used for defining a macro command.  The given character
 * will be assigned the string defined by the loop.
 * Returns nonzero if failed.
 */
setloop(begval, endval, ch)
{
	register struct	input	*ip;		/* input structure */

	ip = curinput + 1;		/* allocate next structure */
	if (ip >= &inputs[MAXINPUT]) {
		return(1);
	}
	ip->i_begptr = (char *) malloc(LOOPSIZE);	/* allocate buffer */
	if (ip->i_begptr == NULL) {
		return(1);
	}
	ip->i_endptr = ip->i_begptr + LOOPSIZE;		/* set up for I/O */
	ip->i_curptr = ip->i_begptr;
	ip->i_first = 1;
	ip->i_getchar = loop_char;
	ip->i_term = loop_term;
	ip->i_type = INP_LOOP;
	ip->i_curval = begval;
	ip->i_endval = endval;
	ip->i_macro = ch;
	update = 1;
	curinput = ip;
	return(0);
}


/*
 * End the range of the currently defined loop.  At this point, all of
 * the characters of the loop have been read in and saved, and we can
 * just proceed to iterate over them.  The next read will find out that
 * the first iteration of the loop is over.
 */
endloop()
{
	register struct	input	*ip;	/* current input */

	ip = curinput;
	if (ip->i_type != INP_LOOP) error("Loop not being defined");
	ip->i_endptr = ip->i_curptr - 1;	/* end before loop term cmd */
	ip->i_first = 0;
}


/*
 * Read one character from a loop buffer.  If at the end of the buffer, the
 * pointer is reset so that the buffer is reread.  When enough iterations
 * have been processed, we are done.  A special case exists the first time
 * through the loop at the first nesting level, in that we don't yet have
 * the characters necessary for the loop, and so we have to read them by
 * ourself.
 */
loop_char(ip)
	register struct	input	*ip;		/* input structure */
{
	register int	ch;

	if (ip->i_first) {			/* collecting input chars */
		if (ip->i_curptr >= ip->i_endptr) error("Loop too long");
		ch = ip[-1].i_getchar(ip - 1);	/* char from previous level */
		if (ch < 0) error("End of file in loop");
		*ip->i_curptr++ = ch;
		return(ch);
	}
	if (ip->i_curptr >= ip->i_endptr) {	/* done with one iteration */
		if (ip->i_curval == ip->i_endval) {
			return(-1);		/* end of file */
		}
		ip->i_curval += ((ip->i_curval < ip->i_endval) ? 1 : -1);
		ip->i_curptr = ip->i_begptr;
	}
	return(*ip->i_curptr++);
}


/*
 * Terminate reading from a loop buffer.  If this was the definition of
 * a macro character, remember it for later.
 */
loop_term(ip)
	struct	input	*ip;		/* input structure */
{
	register struct	macro	*mp;	/* macro being defined */

	mp = &macros[ip->i_macro - 'a'];
	if ((mp >= macros) && (mp < &macros[26])) {
		if (mp->m_begptr) free(mp->m_begptr);
		mp->m_begptr = ip->i_begptr;
		mp->m_endptr = ip->i_endptr;
	} else
		free(ip->i_begptr);		/* or free buffer */
	update = 1;
	curinput = (ip - 1);
}


/* Set up to read a defined macro command.  Returns nonzero if failed. */
setmacro(arg1, arg2, ch)
{
	register struct	input	*ip;	/* current input */
	register struct	macro	*mp;	/* macro command */

	mp = &macros[ch - 'a'];		/* verify macro character */
	if ((mp < macros) || (mp >= &macros[26]) || (mp->m_begptr == NULL)) {
		return(1);
	}
	ip = curinput + 1;		/* use next input structure */
	if (ip >= &inputs[MAXINPUT]) {
		return(1);
	}
	ip->i_getchar = macro_char;	/* set up for I/O */
	ip->i_term = macro_term;
	ip->i_begptr = mp->m_begptr;
	ip->i_curptr = mp->m_begptr;
	ip->i_endptr = mp->m_endptr;
	ip->i_type = INP_MACRO;
	ip->i_macro = ch;
	ip->i_curval = arg1;
	ip->i_endval = arg2;
	update = 1;
	curinput = ip;
	return(0);
}


/* Here to read next character from macro definition. */
macro_char(ip)
	register struct	input	*ip;		/* input structure */
{
	if (ip->i_curptr >= ip->i_endptr) {
		return(-1);		/* end of file */
	}
	return(*ip->i_curptr++);
}


/* Here to terminate reading from a macro definition. */
macro_term(ip)
	struct	input	*ip;			/* input structure */
{
	curinput = (ip - 1);
	update = 1;
}


/*
 * Read a line from the user terminated by a newline (which is removed).
 * Editing of the input line is fully handled.  The prompt string and the
 * input line are only visible if the current input is from the terminal.
 * Returns a pointer to the null-terminated string.
 */
char *
readstring(prompt)
	register char	*prompt;	/* prompt string */
{
	int	i;			/* number of characters read */

	scanreset();				/* no more scan interference */
	if (prompt == NULL) prompt = "";	/* set prompt if given NULL */
	if (ttyisinput() == 0) prompt = NULL;	/* no window stuff if not tty */
	if (prompt) dpywindow(0, 0, 0, -1);	/* show input in top line */
	dowait = 1;				/* must wait for tty chars */
	i = dpyread(prompt, getdpychar, stringbuf, FILESIZE);	/* read it */
	dowait = 0;				/* back to normal */
	stringbuf[i] = '\0';			/* terminate line */
	update = 1;
	return(stringbuf);
}


/*
 * Routine called by dpyread to read the next character of input.
 * We return end of input on the newline character.
 */
getdpychar()
{
	register int	ch;		/* character just read */

	ch = readchar();
	if (stop || (ch == '\n')) return(-1);
	return(ch);
}


/*
 * Routine called to wait until a space, newline, or escape character
 * is typed.  It is assumed that the screen contains a display which
 * we can append our message to.
 */
spacewait()
{
	register int	ch;			/* read character */

	scanreset();				/* throw out stored chars */
	dpystr("\nType <space> to return\n");	/* append our message */
	dpyclearwindow();
	dpyhome();
	dpyupdate();				/* show the result */
	redraw = 1;
	dowait = 1;				/* must wait for chars */
	do {
		if (stop) break;
		ch = readchar();
	} while ((ch != ' ') && (ch != ESC) && (ch != '\n'));
	dowait = 0;
}


/*
 * Write the current object out to the named file, with the given maximum
 * sizes for "pretty" output.  If the size is exceeded, the output is
 * compressed.  The file as written can be read in as commands which will
 * regenerate the object.
 */
writeobject(obj, name, maxrows, maxcols)
	struct	object	*obj;		/* object to write */
	char	*name;			/* filename to use */
{
	register FILE	*fd;		/* file structure */
	register struct	row	*rp;	/* current row structure */
	register struct	cell	*cp;	/* current cell */
	register int	row;		/* current row value */
	register int	col;		/* current column value */
	struct	row	*trp;		/* temporary row structure */
	int	minrow;			/* minimum row of object */
	int	maxrow;			/* maximum row of object */
	int	mincol;			/* minimum column of object */
	int	maxcol;			/* maximum column of object */
	int	curmin;			/* current minimum column */
	int	curmax;			/* current maximum column */
	int	testmin;		/* test minimum column of rows */
	int	testmax;		/* test maximum column of rows */

	minmax(obj, &minrow, &maxrow, &mincol, &maxcol);
	if (minrow > maxrow) error("Null object");	/* nothing to write */
	fd = fopen(name, "w");
	if (fd == NULL) error("Cannot open output file");
	fprintf(fd, "! \"%s\" (cells %d length %d width %d generation %d)\n",
		obj->o_name, obj->o_count, maxrow - minrow + 1,
		maxcol - mincol + 1, obj->o_gen);
	if (obj->o_currow > minrow) fprintf(fd, "%dk", obj->o_currow - minrow);
	if (obj->o_currow < minrow) fprintf(fd, "%dj", minrow - obj->o_currow);
	if (obj->o_curcol > mincol) fprintf(fd, "%dh", obj->o_curcol - mincol);
	if (obj->o_curcol < mincol) fprintf(fd, "%dl", mincol - obj->o_curcol);
	fprintf(fd, "@!\n");
	curmin = INFINITY;
	curmax = -INFINITY;
	rp = obj->o_firstrow;
	row = minrow;
	for (; rp != termrow; rp = rp->r_next) {
		/*
		 * See if user wants to stop.
		 */
		if (stop) {
			fclose(fd);
			return;
		}
		/*
		 * Skip down to the next row with something in it.
		 */
		if (rp->r_firstcell == termcell) continue;
		if (rp->r_row > (row + maxrows)) {	/* skip to right row */
			fprintf(fd, "%d\n", rp->r_row - row);
			row = rp->r_row;
		}
		while (rp->r_row > row) {
			fputs(".\n", fd);
			row++;
		}
		/*
		 * Output the current row, compressing if it is too wide.
		 */
		col = mincol;
		cp = rp->r_firstcell;
		if ((cp->c_col + maxcols) < rp->r_lastcell->c_col) {
			while (cp != termcell) {
				/*
				 * Write all adjacent blanks.
				 */
				if (cp->c_col > col + 2) {
					fprintf(fd, "%d.", cp->c_col - col);
					col = cp->c_col;
				}
				while (cp->c_col > col) {
					fputc('.', fd);
					col++;
				}
				/*
				 * Write all adjacent cells.
				 */
				while ((cp->c_col + 1) == cp->c_next->c_col) {
					cp = cp->c_next;
				}
				if (cp->c_col >= col + 2) {
					fprintf(fd, "%dO", cp->c_col - col + 1);
					col = cp->c_col + 1;
				}
				while (col <= cp->c_col) {
					fputc('O', fd);
					col++;
				}
				cp = cp->c_next;
			}
			fputc('\n', fd);
			row++;
			curmin = INFINITY;
			curmax = -INFINITY;
			continue;
		}
		/*
		 * Here if the row doesn't need compressing.  See if several
		 * rows from this one on can all fit in the same range.  If
		 * so, set things up so they will align themselves nicely.
		 */
		if ((cp->c_col < curmin) || (rp->r_lastcell->c_col > curmax)) {
			curmin = cp->c_col;
			curmax = rp->r_lastcell->c_col;
			trp = rp;
			for (; rp != termrow; rp = rp->r_next) {
				if (rp->r_firstcell == termcell) continue;
				testmin = rp->r_firstcell->c_col;
				if (testmin > curmin) testmin = curmin;
				testmax = rp->r_lastcell->c_col;
				if (testmax < curmax) testmax = curmax;
				if ((testmax - testmin) >= maxcols) break;
				curmin = testmin;
				curmax = testmax;
			}
			rp = trp;
		}
		/*
		 * Type the row, with the initial shift if necessary.
		 */
		if (curmin != mincol) {
			fprintf(fd, "%d.", curmin - mincol);
			col = curmin;
		}
		for (cp = rp->r_firstcell; cp != termcell; cp = cp->c_next) {
			while (cp->c_col > col) {
				fputc('.', fd);
				col++;
			}
			fputc('O', fd);
			col++;
		}
		fputc('\n', fd);
		row++;
	}
	fclose(fd);
}



/* Write all defined macros to a file so they can be read back in later. */
writemacros(name)
	char	*name;			/* file name to write to */
{
	register int	f;		/* file descriptor */
	register struct	macro	*mp;	/* current macro */
	int	ch;			/* character */

	f = creat(name, 0666);
	if (f < 0) error("Cannot create file");
	for (mp = macros; mp < &macros[26]; mp++) {
		if ((mp->m_begptr == NULL) || (mp->m_begptr >= mp->m_endptr)) {
			continue;
		}
		ch = 'a' + (mp - macros);
		write(f, "<", 1);
		write(f, &ch, 1);
		write(f, mp->m_begptr, mp->m_endptr - mp->m_begptr);
		write(f, ">!\n", 3);
	}
	close(f);
}
