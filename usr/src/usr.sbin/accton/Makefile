#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.2 (2.11BSD) 2025/11/27
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	accton.c
OBJS=	accton.o
MAN=	accton.0
MANSRC=	accton.8

all: accton accton.0

accton: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

accton.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} accton tags ${MAN}

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: accton
	install -c -s -o root -g staff -m 755 accton /usr/sbin/accton
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8/${MAN}

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
