#
# m4 makefile
#

CFLAGS=	-O
LFLAG=	-i
ALL=	m4
OBJS=	m4.o m4y.o

all:	${ALL}

m4:	${OBJS}
	${CC} ${LFLAG} -o $@ ${OBJS}

install: m4
	install -s -o bin -g bin -m 711 m4 ${DESTDIR}/usr/bin

clean:
	rm -f ${ALL} ${OBJS}

.c.o:
	${CC} ${CFLAGS} -c $*.c

m4.o:	m4.c
m4y.o:	m4y.y
	yacc m4y.y
	mv y.tab.c m4y.c
	${CC} ${CFLAGS} -c m4y.c
