CFLAGS=	-O
SEPFLAG= -i
SRCS=	mkovmake.c
OBJS=	mkovmake.o
MANSRC=	mkovmake.1
MAN=	mkovmake.0

all: mkovmake

mkovmake: ${OBJS}
	${CC} ${SEPFLAG} -o $@ ${CFLAGS} ${OBJS}

clean:
	rm -f ${OBJS} core mkovmake ${MAN}

install: all ${MAN}
	install -s -o bin -g bin -m 755 mkovmake ${DESTDIR}/usr/local
	install -m 444 -o bin -g bin ${MAN} ${DESTDIR}/usr/local/man/cat1

${MAN}: ${MANSRC}
	rm -f ${MAN}
	/usr/man/manroff ${MANSRC} > ${MAN}

lint: ${SRCS}
	lint ${CFLAGS} ${SRCS}
