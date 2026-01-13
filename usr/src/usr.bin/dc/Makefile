#
#	@(#)Makefile	1.5	(2.11BSD) 2025/11/25
#
SRCS=	dc.c dc.h
CFLAGS=	-O
LDFLAGS= -i

all: dc

dc:	dc.c dc.h
	${CC} ${CFLAGS} ${LDFLAGS} dc.c -o dc

install: dc
	install -c -s -m 755 -o root dc /usr/bin/dc

clean:
	rm -f *.o dc a.out errs core
