

/*******************************************************************
 *
 *  Remote Access Package,  System Calls
 *
 *******************************************************************
 *
 *  Copyright (c) 1984 by   Walter F. Tichy and Zuwang Ruan
 *                          Department of Computer Sciences
 *                          Purdue University
 *                          West Lafayette, IN 47907
 *
 *  All rights reserved.  No parts of this software may be sold or
 *  distributed in any form or by any means without the prior written
 *  permission of the authors.
 *
 *******************************************************************/

static char rcsid[] = "$Header: /a/ruan/src/RCS/syscall.c,v 1.1 84/06/19 22:39:55 ruan Exp $";

/* $Log:	syscall.c,v $
 * Revision 1.1  84/06/19  22:39:55  ruan
 * Initial revision
 *  */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>

#include "ra.h"
#include "local.h"

char    Host[NameLeng], Name[cchMax];
char    Host1[NameLeng], Name1[cchMax];
extern  int errno;

open(sb, flags, mode)
char    *sb;
int     flags, mode;
{
    int     fdPseudo, fdHid, code;

    if (IsLocal(sb, Host, Name)) {              /* local */
	if ((code = open_l(sb, flags, mode)) != -1)
	    return(code);
	if ((errno != ENOENT) || NoRemoteLink(sb, Host, Name))
	    return(-1);
    }
						/* remote */
    fdPseudo = HoldSlot;
    if (fdPseudo < 0)
	return(-1);
    if (((fdHid = getHostConn(Host)) < 0) ||
	(SendCommand(fdHid, OPEN, 1, Name, "", 0, flags, mode, fdPseudo) < 0) ||
	(RecvResult(fdHid) < 0)) {
	(void)close_l(fdPseudo);
	return(-1);
    }
    if ((setmap(fdPseudo, fdHid) < 0)) {
	(void)close_l(fdPseudo);
	return(-1);
    }
    return(fdPseudo);
}

creat(sb, mode)
char    *sb;
int     mode;
{
    int     fdPseudo, fdHid, code;

    if (IsLocal(sb, Host, Name)) {              /* local */
	if ((code = creat_l(sb, mode)) != -1)
	    return(code);
	if ((errno != ENOENT) || NoRemoteLink(sb, Host, Name))
	    return(-1);
    }
						/* remote */
    fdPseudo = HoldSlot;
    if (fdPseudo < 0)
	return(-1);
    if (((fdHid = getHostConn(Host)) < 0) ||
	(SendCommand(fdHid, CREAT, 1, Name, "", 0, mode, fdPseudo, 0) < 0) ||
	(RecvResult(fdHid) < 0) ||
	(setmap(fdPseudo, fdHid) < 0)) {
	(void)close_l(fdPseudo);
	return(-1);
    }
    return(fdPseudo);
}

stat(sb, pstatbuf)
char    *sb;
struct  stat    *pstatbuf;
{
    int     fdHid;

    if (IsLocal(sb, Host, Name)) {              /* local */
	if (stat_l(sb, pstatbuf) == 0)
	    return(0);
	if ((errno != ENOENT) || NoRemoteLink(sb, Host, Name))
	    return(-1);
    }
						/* remote */
    if (((fdHid = getHostConn(Host)) < 0) ||
	(SendCommand(fdHid, STAT, 1, Name, "", 0, 0, 0, 0) < 0) ||
	(RecvResult(fdHid) < 0) ||
	(RecvStat(fdHid, pstatbuf) < 0))
	return(-1);
    return(0);
}

fstat(fd, pstatbuf)
int     fd;
struct  stat    *pstatbuf;
{
    int     fdHid;

    if ((fdHid = getmap(fd)) == -1)
	return(fstat_l(fd, pstatbuf));

    if ((SendCommand(fdHid, FSTAT, 0, "", "", fd, 0, 0, 0) < 0) ||
	(RecvResult(fdHid) < 0) ||
	(RecvStat(fdHid, pstatbuf) < 0))
	return(-1);
    return(0);
}

