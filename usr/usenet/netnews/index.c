#include "params.h"
static char *sccsid = "@(#)index.c	1.3	3/2/81";

/*
 * This file contains a set of routines
 * for accessing various files.
 */

static	FILE	*sysfile;
static	FILE	*uindex, *nindex;
static	FILE	*tuout, *tnout;
static	int	nmode, umode;

static	char *fldget();

#define	UPDATE	0
#define	READ	1
#define	APPEND	2

/*
 * Open uindex for reading only.
 */
u_openr()
{
	uindex = xfopen(UINDEX, "r");
	umode = READ;
}

/*
 * Open uindex for modifying.
 */
u_openm()
{
	u_openr();
	tuout = xfopen(TUFILE, "w");
	umode = UPDATE;
}

/*
 * Open uindex for append.
u_opena()
{
	uindex = xfopen(UINDEX, "a");
	umode = APPEND;
}
 */

/*
 * Open nindex for reading only.
 */
n_openr()
{
	nindex = xfopen(NINDEX, "r");
	nmode = READ;
}

/*
 * Open nindex for modifying.
 */
n_openm()
{
	n_openr();
	tnout = xfopen(TNFILE, "w");
	nmode = UPDATE;
}

/*
 * Open nindex for append.
n_opena()
{
	nindex = xfopen(NINDEX, "a");
	nmode = APPEND;
}
 */

/*
 * Read a record from uindex.
 */
u_read(up)
register struct urec *up;
{
	register char *p;

	p = bfr;
	if (fgets(p, BUFLEN, uindex) == NULL)
		return(FALSE);
	if (!nstrip(p))
		xerror("uindex record too long");
	up->u_uid = atoi(p);
	if (up->u_uid >= USIZE)
		xerror("uindex uid too big");
	while (*p && *p != ':')
		p++;
	if (*p++ == '\0')
		goto bad;
	up->u_date = atol(p);
	while (*p && *p != ':')
		p++;
	strcpy(&up->u_nglist[0], DFLTSUB);
	if (*p++)
		strcpy(&up->u_nglist[0], p);
	ngcat(&up->u_nglist[0]);
	return(TRUE);
bad:
	xerror("Bad uindex format");
	/* NOTREACHED */
}

/*
 * Read a nindex record.
 */
n_read(np)
register struct nrec *np;
{
	register char *p;

	p = bfr;
	if (fgets(p, BUFLEN, nindex) == NULL)
		return(FALSE);
	if (!nstrip(p))
		xerror("nindex record too long");
	p = fldget(&np->n_file[0], p);
	if (*p++ == '\0')
		goto bad;
	np->n_date = atol(p);
	while (*p && *p != ':')
		p++;
	strcpy(&np->n_nglist[0], DFLTSUB);
	if (*p++)
		strcpy(&np->n_nglist[0], p);
	ngcat(&np->n_nglist[0]);
	return(TRUE);
bad:
	xerror("Bad nindex format");
	/* NOTREACHED */
}

/*
 * Write a uindex record.
 */
u_write(up)
register struct urec *up;
{
	register FILE *fp;

	fp = (umode == APPEND) ? uindex : tuout;
	ngdel(strcpy(bfr, up->u_nglist));
	if (strcmp(bfr, DFLTSUB) == 0)
		fprintf(fp, "%d:%ld\n", up->u_uid, up->u_date);
	else
		fprintf(fp, "%d:%ld:%s\n", up->u_uid, up->u_date, bfr);
}

/*
 * Write a nindex record.
 */
n_write(np)
register struct nrec *np;
{
	register FILE *fp;

	fp = (nmode == APPEND) ? nindex : tnout;
	ngdel(strcpy(bfr, np->n_nglist));
	if (strcmp(bfr, DFLTSUB) == 0)
		fprintf(fp, "%s:%ld\n", np->n_file, np->n_date);
	else
		fprintf(fp, "%s:%ld:%s\n", np->n_file, np->n_date, bfr);
}

/*
 * This is a belated reply to complaints about curdled .uindexes.:
 * The problem is usually due to running out of disk space.
 * The following is a new (untested) u_close() to go in index.c;
 * n_close() could be changed similarly.
 * (Alas, if this happens when rnews is running, the news will be lost
 * (nowhere to put it), and mail will probably be unable to report the loss.)
 */

