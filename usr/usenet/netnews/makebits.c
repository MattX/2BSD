#include "params.h"
#if USIZE <= 0
# define BTSZ 8192
#else
# define BTSZ ((USIZE+7)/8)
#endif
static char *sccsid = "@(#)makebits.c	1.3	9/22/80";

/*
 * Reconstruct the bit file.
 */
makebits()
{
	struct nrec nindtemp[NNREC+1];
	register struct nrec *nptr;
	struct urec urec;
	char bitbuf[BTSZ];	/* incore bit file */
	register int i, eofflag;
	register FILE *bits;

	/* set all bits to "up to date" (0) */
	for (i = 0; i < BTSZ; i++)
		bitbuf[i] = 0;

	lock();
	n_openr();	/* open nindex */
	for (eofflag = FALSE; !eofflag;) {
		/* loop once per section of nindex file. */
		nptr = nindtemp;
		eofflag = TRUE;
		while (n_read(nptr)) {
			if (++nptr >= &nindtemp[NNREC]) {
				eofflag = FALSE;
				break;
			}
		}
		nptr->n_date = -1L;

		/*
		 * Got this piece of nindex in, now read uindex
		 * and set all of the appropriate bits.
		 */
		u_openr();
		while (u_read(&urec)) {
			for (nptr = nindtemp; nptr->n_date > 0L; nptr++) {
				/* check dates. */
				if (nptr->n_date <= urec.u_date)
					continue;

				/* check for newsgroup match. */
				if (!ngmatch(nptr->n_nglist, urec.u_nglist))
					continue;

				bitbuf[urec.u_uid>>3] |= (1<<(urec.u_uid&07));
				break;
			}
		}
		u_close();
	}
	n_close();

	/* write out the bits. */
	bits = xfopen(BITFILE, "w");
	fwrite(bitbuf, 1, BTSZ, bits);
	fclose(bits);
#ifdef PWB
	chmod(BITFILE, 0644);
	if (duid != 0)
		chown(BITFILE, ((dgit << 8) | duid));
#else
	if (duid != 0)
		chown(BITFILE, duid, dgid);
#endif
	unlock();
}

/*
 * Force uindex to have the same users as /etc/passwd.
 */
users(add_time)
time_t add_time;
{
	char usebits[BTSZ];
	register unsigned i;
	register struct passwd *pw;
	struct urec urec;

	for (i = 0; i < BTSZ; i++)
		usebits[i] = 0;

	/* scan /etc/passwd */
	while ((pw=getpwent()) != NULL) {
		if ((i = (unsigned) pw->pw_uid) >= (unsigned) USIZE)
			xerror("passwd uid too big");
		usebits[i>>3] |= (1<<(i&07));
	}
	endpwent();

	/* scan uindex to delete old users. */
	lock();
	u_openm();
	while (u_read(&urec))
		if (usebits[urec.u_uid>>3] & (1<<(urec.u_uid&07))) {
			usebits[urec.u_uid>>3] &= ~(1<<(urec.u_uid&07));
			u_write(&urec);
		}

	/* scan usebits to append new users. */
	urec.u_date = add_time;
	strcpy(urec.u_nglist, DFLTSUB);
	for (i = 0; i < USIZE; i++)
		if (usebits[i>>3] & (1<<(i&07))) {
			urec.u_uid = i;
			u_write(&urec);
		}
	u_close();
	unlock();
}
