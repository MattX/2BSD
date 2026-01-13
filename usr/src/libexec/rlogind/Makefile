#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/25
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	rlogind.c
OBJS=	rlogind.o
MAN=	rlogind.0
MANSRC=	rlogind.8

all: rlogind rlogind.0

rlogind: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS} -lutil

rlogind.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} rlogind tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: rlogind
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 rlogind /usr/libexec/rlogind

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