/*
 * Close uindex.
 */

u_close()
{
	fclose(uindex);
	if (umode == UPDATE) {
		fflush(tuout);		/* Write out all of file */
		if (ferror(tuout)) {	/* Check for write error */
			unlink(TUFILE);
			xerror("write error on temporary .uindex file");
		}
		fclose(tuout);
		rename(TUFILE, UINDEX);
	}
}

/*
 * Close nindex.
 */
n_close()
{
	fclose(nindex);
	if (nmode == UPDATE) {
		fclose(tnout);
		rename(TNFILE, NINDEX);
	}
}

/*
 * Open SYSFILE.
 */
s_openr()
{
	sysfile = xfopen(SYSFILE, "r");
}

/*
 * Read SYSFILE.
 */
s_read(sp)
register struct srec *sp;
{
	register char *p;

	p = bfr;
	if (fgets(p, BUFLEN, sysfile) == NULL)
		return(FALSE);
	if (!nstrip(p))
		xerror("SYSFILE line too long.");
	sp->s_xmit[0] = '\0';

	p = fldget(sp->s_name, p);
	if (*p++ == '\0')
		xerror("Bad SYSFILE line.");

	p = fldget(sp->s_nbuf, p);
	ngcat(sp->s_nbuf);
	if (*p++ == '\0')
		return(TRUE);

	p = fldget(bfr, p);
	if (*p++ == '\0')
		return(TRUE);

	fldget(sp->s_xmit, p);
	return(TRUE);
}

static char *
fldget(q, p)
register char *q, *p;
{
	while (*p && *p != ':') {
		if (*p == '\\' && p[1]==':')
			p++;
		*q++ = *p++;
	}
	*q = '\0';
	return(p);
}

/*
 * Find the SYSFILE record for a system.
 */
s_find(sp, system)
register struct srec *sp;
char *system;
{
	s_openr();
	while (s_read(sp))
		if (strncmp(system, sp->s_name, SNLN) == 0) {
			s_close();
			return(TRUE);
		}
	s_close();
	return(FALSE);
}

/*
 * Close sysfile.
 */
s_close()
{
	fclose(sysfile);
}

/*
 * Local open routine.
 */
FILE *
xfopen(name, mode)
register char *name, *mode;
{
	register FILE *fp;

	if ((fp = fopen(name, mode)) == NULL) {
		sprintf(bfr, "Cannot open %s (%s)", name, mode);
		xerror(bfr);
	}
	return(fp);
}

/*
 * Read header from file dir/name into *hp.
 * Return (FILE *) if header okay, else NULL.
 */
FILE *
hread(hp, dir, name)
register struct hbuf *hp;
char *dir, *name;
{
	register FILE *fp;
	register int len;

	sprintf(bfr, "%s/%s", dir, name);
	if ((fp = fopen(bfr, "r")) == NULL)
		return(NULL);
	if (getc(fp) != PROTO)
		return(NULL);
	fgets(hp->name, NAMELEN+1, fp);	/* file name */
	if (!nstrip(hp->name))
		return(NULL);
	fgets(hp->nbuf, BUFLEN, fp);	/* newsgroup list */
	if (!nstrip(hp->nbuf))
		return(NULL);
	ngcat(hp->nbuf);
	fgets(hp->path, BUFLEN, fp);	/* source path */
	if (!nstrip(hp->path))
		return(NULL);
	/* strip off sys! from front of path. */
	strcpy(bfr, SYSNAME);
	if (strncmp(bfr, hp->path, (len = strlen(bfr))) == 0 && index(NETCHRS,hp->path[len]))
		strcpy(hp->path, &(hp->path[len+1]));
	fgets(hp->date, DATELEN, fp);	/* date */
	if (!nstrip(hp->date))
		return(NULL);
	fgets(hp->title, BUFLEN, fp);	/* title */
	if (!nstrip(hp->title))
		return(NULL);
	return(fp);
}

/*
 * Write header at 'hp' on stream 'fp' in network format.
 */
hwrite(hp, fp)
register struct hbuf *hp;
register FILE *fp;
{
	ngdel(strcpy(bfr, hp->nbuf));
	fprintf(fp, "%c%s\n%s\n%s!%s\n%s\n%s\n",
		PROTO, hp->name, bfr, SYSNAME, hp->path, hp->date, hp->title);
}
