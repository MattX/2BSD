#
# makefile for C compiler
#
# FP should be -f for machines without floating point

FP=
CFLAGS=	-O ${INCLUDES} ${FP}
SRC=	c00.c c01.c c02.c c03.c c04.c c05.c c10.c c11.c c12.c c13.c \
	c20.c c21.c cvopt.c

all:	c0 c1 c2 fc1

install:
	install -o bin -g bin -m 711 c0 ${DESTDIR}/lib
	install -o bin -g bin -m 711 c1 ${DESTDIR}/lib
	install -o bin -g bin -m 711 c2 ${DESTDIR}/lib
	install -o bin -g bin -m 711 fc1 ${DESTDIR}/lib

fcp:	fc1
	cp fc1 /lib/fc1
	rm -f fc1 c1*.o

c0: c00.o c01.o c02.o c03.o c04.o c05.o
	${CC} ${LFLAG} c00.o c01.o c02.o c03.o c04.o c05.o -o c0

c1: c10.o c11.o c12.o c13.o table.o
	${CC} ${LFLAG} c10.o c11.o c12.o c13.o table.o -o c1

fc1: c10.o c11.o c12.o c13.o table.o
	${CC} ${LFLAG} -f c10.o c11.o c12.o c13.o table.o -o fc1

table.o: table.s cvopt
	./cvopt <table.s >table.i
	as -o table.o table.i
	rm -f table.i

c2: c20.o c21.o
	${CC} ${FMT} -O ${INCLUDES} c20.o c21.o -o c2

cvopt:	cvopt.c
	${CC} ${LFLAG} $(CFLAGS) -o cvopt cvopt.c

clean:
	rm -f *.o c0 c1 c2 fc1 cvopt core

c00.o:		c0.h c00.c
c01.o:		c0.h c01.c
c02.o:		c0.h c02.c
c03.o:		c0.h c03.c
c04.o:		c0.h c04.c
c05.o:		c0.h c05.c
c10.o:		c1.h c10.c
c11.o:		c1.h c11.c
c12.o:		c1.h c12.c
c13.o:		c1.h c13.c
c20.o:		c2.h c20.c
c21.o:		c2.h c21.c
cvopt.o:	cvopt.c
