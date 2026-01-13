#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/25
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	icheck.c
OBJS=	icheck.o
MAN=	icheck.0
MANSRC=	icheck.8

all: icheck icheck.0

icheck: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

icheck.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} icheck tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: icheck
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 icheck /sbin/icheck

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
