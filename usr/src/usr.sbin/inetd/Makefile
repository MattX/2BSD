#
# Public Domain.  1996/11/6 - Steven Schultz
#
#	@(#)Makefile	1.2 (2.11BSD) 2025/11/28
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	inetd.c
OBJS=	inetd.o
MAN=	inetd.0
MANSRC=	inetd.8

all: inetd inetd.0

inetd: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

inetd.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} inetd tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: inetd
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 inetd /usr/sbin/inetd

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
