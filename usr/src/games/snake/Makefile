#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.1.2 (2.11BSD) 2025/11/25
#
CFLAGS=	-O -DCHECKBUSY
LIBS=	-lm -ltermcap
BIN=	/usr/games
LIB=	/usr/games/lib
USER=	daemon
UTILS=	snscore busy
OBJS=	snake.o move.o
ALL=	snake ${UTILS}
LDFLAGS= -i

all:	${ALL}

snake:	$(OBJS)
	cc ${LDFLAGS} $(OBJS) -o snake $(LIBS)

snake.o move.o:snake.h

snscore: snscore.c
	cc ${LDFLAGS} $(CFLAGS) snscore.c -o snscore

busy:	busy.c
	cc ${LDFLAGS} $(CFLAGS) busy.c -o busy

install: all
	install -c -s -m 4755 -o ${USER} snake ${BIN}/snake
	install -c -s -m 755 -o ${USER} snscore ${BIN}/snscore
	install -c -s -m 755 -o ${USER} busy ${LIB}/busy
	cat /dev/null >> $(LIB)/snakerawscores
	chmod 644 $(LIB)/snakerawscores
	chown $(USER) $(LIB)/snakerawscores

clean:
	rm -f *.o ${ALL}
