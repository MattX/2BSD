#
# prep makefile
#

CFLAGS=	-O
OBJ=	prep0.o prep1.o prep2.o
ALL=	prep

all: ${ALL}

install: ${ALL}
	install -s -o bin -g bin -m 711 prep ${DESTDIR}/usr/bin

prep:	${OBJ}
	${CC} ${LFLAG} ${OBJ} -o $@

clean:
	rm -f ${OBJ} ${ALL}

.c.o:
	${CC} ${CFLAGS} -c $*.c

prep0.o:	prep0.c prep.h
prep1.o:	prep1.c prep.h
prep2.o:	prep2.c
