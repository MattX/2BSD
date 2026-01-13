#
# Public Domain.  1999/2/19 - Steven Schultz
#
#	@(#)Makefile	1.3 (2.11BSD) 2025/11/25
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	acctd.c
OBJS=	acctd.o
MAN=	acctd.0
MANSRC=	acctd.8

all: acctd acctd.0

acctd: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

acctd.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} acctd tags ${MAN}

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: acctd
	install -c -s -o root -g staff -m 700 acctd /usr/libexec/acctd
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8/${MAN}

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
