#
# xsend makefile
#

CFLAGS =-O
ALL=	enroll xsend xget

all: ${ALL}

install: ${ALL}
	install -s -o bin -g bin -m 711 enroll ${DESTDIR}/usr/bin
	install -s -o bin -g bin -m 711 xget ${DESTDIR}/usr/bin
	install -s -o bin -g bin -m 711 xsend ${DESTDIR}/usr/bin

enroll:	enroll.o lib.o
	${CC} ${LFLAG} enroll.o lib.o -lmp -o $@

xget:	xget.o lib.o
	${CC} ${LFLAG} xget.o lib.o -lmp -o $@

xsend:	xsend.o lib.o
	${CC} ${LFLAG} xsend.o lib.o -lmp -o $@

clean:
	rm -f *.o ${ALL}

.c.o:
	${CC} ${CFLAGS} -c $*.c

enroll.o:	enroll.c xmail.h
lib.o:		lib.c xmail.h
xget.o:		xget.c xmail.h
xsend.o:	xsend.c xmail.h
