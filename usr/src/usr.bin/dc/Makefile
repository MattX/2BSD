#
# dc makefile
#

CFLAGS=	-O
ALL=	dc
OBJS=	dc.o

all: ${ALL}

dc: ${OBJS}
	${CC} ${LFLAG} -O ${OBJS} -o $@

install: ${ALL}
	install -s -o bin -g bin -m 711 dc ${DESTDIR}/usr/bin

clean:
	rm -f ${OBJS} ${ALL}

.c.o:
	${CC} ${CFLAGS} -c $*.c

dc.o:	dc.h dc.c
