#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/28
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	pstat.c
OBJS=	pstat.o
MAN=	pstat.0
MANSRC=	pstat.8

all: pstat pstat.0

pstat: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS}

pstat.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} pstat tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: pstat ${MAN}
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g kmem -m 2755 pstat /usr/sbin/pstat

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