lstat(sb, pstatbuf)
char    *sb;
struct  stat    *pstatbuf;
{
    int     fdHid;

    if (IsLocal(sb, Host, Name)) {              /* local */
	if (lstat_l(sb, pstatbuf) == 0)
	    return(0);
	if ((errno != ENOENT) || NoRemoteLink(sb, Host, Name))
	    return(-1);
    }
						/* remote */
    if (((fdHid = getHostConn(Host)) < 0) ||
	(SendCommand(fdHid, LSTAT, 1, Name, "", 0, 0, 0, 0) < 0) ||
	(RecvResult(fdHid) < 0) ||
	(RecvStat(fdHid, pstatbuf) < 0))
	return(-1);
    return(0);
}

readlink(sb, rgch, cch)
char    *sb, *rgch;
int     cch;
{
    int     fdHid, code;

    if (IsLocal(sb, Host, Name)) {              /* local */
	if ((code = readlink_l(sb, rgch, cch)) != -1)
	    return(code);
	if ((errno != ENOENT) || NoRemoteLink(sb, Host, Name))
	    return(-1);
    }
						/* remote */
    if (((fdHid = getHostConn(Host)) < 0) ||
	(SendCommand(fdHid, READLINK, 1, Name, "", 0, cch, 0, 0) < 0))
	return(-1);
    return(RecvLink(fdHid, rgch, cch));
}

access(sb, mode)
char    *sb;
int     mode;
{
    int     fdHid;

    if (IsLocal(sb, Host, Name)) {              /* local */
	if (access_l(sb, mode) == 0)
	    return(0);
	if ((errno != ENOENT) || NoRemoteLink(sb, Host, Name))
	    return(-1);
    }
						/* remote */
    if (((fdHid = getHostConn(Host)) < 0) ||
	(SendCommand(fdHid, ACCESS, 1, Name, "", 0, mode, 0, 0) < 0))
	return(-1);
    return(RecvResult(fdHid));
}

chdir(sb)
char    *sb;
{
    int     fdHid;

    if (IsLocal(sb, Host, Name)) {              /* local */
	if (chdir_l(sb) == 0)
	    return(0);
	if ((errno != ENOENT) || NoRemoteLink(sb, Host, Name))
	    return(-1);
    }
						/* remote */
    if (((fdHid = getHostConn(Host)) < 0) ||
	(SendCommand(fdHid, CHDIR, 1, Name, "", 0, 0, 0, 0) < 0))
	return(-1);
    return(RecvResult(fdHid));
}

rmdir(sb)
char    *sb;
{
    int     fdHid;

    if (IsLocal(sb, Host, Name)) {              /* local */
	if (rmdir_l(sb) == 0)
	    return(0);
	if ((errno != ENOENT) || NoRemoteLink(sb, Host, Name))
	    return(-1);
    }
						/* remote */
    if (((fdHid = getHostConn(Host)) < 0) ||
	(SendCommand(fdHid, RMDIR, 1, Name, "", 0, 0, 0, 0) < 0))
	return(-1);
    return(RecvResult(fdHid));
}

chmod(sb, mode)
char    *sb;
int     mode;
{
    int     fdHid;

    if (IsLocal(sb, Host, Name)) {              /* local */
	if (chmod_l(sb, mode) == 0)
	    return(0);
	if ((errno != ENOENT) || NoRemoteLink(sb, Host, Name))
	    return(-1);
    }
						/* remote */
    if (((fdHid = getHostConn(Host)) < 0) ||
	(SendCommand(fdHid, CHMOD, 1, Name, "", 0, mode, 0, 0) < 0))
	return(-1);
    return(RecvResult(fdHid));
}

fchmod(fd, mode)
int     fd, mode;
{
    int     fdHid;

    if ((fdHid = getmap(fd)) == -1)
	return(fchmod_l(fd, mode));

    if (SendCommand(fdHid, FCHMOD, 0, "", "", fd, mode, 0, 0) < 0)
	return(-1);
    return(RecvResult(fdHid));
}


