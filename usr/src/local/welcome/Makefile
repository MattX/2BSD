#       Makefile        2.0 (2.11BSD) 2025/11/25

BINDIR=	/usr/local
CFLAGS= -O
LDFLAGS= -i
PROGRAM=	welcome
SRCS=		welcome.c
OBJS=		welcome.o
MAN=		welcome.0
LIBS=

all:	${PROGRAM} ${MAN}

${PROGRAM}:	${OBJS}
	${CC} ${LDFLAGS} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

install: ${PROGRAM} welcome.0
	install -c -s -m 755 ${PROGRAM} ${BINDIR}
	install -c -m 444 -o root -g staff welcome.0 /usr/local/man/cat1

welcome.0: welcome.1
	/usr/man/manroff welcome.1 > welcome.0

tags:
	ctags -tdw *.c

clean:
	rm -f a.out core *.o welcome.0 ${PROGRAM}

depend:
	mkdep welcome.c
