#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.1.3 (2.11BSD) 2025/11/25
#
CFLAGS=	-O
LDFLAGS=-i

OBJS=	main.o init.o subr.o gettytab.o get_date.o

all:	getty

getty:	${OBJS}
	${CC} ${LDFLAGS} -o getty ${OBJS}

install: all
	install -c -s -m 555 -o root -g staff getty /usr/libexec/getty

clean:
	rm -f getty ${OBJS} a.out core errs

main.o: gettytab.h
init.o: gettytab.h
subr.o: gettytab.h
