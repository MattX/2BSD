#include "params.h"
#define	READ	0
#define	CHG	1
static char *sccsid = "@(#)subscr.c	1.3	9/14/80";

/*
 * Change a user's subscription list.
 */
subscr()
{
	struct urec urec;
	register int mode, newuser;

	mode = READ;
	if (*header.nbuf) {
		mode = CHG;
		bitup = TRUE;
		ngfcheck(TRUE);
		/*
		 * Administrative nit:
		 * force user to subscribe to "ADMSUB".
		 */
		ngcat(strcpy(bfr, ADMSUB));
		if (!ngmatch(bfr, header.nbuf)) {
			if (strlen(header.nbuf)+strlen(bfr) >= BUFLEN)
				xerror("Newsgroup list too long");
			strcat(header.nbuf, bfr);
		}
		/* End of administrative nit. */
	}
	lock();
	if (mode == READ)
		u_openr();
	else
		u_openm();
	newuser = TRUE;
	while (u_read(&urec)) {
		if (uid == urec.u_uid) {
			newuser = FALSE;
			if (mode == READ)
				break;
			strcpy(urec.u_nglist, header.nbuf);
		}
		if (mode == CHG)
			u_write(&urec);
	}
	if (mode == READ) {
		if (newuser)
			strcpy(urec.u_nglist, DFLTSUB);
		ngdel(urec.u_nglist);
		printf("Subscription list = %s\n", urec.u_nglist);
	}
	else if (newuser) {
		urec.u_uid = uid;
		getdtln();
		urec.u_date = header.ndate;
		strcpy(urec.u_nglist, header.nbuf);
		u_write(&urec);
	}
	u_close();
	unlock();
}
