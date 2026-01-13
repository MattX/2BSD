#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/12/18
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	trsp.c
OBJS=	trsp.o
MAN=	trsp.0
MANSRC=	trsp.8

all: trsp trsp.0

trsp: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

trsp.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} trsp tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: all
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 trsp /usr/sbin/trsp

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
