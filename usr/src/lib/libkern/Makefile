#
#	@(#)Makefile	1.0 (2.11BSD GTE) 1995/1/10
#
# This is the Makefile for 'libkern.a'.  These are the specially compiled
# versions of libc.a routines which the kernel and network use.
#
# There are no source files in this directory.  The VPATH capability is
# used instead.  libkern.a needs to be installed only once (usually when the 
# C library is installed) rather than compiling the sources each time 
# a kernel is compiled..

DEFS=	-DKERNEL
CFLAGS=	 -O ${DEFS}
CPP=	/lib/cpp -P ${DEFS}
AS=	/bin/as -u
PREFIX=	/usr/src/lib/libc
VPATH= ${PREFIX}/pdp/crt:${PREFIX}/pdp/gen:${PREFIX}/pdp/net

SRCS=	bcmp.s bcopy.s bzero.s ffs.s htonl.s htons.s insque.s \
	ldiv.s lmul.s lrem.s remque.s strlen.s udiv.s uldiv.s ulsh.s

OBJS=	bcmp.o bcopy.o bzero.o ffs.o htonl.o htons.o insque.o \
	ldiv.o lmul.o lrem.o remque.o strlen.o udiv.o uldiv.o ulsh.o

.c.o:
	${CC} ${CFLAGS} $*.c
	ld -X -r $*.o
	mv a.out $*.o

.s.o:
	${CPP} $*.s | ${AS} -o $*.o
	ld -X -r $*.o
	mv a.out $*.o

all: libkern.a

libkern.a: ${OBJS}
	rm -f libkern.a
	ar rv libkern.a ${OBJS}

clean:
	rm -f ${OBJS} libkern.a

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: libkern.a
	install -o root -g bin -m 644 libkern.a ${DESTDIR}/usr/lib/libkern.a
	ranlib ${DESTDIR}/usr/lib/libkern.a

# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
