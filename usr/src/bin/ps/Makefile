#
# makefile for ps
#

CFLAGS=	-O ${FP} -DNUMDEV=256 -DTERMCAP
OBJS=	ps.o bmove.11.o
ALL=	ps

all:	${ALL}

install:
	install -s -o root -g su -m 4711 ps ${DESTDIR}/bin

ps: ${OBJS}
	${CC} ${LFLAG} -o $@ ${OBJS} -ltermcap

clean:
	rm -f *.o ${ALL}
