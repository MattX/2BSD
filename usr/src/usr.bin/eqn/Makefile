#
# eqn makefile
#
# FP set to -f for machines without floating point

CFLAGS=-O
YFLAGS=-d
ALL=	eqn
OBJS=	e.o diacrit.o eqnbox.o font.o fromto.o funny.o glob.o \
	integral.o io.o lex.o lookup.o mark.o matrix.o move.o \
	over.o paren.o pile.o shift.o size.o sqrt.o text.o

all: ${ALL}

eqn:	${OBJS}
	${CC} ${LFLAG} ${FP} ${OBJS} -o $@

install: ${ALL}
	install -s -o bin -g bin -m 711 eqn ${DESTDIR}/bin/eqn

clean:
	rm -f ${OBJS} ${ALL}

e.o:	e.y
	yacc -d e.y
	mv y.tab.c e.c
	mv y.tab.h e.def
	${CC} ${FP} ${CFLAGS} -c e.c

.c.o:
	${CC} ${FP} ${CFLAGS} -c $*.c

diacrit.o:		e.h e.def diacrit.c
eqnbox.o:		e.h e.def eqnbox.c
font.o:			e.h e.def font.c
fromto.o:		e.h e.def fromto.c
funny.o:		e.h e.def funny.c
glob.o:			e.h e.def glob.c
integral.o:		e.h e.def integral.c
io.o:			e.h e.def io.c
lex.o:			e.h e.def lex.c
lookup.o:		e.h e.def lookup.c
mark.o:			e.h e.def mark.c
matrix.o:		e.h e.def matrix.c
move.o:			e.h e.def move.c
over.o:			e.h e.def over.c
paren.o:		e.h e.def paren.c
pile.o:			e.h e.def pile.c
shift.o:		e.h e.def shift.c
size.o:			e.h e.def size.c
sqrt.o:			e.h e.def sqrt.c
text.o:			e.h e.def text.c
