#
# Copyright (c) 1987 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.7.2 (2.11BSD) 2025/11/29
#
#
SRCS=	aldiv.s almul.s alrem.s csv.s ldiv.s lmul.s lrem.s udiv.s \
	uldiv.s ulrem.s ulsh.s ultof.s
OBJS=	aldiv.o almul.o alrem.o csv.o ldiv.o lmul.o lrem.o udiv.o \
	uldiv.o ulrem.o ulsh.o ultof.o

CFLAGS=	-O ${DEFS}
TAGSFILE=tags

.s.o:
	/lib/cpp -E -DPROF ${DEFS} $< | sed -e 's;^#;/;' | as -V -o $@
	-ld -X -r $*.o
	mv a.out profiled/$*.o
	/lib/cpp -E ${DEFS} $< | sed -e 's;^#;/;' | as -V -o $@
	-ld -x -r $*.o
	mv a.out $*.o

.c.o:
	cc -p -c ${CFLAGS} $*.c
	-ld -X -r $*.o
	mv a.out profiled/$*.o
	cc -c ${CFLAGS} $*.c
	-ld -x -r $*.o
	mv a.out $*.o

crtlib crtlib_p: ${OBJS}
	@echo "building profiled crtlib"
	@cd profiled; ar cru ../crtlib_p ${OBJS}
	@echo "building normal crtlib"
	@ar cru crtlib ${OBJS}

tags:

clean:
	rm -f *.o profiled/*.o errs a.out core crtlib crtlib_p \
		tags Makefile.bak

depend:	${SRCS}
	mkdep ${CFLAGS} ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

aldiv.o: aldiv.s DEFS.h
almul.o: almul.s DEFS.h
alrem.o: alrem.s DEFS.h
csv.o: csv.s DEFS.h
ldiv.o: ldiv.s DEFS.h
lmul.o: lmul.s DEFS.h
lrem.o: lrem.s DEFS.h
udiv.o: udiv.s DEFS.h
uldiv.o: uldiv.s DEFS.h
ulrem.o: ulrem.s DEFS.h
ulsh.o: ulsh.s DEFS.h
ultof.o: ultof.s DEFS.h

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
