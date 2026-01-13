#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/28
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	quotaon.c
OBJS=	quotaon.o
MAN=	quotaon.0
MANSRC=	quotaon.8

all: quotaon quotaon.0

quotaon: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS}

quotaon.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} quotaon tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: quotaon
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	-rm -f /usr/man/cat8/quotaoff.0
	ln /usr/man/cat8/quotaon.0 /usr/man/cat8/quotaoff.0
	install -c -s -o root -g staff -m 755 quotaon /usr/sbin/quotaon
	-rm -f /usr/sbin/quotaoff
	ln /usr/sbin/quotaon /usr/sbin/quotaoff

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