mkdir(sb, mode)
char    *sb;
int     mode;
{
    int     fdHid;

    if (IsLocal(sb, Host, Name)) {              /* local */
	if (mkdir_l(sb, mode) == 0)
	    return(0);
	if ((errno != ENOENT) || NoRemoteLink(sb, Host, Name))
	    return(-1);
    }
						/* remote */
    if (((fdHid = getHostConn(Host)) < 0) ||
	(SendCommand(fdHid, MKDIR, 1, Name, "", 0, mode, 0, 0) < 0))
	return(-1);
    return(RecvResult(fdHid));
}

truncate(sb, leng)
char    *sb;
int     leng;
{
    int     fdHid;

    if (IsLocal(sb, Host, Name)) {              /* local */
	if (truncate_l(sb, leng) == 0)
	    return(0);
	if ((errno != ENOENT) || NoRemoteLink(sb, Host, Name))
	    return(-1);
    }
						/* remote */
    if (((fdHid = getHostConn(Host)) < 0) ||
	(SendCommand(fdHid, TRUNC, 1, Name, "", 0, leng, 0, 0) < 0))
	return(-1);
    return(RecvResult(fdHid));
}

ftruncate(fd, leng)
int     fd, leng;
{
    int     fdHid;

    if ((fdHid = getmap(fd)) == -1)
	return(ftruncate_l(fd, leng));

    if (SendCommand(fdHid, FTRUN, 0, "", "", fd, leng, 0, 0) < 0)
	return(-1);
    return(RecvResult(fdHid));
}

unlink(sb)
char    *sb;
{
    int     fdHid;

    if (IsLocal(sb, Host, Name)) {              /* local */
	if (unlink_l(sb) == 0)
	    return(0);
	if ((errno != ENOENT) || NoRemoteLink(sb, Host, Name))
	    return(-1);
    }
						/* remote */
    if (((fdHid = getHostConn(Host)) < 0) ||
	(SendCommand(fdHid, UNLINK, 1, Name, "", 0, 0, 0, 0) < 0))
	return(-1);
    return(RecvResult(fdHid));
}

link(sb1, sb2)
char    *sb1, *sb2;
{
    int     fdHid1, fdHid2, code1, code2;

    code1 = IsLocal(sb1, Host1, Name1);
    code2 = IsLocal(sb2, Host, Name);
    if (code1 && code2) {                       /* both local */
	if (link_l(sb1, sb2) == 0)
	    return(0);
	if (errno != ENOENT)
	    return(-1);
    }
    if (code1)
	code1 = NoRemoteLink(sb1, Host1, Name1);
    if (code2)
	code2 = NoRemoteLink(sb2, Host, Name);
    if (code1 != code2) {
	errno =EXHOST;
	return(-1);
    }
    if (code1 && code2)
	return(-1);
						/* both remote */
    if (((fdHid1 = getHostConn(Host1)) < 0) ||
	((fdHid2 = getHostConn(Host)) < 0))
	return(-1);
    if (fdHid1 != fdHid2) {
	errno = EXHOST;
	return(-1);
    }
    if (SendCommand(fdHid1, LINK, 2, Name1, Name, 0, 0, 0, 0) < 0)
	return(-1);
    return(RecvResult(fdHid1));
}

symlink(sb1, sb2)
char    *sb1, *sb2;
{
    int     fdHid;

    if (IsLocal(sb2, Host, Name)) {             /* local */
	if (symlink_l(sb1, sb2) == 0)
	    return(0);
	if ((errno != ENOENT) || NoRemoteLink(sb2, Host, Name))
	    return(-1);
    }
						/* remote */
    if (((fdHid = getHostConn(Host)) < 0) ||
	(SendCommand(fdHid, SYMLINK, 2, sb1, Name, 0, 0, 0, 0) < 0))
	return(-1);
    return(RecvResult(fdHid));
}

