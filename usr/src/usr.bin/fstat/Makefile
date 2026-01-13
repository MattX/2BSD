#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.2 (2.11BSD) 2025/11/27
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	fstat.c
OBJS=	fstat.o
MAN=	fstat.0
MANSRC=	fstat.8

all: fstat fstat.0

fstat: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS}

fstat.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} fstat tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: fstat fstat.0
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g kmem -m 2755 fstat /usr/bin/fstat

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
