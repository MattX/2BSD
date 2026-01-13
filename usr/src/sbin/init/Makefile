#
# Public Domain.  1997/1/10 - Steven Schultz
#
#	@(#)Makefile	1.2 (2.11BSD) 2025/11/25
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	init.c
OBJS=	init.o
MAN=	init.0
MANSRC=	init.8

all: init init.0

init: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

init.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} init tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: init
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	-mv -f /etc/init /etc/init.old
	install -c -s -o root -g staff -m 700 init /etc/init

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
