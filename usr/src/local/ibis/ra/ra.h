

/*******************************************************************
 *
 *  Remote Access Package,  Configuration Independent Definitions
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

/* $Header: /a/ruan/src/RCS/ra.h,v 1.1 84/06/19 22:41:23 ruan Exp $ */

/* $Log:	ra.h,v $
 * Revision 1.1  84/06/19  22:41:23  ruan
 * Initial revision
 *  */

#define cchMax  512

struct  command {
    int     comm;
    int     nname;
    int     arg1;
    int     arg2;
    int     arg3;
    int     arg4;
};

struct  result {
    int     code;
    int     errno;
};

struct  data  {
    int     leng;
    int     flag;
    char    data[cchMax];
};

#define Lowest  2021
#define Highest 2050
#define NameLeng 24

#define ECOMM   ECONNABORTED            /* illegal command */
#define EXHOST  EXDEV                   /* cross host link */
#define EINMASK 80                      /* inconsistnt mask */
#define EUAVAIL 81                      /* remote version unavailble */

#define ENDDATA 1

#define FORK    2
#define READ    3
#define WRITE   4
#define OPEN    5
#define CLOSE   6
#define CREAT   8
#define LINK    9
#define UNLINK  10
#define CHDIR   12
#define MKNOD   14
#define CHMOD   15
#define CHOWN   16
#define STAT    18
#define LSEEK   19
#define FSTAT   28
#define ACCESS  33
#define LSTAT   40
#define DUP     41
#define SYMLINK 57
#define READLINK 58
#define UMASK   60
#define FSYNC   95
#define FCHMOD  124
#define RENAME  128
#define TRUNC   129
#define FTRUN   130
#define FLOCK   131
#define MKDIR   136
#define RMDIR   137

#define MIN(a, b)   (((a) < (b))? (a): (b))
#define Error(mesg) { perror(mesg); exit(1); }
#define ERROR(mesg) { perror(mesg); return(-1); }

#define F_PSEUDO    020000      /* file descriptor for remote object */
#define F_HIDDEN    040000      /* file descriptor for actual connection */
#define HoldSlot    open_l("/tmp/null", O_CREAT, 0666)

#define NOFILE      20          /* max number of open files */
