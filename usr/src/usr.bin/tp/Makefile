#
# tp makefile
#

CFLAGS=	-O
OBJ=	tp0.o tp1.o tp2.o tp3.o
ALL=	tp

all: ${ALL}

tp: ${OBJ}
	${CC} ${LFLAG} ${CFLAGS} ${OBJ} -o $@

install: ${ALL}
	install -s -o bin -g bin -m 711 tp ${DESTDIR}/usr/bin/tp

.c.o:
	${CC} ${CFLAGS} -c $*.c

clean:
	rm -f ${OBJ} ${ALL}

tp0.o:	tp0.c tp.h
tp1.o:	tp1.c tp.h
tp2.o:	tp2.c tp.h
tp3.o:	tp3.c tp.h
