/*
 *			S A M P L E . C 
 *
 * $Revision: 1.2 $
 *
 * $Log:	sample.c,v $
 * Revision 1.2  83/12/16  00:10:07  dpk
 * Added distinctive RCS header
 * 
 */
#ifndef lint
static char RCSid[] = "@(#)$Header: sample.c,v 1.2 83/12/16 00:10:07 dpk BRL $";
#endif

/* wout -- write out file or region */

/* aflag = 0	Write buffer to file */
/* aflag = 1	Append buffer to file */
/* aflag = 2	Write region to file */
/* aflag = 3	Append region to file */

wout(name,aflag)

char *name;
int aflag;
{
	char backb[128];		/* buffer for name of backup file */
	char exist;
	struct stat inode;		/* status buffer */

	inode.st_mode = 0666;		/* default mode, rw-rw-rw */
	inode.st_nlink = 1;		/* number of links */
	exist = !stat(name,&inode);	/* get old file modes */
					/* don't care if file not there */
	if (aflag & 1) {
		outfile = fopen (&outbuf,name,"a");
		if (outfile) goto wfile1; /* if file was there */
	}
	if (inode.st_nlink != 1) {
		if ((i = gyn("Overwrite existing file (y), or make new file (n)? ")) < 0)
			return(0);
		else if (i > 0) {
			sprintf(backb, "cp %s{,%s}; echo \"\"\n", name, BACKSTR);
			fputs(backb, cshin);
			(void) fgets(backb, 16, cshout);
			goto wfile;
		}
	}
	if (exist) {
		if (access(name, 2) < 0) {
			error(WARN,errno,name);
			return(0);
		}
		strcpy(backb, name);
		strcat(backb, BACKSTR);
		unlink(backb);
		if (link(name, backb)) {
			if (!crashes) {
				prompt1("[No backup file created]");
				mflush(stdout);
				sleep(2);
			}
		} else		
			unlink(name);
	}
wfile:	outfile = fopen (&outbuf,name,"w");
wfile1:	if (outfile == NULL) {
		error(WARN,errno,name);
		return(0);
	}
	if (!crashes) {
		prompt1("Writing %s", name);
		mflush(stdout);
	}
	unlink(emdir);
	if (*emkeys)
		unlink(emkeys);		/* To avoid potential disaster */
	wbuf(outfile, aflag >= 2);
	mclose(outfile);
	if (PICTMD && aflag < 2) {
		minpict = nlines;
		maxpict = 0;
	}
	if (exist) {
		chmod(name,inode.st_mode);	/* set modes right */
		chown(name,inode.st_uid,inode.st_gid);	/* works for root */
	}
	return(1);
}
