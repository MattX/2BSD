#
#	@(#)Makefile	1.2 (2.11BSD) 2025/11/26
#

CFLAGS=	-O
LDFLAGS= -i
SRCS=	ping.c
OBJS=	ping.o
MAN=	ping.0
MANSRC=	ping.8

all: ping ${MAN}

ping: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${CFLAGS} ${OBJS}

ping.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} ping

cleandir: clean
	rm -f tags .depend

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: ${MAN} ping
	install -c -s -o root -g staff -m 4751 ping /bin
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8

lint: ${SRCS}
	lint -haxc ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
