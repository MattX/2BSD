#
# yacc makefile
#

CFLAGS=-O
OBJS=	y1.o y2.o y3.o y4.o
ALL=	yacc
OVALL=	ovyacc

all: ${ALL}

ovall: ${OVALL}

yacc: ${OBJS}
	${CC} ${LFLAG} ${OBJS} -o ${ALL}

ovyacc: ${OBJS}
	ld ${LFLAG} ${CRT} -o ${ALL} -Z y1.o y3.o -Z y2.o -L y4.o -lovc

install:
	install -s -o bin -g bin -m 711 yacc ${DESTDIR}/usr/bin

.c.o:
	${CC} ${CFLAGS} -c $*.c

y1.o:	y1.c dextern files
y2.o:	y2.c dextern files
y3.o:	y3.c dextern files
y4.o:	y4.c dextern files

clean:
	rm -f ${OBJS} ${ALL}
