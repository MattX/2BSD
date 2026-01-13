#
# Copyright (c) 1987 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.1.3	(2.11BSD)	2025/11/25
#

CFLAGS=	-O -DBSD -DDEST=\"/usr/games/lib/atc/\"
LDFLAGS= -i
YFLAGS=	-d
LIBS=	-ll -lm -lcurses -ltermcap
SRCS=	extern.c grammar.c graphics.c input.c lex.c list.c log.c \
	main.c tunable.c update.c
OBJS=	extern.o grammar.o input.o lex.o list.o log.o main.o tunable.o \
	graphics.o update.o

all: atc

atc: ${OBJS}
	${CC} ${LDFLAGS} ${OBJS} ${LIBS} -o $@

clean: FRC
	rm -f ${OBJS} core atc grammar.c y.tab.h y.tab.c lex.yy.c lex.c

depend: lex.c grammar.c FRC
	mkdep ${CFLAGS} ${SRCS}

install: FRC
	-[ -d /usr/games/lib/atc ] || mkdir /usr/games/lib/atc
	chmod 700 /usr/games/lib/atc
	chown games.staff /usr/games/lib/atc
	(cd games; install -c -o games -g staff -m 600 * /usr/games/lib/atc)
	install -c -s -o games -g staff -m 4700 atc /usr/games/hide/atc
	(cd /usr/games; rm -f atc; ln -s dm atc; chown games.staff atc)

lint: lex.c grammar.c FRC
	lint ${CFLAGS} ${SRCS}

tags: lex.c grammar.c FRC
	ctags ${SRCS}

FRC:
