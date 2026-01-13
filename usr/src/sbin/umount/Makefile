#
# Public Domain.  1996/1/15 - Steven Schultz
#
#	@(#)Makefile	1.2 (2.11BSD) 2025/11/25
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	umount.c
OBJS=	umount.o
MAN=	umount.0
MANSRC=	umount.8

all: umount umount.0

umount: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

umount.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} umount tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: all
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 umount /sbin/umount

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