rename(sb1, sb2)
char    *sb1, *sb2;
{
    int     fdHid1, fdHid2, code1, code2;

    code1 = IsLocal(sb1, Host1, Name1);
    code2 = IsLocal(sb2, Host, Name);
    if (code1 && code2) {                       /* both local */
	if (rename_l(sb1, sb2) == 0)
	    return(0);
	if (errno != ENOENT)
	    return(-1);
    }
    if (code1)
	code1 = NoRemoteLink(sb1, Host1, Name1);
    if (code2)
	code2 = NoRemoteLink(sb2, Host, Name);
    if (code1 != code2) {
	errno = EXHOST;
	return(-1);
    }
    if (code1 && code2)
	return(-1);
						/* both remote */
    if (((fdHid1 = getHostConn(Host1)) < 0) ||
	((fdHid2 = getHostConn(Host)) < 0))
	return(-1);
    if (fdHid1 != fdHid2) {
	errno = EXHOST;
	return(-1);
    }
    if (SendCommand(fdHid1, RENAME, 2, Name1, Name, 0, 0, 0, 0) < 0)
	return(-1);
    return(RecvResult(fdHid1));
}


read(fd, rgch, cch)
int     fd, cch;
char    *rgch;
{
    int     fdHid;

    if ((fdHid = getmap(fd)) == -1)
	return(read_l(fd, rgch, cch));

    if (cch == 0)
	return(0);
    if (cch < 0)
	return(-1);

    if (SendCommand(fdHid, READ, 0, "", "", fd, cch, 0, 0) < 0)
	return(-1);
    return(RecvData(fdHid, rgch, cch));
}

write(fd, rgch, cch)
int     fd, cch;
char    *rgch;
{
    int    fdHid;

    if ((fdHid = getmap(fd)) == -1)
	return(write_l(fd, rgch, cch));

    if (cch == 0)
	return(0);
    if (cch < 0)
	return(-1);

    if (SendCommand(fdHid, WRITE, 0, "", "", fd, cch, 0, 0) < 0)
	return(-1);
    return(SendData(fdHid, rgch, cch));
}

lseek(fd, offset, base)
int     fd, offset, base;
{
    int    fdHid;

    if ((fdHid = getmap(fd)) == -1)
	return(lseek_l(fd, offset, base));

    if (SendCommand(fdHid, LSEEK, 0, "", "", fd, offset, base, 0) < 0)
	return(-1);
    return(RecvResult(fdHid));
}

flock(fd, operation)
int     fd, operation;
{
    int    fdHid;

    if ((fdHid = getmap(fd)) == -1)
	return(flock_l(fd, operation));

    if (SendCommand(fdHid, FLOCK, 0, "", "", fd, operation, 0, 0) < 0)
	return(-1);
    return(RecvResult(fdHid));
}

fsync(fd)
int     fd;
{
    int    fdHid;

    if ((fdHid = getmap(fd)) == -1)
	return(fsync_l(fd));

    if (SendCommand(fdHid, FSYNC, 0, "", "", fd, 0, 0, 0) < 0)
	return(-1);
    return(RecvResult(fdHid));
}

dup(fd)
int     fd;
{
    int     fdHid;
    int     fd1;

    fd1 = dup_l(fd);
    if ((fd1 < 0) || ((fdHid = getmap(fd)) == -1))
	return(fd1);
    if ((SendCommand(fdHid, DUP, 0, "", "", fd, fd1, 0, 0) < 0) ||
	(RecvResult(fdHid) < 0)) {
	(void)close_l(fd1);
	return(-1);
    }
    return(fd1);
}

dup2(fd, fd1)
int     fd, fd1;
{
    int     fdHid;
    int     code;

    code = dup2_l(fd, fd1);
    if ((code < 0) || ((fdHid = getmap(fd)) == -1))
	return(code);

    if ((SendCommand(fdHid, DUP, 0, "", "", fd, fd1, 0, 0) < 0) ||
	(RecvResult(fdHid) < 0)) {
	(void)close_l(fd1);
	return(-1);
    }
    return(0);
}

close(fd)
int     fd;
{
    int     fdHid;

    if ((fdHid = getmap(fd)) == -1)
	return(close_l(fd));

    if ((SendCommand(fdHid, CLOSE, 0, "", "", fd, 0, 0, 0) < 0) ||
	(RecvResult(fdHid) < 0))
	return(-1);
    (void)close_l(fd);
    return(0);
}

