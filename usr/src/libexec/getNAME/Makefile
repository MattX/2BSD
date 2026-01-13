#
# Public Domain.  1996/10/24 - sms
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/25
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	getNAME.c
OBJS=	getNAME.o

all: getNAME

getNAME: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

clean:
	rm -f ${OBJS} getNAME tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: getNAME
	install -c -s -o root -g staff -m 755 getNAME /usr/libexec/getNAME

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
