#
# makefile for awk
#
# FP should be -f for machines without floating point

CFLAGS=-O
YFLAGS=-d
FP=
FILES=	awk.lx.o b.o main.o token.o tran.o lib.o run.o parse.o proctab.o
SOURCE=	awk.def awk.g.y awk.lx.l b.c lib.c main.c parse.c proctab.c \
	proc.c run.c token.c tran.c
ALL=	awk

all:	${ALL}

install:
	install -s -o bin -g bin -m 711 awk ${DESTDIR}/bin

awk:	${FILES} awk.g.o
	${CC} ${LFLAG} awk.g.o ${FILES} -lm -o awk

y.tab.h: awk.g.o

awk.h:	y.tab.h
	-cmp -s y.tab.h awk.h || cp y.tab.h awk.h

.c.o:
	${CC} ${CFLAGS} ${FP} -c $*.c

${FILES}: awk.h awk.def

token.c: awk.h
	ed - <tokenscript
	rm temp

proctab.c: proc
	proc > proctab.c

proc: awk.h proc.o token.o
	${CC} -o proc proc.c token.o

clean:
	rm -f *.o ${ALL}

proctab.o:	proctab.c
proc.o:		proc.c
token.o:	token.c
b.o:		b.c
main.o:		main.c
tran.o:		tran.c
lib.o:		lib.c
run.o:		run.c
parse.o:	parse.c
