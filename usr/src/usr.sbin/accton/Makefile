#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.0 (2.11BSD) 1996/11/16
#
CFLAGS=	 -O
SEPFLAG= -i
SRCS=	accton.c
OBJS=	accton.o

all: accton

accton: ${OBJS}
	${CC} ${CFLAGS} ${SEPFLAG} -o $@ ${OBJS}

clean:
	rm -f ${OBJS} accton tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: accton
	install -s -o root -g bin -m 755 accton ${DESTDIR}/usr/sbin/accton

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
