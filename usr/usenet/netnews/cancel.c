#include "params.h"
static char *sccsid = "@(#)cancel.c	1.2	9/14/80";

/*
 * Cancel a news item.
 */
cancel(file)
register char *file;
{
	char fname[BUFLEN];
	struct nrec nrec;
	register int gone;	/* set to true if cancel works. */

	gone = FALSE;
	getuser();
	lock();
	n_openm();
	while (n_read(&nrec)) {
		if (strcmp(file, nrec.n_file) == 0) {
			if (pathmatch(file)) {
				sprintf(fname, "%s/%s", NEWSD, file);
				canned(fname, file);
				if (unlink(fname)) {
					sprintf(bfr,
						"unlink of %s failed", file);
					xerror(bfr);
				}
				bitup = TRUE;  /*cause bit map update. */
				gone = TRUE;
				goto contin;
			} else
				printf("Not contributor\n");
		}
		n_write(&nrec);
    contin:;
	}

	n_close();
	unlock();
	return(gone);
}

/*
 * Path name match check.
 */
static
pathmatch(file)
register char *file;
{
	struct hbuf h;
	register FILE *fp;

	if (uid == ROOTID)
		return(TRUE);
	if ((fp = hread(&h, NEWSD, file)) == NULL)
		return(FALSE);
	fclose(fp);
	return(strcmp(h.path, header.path) == 0);
}
