#	@(#)Makefile	5.9 (2.11BSD) 2025/11/25

CFLAGS=	-I. -O
LDFLAGS= -i

SRCS=	append.c ar.c archive.c contents.c delete.c extract.c misc.c \
	move.c print.c replace.c 

EXTRASRCS= strmode.c

OBJS=	append.o ar.o archive.o contents.o delete.o extract.o misc.o \
	move.o print.o replace.o 

EXTRAOBJ= strmode.o

all: ar ar.1.0 ar.5.0

ar: ${OBJS} ${EXTRAOBJ}
	cc ${LDFLAGS} -o ar ${OBJS} ${EXTRAOBJ}

ar.1.0: ar.1
	nroff -man ar.1 > ar.1.0

ar.5.0: ar.5.5
	nroff -man ar.5.5 > ar.5.0

install:
	install -c -s -m 755 -o root ar /bin/ar
	install -c -o root -g staff -m 444 ar.1.0 /usr/man/cat1/ar.0
	install -c -o root -g staff -m 444 ar.5.0 /usr/man/cat5/ar.0

lint:
	lint -havx -I. ${SRCS} ${EXTRASRCS}

clean:
	rm -f core ar *.o ar.1.0 ar.5.0
