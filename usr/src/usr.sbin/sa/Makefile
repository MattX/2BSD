#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.0 (2.11BSD) 1996/11/16
#
CFLAGS=	 -O
SEPFLAG= -i
SRCS=	sa.c
OBJS=	sa.o
MAN=	sa.0
MANSRC=	sa.8

all: sa sa.0

sa: ${OBJS}
	${CC} ${CFLAGS} ${SEPFLAG} -o $@ ${OBJS}

sa.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} sa tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: sa
	install -c -o bin -g bin -m 444 ${MAN} ${DESTDIR}/usr/man/cat8
	-rm -f ${DESTDIR}/usr/man/cat8/accton.0
	ln ${DESTDIR}/usr/man/cat8/sa.0 ${DESTDIR}/usr/man/cat8/accton.0
	install -s -o root -g bin -m 755 sa ${DESTDIR}/usr/sbin/sa

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
