#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/27
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	renice.c
OBJS=	renice.o
MAN=	renice.0
MANSRC=	renice.8

all: renice renice.0

renice: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS}

renice.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} renice tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: renice
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 renice /usr/bin/renice

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
