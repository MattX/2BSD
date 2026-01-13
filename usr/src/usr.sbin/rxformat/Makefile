#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/28
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	rxformat.c
OBJS=	rxformat.o
MAN=	rxformat.0
MANSRC=	rxformat.8

all: rxformat rxformat.0

rxformat: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS}

rxformat.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} rxformat tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: rxformat
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 rxformat /usr/sbin/rxformat

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
