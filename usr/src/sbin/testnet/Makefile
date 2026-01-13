#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/25
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	testnet.c
OBJS=	testnet.o

all: testnet

testnet: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

clean:
	rm -f ${OBJS} testnet tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: testnet
	install -c -s -o root -g staff -m 755 testnet /sbin/testnet

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
