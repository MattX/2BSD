#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.0 (2.11BSD) 1996/11/16
#
CFLAGS=	 -O
SEPFLAG= -i
SRCS=	cron.c
OBJS=	cron.o
MAN=	cron.0
MANSRC=	cron.8

all: cron cron.0

cron: ${OBJS}
	${CC} ${CFLAGS} ${SEPFLAG} -o $@ ${OBJS}

cron.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} cron tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: cron
	install -c -o bin -g bin -m 444 ${MAN} ${DESTDIR}/usr/man/cat8
	install -s -o root -g bin -m 755 cron ${DESTDIR}/usr/sbin/cron

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
