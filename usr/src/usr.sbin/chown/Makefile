#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/28
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	chown.c
OBJS=	chown.o
MAN=	chown.0
MANSRC=	chown.8

all: chown chown.0

chown: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

chown.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} chown tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: chown
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 chown /usr/sbin/chown

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
