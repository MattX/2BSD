#
# makefile for sed
#

CFLAGS=	-O
OBJ=	sed0.o sed1.o
ALL=	sed

all: ${ALL}

sed: ${OBJ}
	${CC} ${LFLAG} -o sed ${OBJ}

clean:
	rm -f sed ${OBJ} ${ALL}

install:
	install -o bin -g bin -m 711 sed ${DESTDIR}/bin

sed0.o:	sed0.c sed.h
sed1.o:	sed1.c sed.h
