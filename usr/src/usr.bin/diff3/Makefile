#	Makefile	4.1	83/02/10

CFLAGS=	-O
ALL=	diff3

all: ${ALL}

diff3: diff3.o
	${CC} -o diff3 diff3.o

install: diff3
	install -s -o bin -g bin -m 711 diff3 $(DESTDIR)/usr/lib
	install -c -o bin -g bin -m 711 diff3.sh $(DESTDIR)/usr/bin/diff3

clean:
	rm -f diff3.o diff3

.c.o:
	${CC} ${CFLAGS} -c $*.c

diff3.o:	diff3.c
