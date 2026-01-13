#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/28
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	quot.c
OBJS=	quot.o
MAN=	quot.0
MANSRC=	quot.8

all: quot quot.0

quot: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

quot.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} quot tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: quot
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g operator -m 2755 quot /usr/sbin/quot

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
