#	Makefile for fingerd
#
#	Version 1.0 - 1995/04/01
#
SEPFLAG= -i
CFLAGS = -O

SRCS = fingerd.c
OBJS = fingerd.o

all:	fingerd fingerd.0

fingerd: ${OBJS}
	${CC} ${SEPFLAG} -o fingerd ${OBJS}

fingerd.0: fingerd.8
	/usr/man/manroff fingerd.8 > fingerd.0

install: all
	install -s -m 755 -g bin -o bin fingerd ${DESTDIR}/etc
	install -m 444 -g bin -o bin fingerd.0 ${DESTDIR}/usr/man/cat8

clean:	
	rm -f *.o fingerd *.core fingerd.0

cleandir: clean
	rm -f tags

depend: ${SRCS}
	mkdep -p ${CFLAGS} ${SRCS}

lint: ${SRCS}
	lint ${CFLAGS} ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
