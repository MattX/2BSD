CFLAGS=	-O

all: msgs

msgs: msgs.c msgs.h
	${CC} ${LFLAG} ${CFLAGS} -o msgs msgs.c -ltermcap

install:
	install -s -o bin -g bin -m 711 msgs ${DESTDIR}/usr/ucb

clean:
	rm -f msg.o ${ALL}
