#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/26
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	rlogin.c
OBJS=	rlogin.o
MAN=	rlogin.0
MANSRC=	rlogin.1

all: rlogin rlogin.0

rlogin: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

rlogin.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} rlogin tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: rlogin
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat1
	install -c -s -o root -g staff -m 4755 rlogin /usr/ucb/rlogin

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
