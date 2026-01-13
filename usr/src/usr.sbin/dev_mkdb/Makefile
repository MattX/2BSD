#
# Public Domain.  2/3/1995 - Steven Schultz
#
#	@(#)Makefile	1.2 (2.11BSD) 2025/11/28
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	dev_mkdb.c
OBJS=	dev_mkdb.o
MAN=	dev_mkdb.0
MANSRC=	dev_mkdb.8

all: dev_mkdb dev_mkdb.0

dev_mkdb: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

dev_mkdb.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} dev_mkdb tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: all
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 dev_mkdb /usr/sbin/dev_mkdb

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
