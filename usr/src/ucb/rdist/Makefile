#
# Copyright (c) 1983 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.1.1 (2.11BSD GTE) 7/11/94
#
DESTDIR=
SEPFLAG= -i
SRCS =	docmd.c expand.c gram.y lookup.c main.c server.c
OBJS =	docmd.o expand.o gram.o lookup.o main.o server.o
LINT =	lint -ps
CFLAGS= -O

rdist:	${OBJS}
	${CC} ${SEPFLAG} -o rdist ${OBJS}

${OBJS}:  defs.h

clean:
	rm -f *.o gram.c errs rdist

install: rdist
	install -s -m 4751 rdist ${DESTDIR}/usr/ucb/rdist

lint:	docmd.c expand.c gram.c lookup.c main.c server.c
	${LINT} docmd.c expand.c gram.c lookup.c main.c server.c

print:	${SRCS}
	lpr -p ${SRCS} defs.h
