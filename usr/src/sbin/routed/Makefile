#
# Copyright (c) 1983 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.2.2 (2.11BSD) 2025/11/25
#
ALL=	routed
OBJS=	af.o if.o input.o main.o output.o startup.o tables.o timer.o \
	trace.o inet.o
CFLAGS=	-O
LDFLAGS=-i

all:	${ALL}

routed: ${OBJS}
	${CC} ${LDFLAGS} -o routed ${LDFLAGS} ${OBJS}

install: routed
	install -c -s routed /sbin

clean:
	rm -f routed *.o core a.out t.c linterrs typescript errs

${OBJS}: defs.h af.h defs.h interface.h table.h trace.h \
	/usr/include/protocols/routed.h
