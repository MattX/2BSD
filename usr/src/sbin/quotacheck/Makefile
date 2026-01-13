#
# Public Domain.  1996/1/23 - Steven Schultz
#
#	@(#)Makefile	1.2 (2.11BSD) 2025/11/25
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	quotacheck.c preen.c
OBJS=	quotacheck.o preen.o
MAN=	quotacheck.0
MANSRC=	quotacheck.8

all: quotacheck quotacheck.0

quotacheck: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

quotacheck.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} quotacheck tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: all
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 quotacheck /sbin/quotacheck

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
