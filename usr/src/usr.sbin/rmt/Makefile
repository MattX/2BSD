#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/28
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	rmt.c
OBJS=	rmt.o
MAN=	rmt.0
MANSRC=	rmt.8

all: rmt rmt.0

rmt: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS}

rmt.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} rmt tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: rmt
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 rmt /usr/sbin/rmt

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
