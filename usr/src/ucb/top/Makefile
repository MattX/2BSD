#
# @(#)Makefile  1.2 (2.11BSD) 2025/11/26
#

CFLAGS= -O
LDFLAGS= -i
MAN=	top.0
MANSRC=	top.1

all: top ${MAN}

top: top.o psdb.o
	cc ${LDFLAGS} -o top top.o psdb.o -lcurses -ltermcap

top.o: top.c psdb.h
	cc ${CFLAGS} -c top.c

psdb.o: psdb.c psdb.h
	cc ${CFLAGS} -c psdb.c

top.0: top.1
	/usr/man/manroff ${MANSRC} > ${MAN}

install: all
	install -c -s -o root -g kmem -m 2755 top /usr/ucb
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat1

clean:
	rm -f top.o psdb.o top ${MAN}
