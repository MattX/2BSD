/* $Header: getpwinfo.c,v 1.5 85/05/20 20:01:42 rick Exp $ */
/* from: #)getpwinfo.c	5.1 (Berkeley) 7/2/83 */

#include "uucp.h"
#include <pwd.h>

/*LINTLIBRARY*/

/*
 *	get passwd file info for uid
 *
 *	return codes:  0  |  FAIL
 *
 */

guinfo(uid, name, path)
int uid;
register char *path, *name;
{
	register struct passwd *pwd;
	struct passwd *getpwuid(), *getpwnam();
	char *getlogin(), *l;

	if ((l = getlogin()) != NULL) {
		pwd = getpwnam(l);
		if (pwd->pw_uid == uid || *l == 'U')
			goto setup;
	}
	if ((pwd = getpwuid(uid)) == NULL) {
		/* can not find uid in passwd file */
		*path = '\0';
		return FAIL;
	}

    setup:
	strcpy(path, pwd->pw_dir);
	strcpy(name, pwd->pw_name);
	return SUCCESS;
}


/*
 *	get passwd file info for name
 *
 *	return codes:  SUCCESS  |  FAIL
 */

gninfo(name, uid, path)
char *path, *name;
int *uid;
{
	register struct passwd *pwd;
	struct passwd *getpwnam();

	if ((pwd = getpwnam(name)) == NULL) {
		/* can not find name in passwd file */
		*path = '\0';
		return FAIL;
	}

	strcpy(path, pwd->pw_dir);
	*uid = pwd->pw_uid;
	return SUCCESS;
}
