#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/25
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	shutdown.c
OBJS=	shutdown.o
MAN=	shutdown.0
MANSRC=	shutdown.8

all: shutdown shutdown.0

shutdown: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

shutdown.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} shutdown tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: shutdown
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g operator -m 4750 shutdown /sbin/shutdown

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
