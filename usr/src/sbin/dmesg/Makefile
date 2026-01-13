#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.2 (2.11BSD) 2025/11/25
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	dmesg.c
OBJS=	dmesg.o
MAN=	dmesg.0
MANSRC=	dmesg.8

all: dmesg dmesg.0

dmesg: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

dmesg.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} dmesg tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: dmesg dmesg.0
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g kmem -m 2755 dmesg /sbin/dmesg

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
