#
# Public Domain.  1996/10/24 - sms
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/12/26
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	makekey.c
OBJS=	makekey.o

all: makekey

makekey: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

clean:
	rm -f ${OBJS} makekey tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: makekey
	install -c -s -o root -g staff -m 755 makekey /usr/libexec/makekey

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
