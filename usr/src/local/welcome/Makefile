#	
#	Makefile for welcome
#
DESTDIR=
BINDIR=	/usr/local
CFLAGS= -O
SEPFLAG= -i
PROGRAM=	welcome
SRCS=		welcome.c
LIBS=

${PROGRAM}:	${SRCS}
	${CC} ${SEPFLAG} ${CFLAGS} -o $@ $@.c ${LIBS}

install: ${PROGRAM} welcome.0
	install ${PROGRAM} ${DESTDIR}/${BINDIR}
	install -c -m 444 -o bin -g bin welcome.0 ${DESTDIR}/usr/local/man/cat1

welcome.0: welcome.1
	/usr/man/manroff welcome.1 > welcome.0

tags:
	ctags -tdw *.c

clean:
	rm -f a.out core *.o welcome.0 ${PROGRAM}

depend:
	echo 'Welcome Compiled.'
