#
#	@(#)Makefile	1.0 (2.11BSD) 1995/12/22
#

CFLAGS=	-O
SEPFLAG= -i
LIBC=	/lib/libc.a
SRCS=	ping.c
OBJS=	ping.o
MAN=	ping.0
MANSRC=	ping.8

all: ping ${MAN}

ping: ${LIBC}
	${CC} ${SEPFLAG} -o $@ ${CFLAGS} $@.c

ping.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} ping

cleandir: clean
	rm -f tags .depend

depend: ${SRCS}
	mkdep -p ${CFLAGS} ${SRCS}

install: ${MAN} ping
	install -s -o root -g bin -m 4751 ping ${DESTDIR}/etc
	install -c -o bin -g bin -m 444 ${MAN} ${DESTDIR}/usr/man/cat8

lint: ${SRCS}
	lint -haxc ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
