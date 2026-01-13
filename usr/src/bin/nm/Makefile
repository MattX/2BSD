#       @(#)Makefile    2.0 (2.11BSD) 2025/11/26

SRCS=	nm.c
OBJS=	nm.o
CFLAGS=	-O -I../ar

all:	nm

nm: ${OBJS}
	cc -i -o nm ${OBJS}

install:
	install -c -s -o root -g staff -m 755 nm /bin/nm

lint:
	lint -havx -I../ar ${SRCS}

clean:
	rm -f *.o *.0 nm
