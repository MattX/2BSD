# The rules.c file can be modified locally for people who still like
#	things like fortran.

LFLAG=	-i

OBJS=	main.o doname.o misc.o files.o rules.o dosys.o gram.o dyndep.o \
	prtmem.o

CFLAGS=	-O

all: make

ovall: ovmake

make: ${OBJS}
	${CC} -o make ${LFLAG} ${OBJS}

ovmake: ${OBJS}
	ld ${LFLAG} -o make ${CRT} -Z doname.o dosys.o gram.o \
	-Z misc.o files.o rules.o -Z dyndep.o prtmem.o -L main.o -lovc

gram.o:	gram.c

${OBJS}: defs

install:
	install -s -o bin -g bin -m 711 make ${DESTDIR}/bin

clean:
	rm -f *.o a.out core errs make gram.c

.c.o:
	${CC} ${CFLAGS} -Dstrchr=index -Dstrrchr=rindex -DBSD4_2 -DBSD -c $*.c
