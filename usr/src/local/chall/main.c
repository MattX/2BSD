static char *RCSid = ": /mnt/staff/adams/Cprogs/Chall/RCS/main.c,v 1.5 83/04/21 07:13:03 adams Exp $";

/* Modification history --
 *
 * :	/mnt/staff/adams/Cprogs/Chall/RCS/main.c,v $
 * Revision 1.5  83/04/21  07:13:03  adams
 * Added multiple directories.
 * 
 * Revision 1.4  83/04/21  06:59:56  adams
 * indented right.
 * R
 * 
 * Revision 1.3  83/04/11  04:52:30  adams
 * Added su-checking.
 * 
 * Revision 1.2  83/04/11  04:47:38  adams
 * changed all the asserts to my new format.
 * 
 * Revision 1.1  83/04/11  03:30:24  adams
 * Initial revision
 * 
 *
 */

/* chall -- change the ownership and group ownership of a directory
 * and all its descendants.  (I wonder if that is spelled right.. hmmm)
 * By the way:
	There is a reason I don't just use find(1).  This
	runs (seriously) 100 times as fast.  Really.
 */
/*
 *
 *  The line marked : and the comment marked : are courtesy
 *  RCS, the Revision Control System written and distributed by 
 *  Walter F. Tichy at University of Purdue.  I have found it invaluable
 *  in generating code, and recommend it whole-heartedly.  For more
 *  information, contact the author.  (purdue!wft or Tichy@purdue)
 * 
 */

/* 
 * original work by Rob Adams, (uiucdcs!adams) on 11 April.
 *
 */
#include <stdio.h>
#include <pwd.h>
#include <grp.h>

int hflag;

main (argc, argv)
int     argc;
char   *argv[];


{

 /* some defs here later. */
    int     uid,
            gid;
    int     i;
    int     walked;
    struct passwd  *pswd,
                   *getpwnam ();
    struct group   *grp,
                   *getgrnam ();


 /* Were we called correctly? */
    assert ((argc >= 4), 's', "Usage: chall owner group file");

#ifdef notyet		/* cant do this yet cause cant chdir("..") */
    if (!strcmp(argv[1], "-h")) {	/* follow symlinks */
	hflag++;
	argc--, argv++;
    }
#endif

 /* parse the args, first the name (argv[1]) */
    if (!strcmp(argv[1], "-1"))
	uid = -1;
    else {
	assert (((getuid () == 0) || (geteuid () == 0)), 's', "You must be SU to change ownership");
	setpwent ();
	pswd = getpwnam (argv[1]);
	endpwent ();
	assert ((pswd != (struct passwd *) 0), 's', "Cant get user info");
	uid = pswd -> pw_uid;
   }

 /* now comes the group. */
    if (!strcmp(argv[2], "-1"))
	gid = -1;
    else {
	setgrent ();
	grp = getgrnam (argv[2]);
	endgrent ();
	assert ((grp != (struct group  *) 0), 's', "Cant get group info");
	gid = grp -> gr_gid;
    }

 /* now, lets just pass everything off to the tree walker. */
    for (i = 3; i < argc; i++) {
	walked = walk (uid, gid, argv[i]);
	assert (walked == 0, 's', "Something is wrong, but I don't know what");
    }
    exit (walked);	/* I know, walked is guaranteed to be 0 here */
}
