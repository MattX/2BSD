#
# lex makefile
#
# SMALL should be "-DSMALL" for a smaller lex

SMALL=
CFLAGS=	-O
OBJS=	header.o lmain.o sub1.o sub2.o y.tab.o
ALL=	lex
OALL=	ovlex

all: ${ALL}

install:
	install -s -o bin -g bin -m 711 lex ${DESTDIR}/usr/bin

lex: ${OBJS}
	${CC} ${LFLAG} ${OBJS} -o $@

ovall: ${OALL}

ovlex: ${OBJS}
	ld -o lex -X ${LFLAG} ${CRT} -Z lmain.o y.tab.o -Z sub2.o \
	-Z header.o -L sub1.o -lovc

y.tab.c: parser.y
	yacc parser.y

clean:
	rm -f ${OBJS} ${ALL} ${OVALL} y.tab.c

.c.o:
	${CC} ${FP} ${OV} ${SMALL} ${CFLAGS} -c $*.c

lmain.o:	lmain.c
lmain.o:	ldefs.c
lmain.o:	once.c
sub1.o:		sub1.c
sub1.o:		ldefs.c
sub2.o:		sub2.c
sub2.o:		ldefs.c
header.o:	header.c
header.o:	ldefs.c
