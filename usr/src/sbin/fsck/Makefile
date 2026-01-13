#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.8.3 (2.11BSD) 2025/11/25
#
CFLAGS=	-O
LDFLAGS=-i
XSTR=	/usr/ucb/xstr

# The program itself
#
PROG=	fsck

# Sources
#
SRCS=	dir.c inode.c main.c pass1.c pass1b.c pass2.c \
	pass3.c pass4.c pass5.c setup.c utilities.c

# Objects
#
OBJS=	${LOCOBJS}
LOCOBJS=dir.o inode.o main.o pass1.o pass1b.o pass2.o \
	pass3.o pass4.o pass5.o setup.o utilities.o

# Header files
#
HDRS=	fsck.h

all:	${PROG}

${PROG}: ${OBJS} ${HDRS} strings.o
	cc ${LDFLAGS} -o ${PROG} ${OBJS} strings.o

.c.o:
	cc -E ${CFLAGS} $*.c | ${XSTR} -c -
	cc ${CFLAGS} -c x.c
	mv -f x.o $*.o
	rm -f x.c

strings.o: strings
	${XSTR}
	cc -c xs.c
	mv -f xs.o strings.o
	rm -f xs.c

install: ${PROG}
	install -c -s -m 755 ${PROG} /sbin/${PROG}

clean:
	rm -f a.out core ${OBJS} ${PROG} strings.o x.c xs.c strings

lint:
	lint ${SRCS}

depend:	${SRCS}
	mkdep ${CFLAGS} ${SRCS}
