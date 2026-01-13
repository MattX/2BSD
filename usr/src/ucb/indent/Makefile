#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.5 (2.11BSD) 2025/11/25
#
CFLAGS=	-O
LDFLAGS= -i
SRCS=	indent.c io.c lexi.c parse.c pr_comment.c args.c
OBJS=	indent.o io.o lexi.o parse.o pr_comment.o args.o

indent: ${OBJS}
	cc ${LDFLAGS} ${CFLAGS} -o indent ${OBJS}

clean:
	rm -f ${OBJS} indent core

install: indent
	install -c -s -m 755 indent /usr/ucb/indent

depend:	${SRCS}
	mkdep ${CFLAGS} ${SRCS}
