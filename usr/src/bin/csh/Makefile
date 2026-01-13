#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley Software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.5 (2.11BSD) 2025/12/25
#
# C Shell with process control; VM/UNIX VAX Makefile
# Bill Joy UC Berkeley; Jim Kulp IIASA, Austria
#
# To profile, put -DPROF in DEFS and -pg in CFLAGS, and recompile.

DEFS=	-DTELL -DVFORK -DFILEC
CFLAGS=	$(DEFS) -O
LDFLAGS=-i
XSTR=	/usr/ucb/xstr
CTAGS=	/usr/ucb/ctags
LIBES=  -lc

BASE=	alloc11.o doprnt11.o printf.o sh.char.o sh.dir.o sh.dol.o \
	sh.err.o sh.exp.o sh.file.o sh.func.o sh.glob.o sh.hist.o \
	sh.init.o sh.lex.o sh.parse.o sh.print.o sh.proc.o sh.sem.o \
	sh.set.o
OV1= 	sh.o sh.exec.o sh.time.o
OV2=	sh.misc.o sh.exec2.o

OBJS=	${BASE} ${OV1} ${OV2}

LIBOBJS= ndbm.o getpwent.o

# Special massaging of C files for sharing of strings
.c.o:
	${CC} -E ${CFLAGS} $*.c | ${XSTR} -c -
	${CC} -c ${CFLAGS} x.c 
	mv -f x.o $*.o
	rm -f x.c

# strings.o must be last since it can change when previous files compile
csh: ${OBJS} strings.o
	ar x /lib/libc.a ${LIBOBJS}
	rm -f csh
	/bin/ld ${LDFLAGS} /lib/crt0.o ${BASE} \
		-Z ${OV1} \
		-Z ${OV2} ${LIBOBJS} \
		-Y strings.o -o csh ${LIBES};

# need an old doprnt, whose output we can trap
doprnt11.o: doprnt11.s
	/lib/cpp -t doprnt11.s | sed '/^#/d' | as -o doprnt11.o

strings.o: strings
	${XSTR}
	${CC} -c xs.c
	mv -f xs.o strings.o
	rm -f xs.c

lint:
	lint -z ${DEFS} sh*.c alloc.c

install: csh
	install -m 755 -o bin -g bin -s csh ${DESTDIR}/bin/csh

clean:
	rm -f a.out strings x.c xs.c csh errs *.o

tags:	*.h sh*.c
	${CTAGS} -t *.h sh*.c
