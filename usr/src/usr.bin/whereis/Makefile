#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/27
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	whereis.c
OBJS=	whereis.o
MAN=	whereis.0
MANSRC=	whereis.1

all: whereis whereis.0

whereis: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS}

whereis.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} whereis tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: whereis
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat1
	install -c -s -o root -g staff -m 755 whereis /usr/bin/whereis

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
