#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.2 (2.11BSD) 2025/11/28
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	rwhod.c
OBJS=	rwhod.o
MAN=	rwhod.0
MANSRC=	rwhod.8

all: rwhod rwhod.0

rwhod: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS}

rwhod.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} rwhod tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: rwhod rwhod.0
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 rwhod /usr/sbin/rwhod

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
