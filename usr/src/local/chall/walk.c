static char *RCSid = "$Header: walk.c,v 1.3 83/11/09 03:06:32 lepreau Exp $";

/*
 * walk()
 *	    This guy does all the recursion.  It should propagate
 *	    errors backwards, but I don't know if I will do that tonight.
 *
 */
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <stdio.h>

walk (uid, gid, filename)
int     uid,
        gid;
char   *filename;
{
    struct stat status;
    int     stated;
    int     isdir;
    int     walked;
    int     chowned = 0;
    DIR	    *dot;
    register struct direct *dp;

 /* see if filename is a non-directory or directory */

    stated = lstat (filename, &status);
    assert ((stated == 0),'p',"chall: lstat()");
#ifdef S_IFLNK
    if ((status.st_mode & S_IFLNK) == S_IFLNK)	/* symlink */
        return (0);
#endif
    isdir = ((status.st_mode & S_IFMT) == S_IFDIR);
    if (!isdir) {
	chowned = chown(filename, uid, gid);
	assert ((chowned == 0),'p',"chall: chown()");
	return (0);
    }
 /* else */
    chdir (filename);
    chowned = chown(".", uid, gid);
    assert ((chowned == 0),'p',"chall: chown()");
    dot = opendir (".");
    assert ((dot != NULL),'p', "chall: opendir()");
    dp = readdir(dot);			/* . */
    assert(!strcmp(dp->d_name, "."), 's', "dot");
    dp = readdir(dot);			/* .. */
    assert(!strcmp(dp->d_name, ".."), 's', "dotdot");
    for (;;) {
	dp = readdir(dot);
	if (dp == NULL)		  /* EOF */
	    break;
	/*
	 * filenames are guaranteed null terminated in 4.2.
	 * Is this true of emulation package?
	 */
	walked = walk (uid, gid, dp->d_name);
	assert ((walked == 0),'s',"Something mysterious just happend");
    }
    closedir (dot);
    chdir (".."); /* this fails for symlinksd so we cant allow them yet! */
    return (0);				  /* just emptied a directory */
}