umask(newmask)
int     newmask;
{
    int     oldmask, i, fd;

    oldmask = umask_l(newmask);
    for (i = 0; i < nHost; i++)
	if ((fd = ConnOn(i)) >= 0)
	    if ((SendCommand(fd, UMASK, 0, "", "", newmask, 0, 0, 0) < 0) ||
		(RecvResult(fd) != oldmask))
		goto fail;
    return(oldmask);

fail:
    errno = EINMASK;
    return(-1);
}

fork()
{
    int     pid, i, j, fdHid;
    int     newport = Highest - 1;
    int     fdOld[nHost], fdNew[nHost], fdConn[nHost];
    int     rport[nHost];
    int     mpFdHst[NOFILE];

    for (i = 0; i < nHost; i++)
	fdOld[i] = -1;

    for (j = 0; j < NOFILE; j++)
	if (((fdHid = getmap(j)) == -1) ||
	    ((i = GetHst(fdHid)) == -1))
	    mpFdHst[j] = -1;
	else {
	    mpFdHst[j] = i;                 /* remember remote host */
	    fdOld[i] = fdHid;               /* of each pseudo fd */
	}

    for (i = 0; i < nHost; i++)             /* for each host */
	if (fdOld[i] == -1)                 /* if not active */
	    fdNew[i] = -1;
	else {                              /* if active */
	    newport--;                      /* get a new port for child */
	    if (((fdNew[i] = bindaport(&newport)) < 0) ||
		((SendCommand(fdOld[i], FORK, 0, "", "", newport, 0, 0, 0) < 0)) ||
		((rport[i] = RecvResult(fdOld[i])) == -1))
		return(-1);
	}
    pid = fork_l();
    if (pid < 0)
	return(-1);

    if (pid == 0) {                         /* child does the following */
	for (i = 0; i < nHost; i++)
	    if (fdNew[i] != -1) {           /* for each active remote host */
		(void)close_l(fdOld[i]);    /* close old connection */
					    /* listen to new connection */
		fdConn[i] = listenNew(i, fdNew[i], rport[i]);
		(void)close_l(fdNew[i]);
	    } else
		fdConn[i] = -1;

	for (j = 0; j < NOFILE; j++)        /* remap pseudo fd */
	    if (mpFdHst[j] != -1) {
		if (setmap(j, fdConn[mpFdHst[j]]) < 0)
		    exit(1);
	    }
	return(0);

    } else {                                /* parent */
	for (i = 0; i < nHost; i++)
	    if (fdNew[i] != -1)
		(void)close_l(fdNew[i]);
	return(pid);
    }
}

fcntl(fd, cmd, arg)
int     fd, cmd, arg;
{
    if (getmap(fd) == -1)                   /* not pseudo */
	return(fcntl_l(fd, cmd, arg));
    errno = EUAVAIL;
    return(-1);
}

mknod(sb, mode, dev)
char    *sb;
int     mode, dev;
{
    if (IsLocal(sb, Host, Name))
	return(mknod_l(sb, mode, dev));
    errno = EUAVAIL;
    return(-1);
}

mount(sb1, sb2, flag)
char    *sb1, *sb2;
int     flag;
{
    if (IsLocal(sb1, Host, Name) && IsLocal(sb2, Host, Name))
	return(mount_l(sb1, sb2, flag));
    errno = EUAVAIL;
    return(-1);
}

umount(sb)
char    *sb;
{
    if (IsLocal(sb, Host, Name))
	return(umount_l(sb));
    errno = EUAVAIL;
    return(-1);
}

chown(sb, owner, group)
char    *sb;
int     owner, group;
{
    if (IsLocal(sb, Host, Name))
	return(chown_l(sb, owner, group));
    errno = EUAVAIL;
    return(-1);
}

chroot(sb)
char    *sb;
{
    if (IsLocal(sb, Host, Name))
	return(chroot_l(sb));
    errno = EUAVAIL;
    return(-1);
}
