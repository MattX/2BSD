#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/28
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	mkproto.c
OBJS=	mkproto.o
MAN=	mkproto.0
MANSRC=	mkproto.8

all: mkproto mkproto.0

mkproto: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

mkproto.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} mkproto tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: mkproto
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 mkproto /usr/sbin/mkproto

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
