#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/25
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	route.c
OBJS=	route.o
MAN=	route.0
MANSRC=	route.8

all: route route.0

route: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

route.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} route tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: route
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 route /sbin/route

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
