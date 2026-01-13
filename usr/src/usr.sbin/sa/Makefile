#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.3 (2.11BSD) 2025/11/28
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	sa.c
OBJS=	sa.o
MAN=	sa.0
MANSRC=	sa.8

all: sa sa.0

sa: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS}

sa.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} sa tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: all
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 sa /usr/sbin/sa

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
