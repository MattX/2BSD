#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.2 (2.11BSD) 2025/11/27
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	arp.c
OBJS=	arp.o
MAN=	arp.0
MANSRC=	arp.8

all: arp arp.0

arp: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

arp.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} arp tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: arp arp.0
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g kmem -m 2755 arp /usr/sbin/arp

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
