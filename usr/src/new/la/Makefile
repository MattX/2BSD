#       Makefile        2.0 (2.11BSD) 2025/11/25

CFLAGS=	-O
LDFLAGS=-i

all: la la.0

la: la.o
	cc ${CFLAGS} ${LDFLAGS} la.o -o la

install: la la.0
	install -c -s -m 755 la /usr/new/la
	install -c -m 444 -o root -g staff la.0 /usr/new/man/cat1/la.0

la.0: la.1
	/usr/man/manroff la.1 > la.0

clean:
	rm -f la *.o la.0

depend:
tags:
lint:
