#
# Copyright (c) 1987 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.6.4 (2.11BSD) 2025/11/29
#
SRCS=	_setjmp.s abs.s alloca.s atof.c bcmp.s bcopy.s bzero.s \
	ecvt.c fabs.s ffs.s frexp.s index.s insque.s ldexp.s ldfps.s \
	modf.s nsym.c remque.s rindex.s rollback.s setjmp.s setjmperr.c \
	strcat.s strcmp.s strcpy.s strlen.s strncat.s strncmp.s strncpy.s
OBJS=	_setjmp.o abs.o alloca.o atof.o bcmp.o bcopy.o bzero.o \
	ecvt.o fabs.o ffs.o frexp.o index.o insque.o ldexp.o ldfps.o \
	modf.o nsym.o remque.o rindex.o rollback.o setjmp.o setjmperr.o \
	strcat.o strcmp.o strcpy.o strlen.o strncat.o strncmp.o strncpy.o

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

genlib genlib_p: ${OBJS}
	@echo "building profiled genlib"
	@cd profiled; ar cru ../genlib_p ${OBJS}
	@echo "building normal genlib"
	@ar cru genlib ${OBJS}

tags:
	cwd=`pwd`; \
	for i in atof.c ecvt.c; do \
		ctags -a -f ${TAGSFILE} $$cwd/$$i; \
	done

clean:
	rm -f *.o profiled/*.o errs a.out core genlib genlib_p \
		tags Makefile.bak

depend:	$(SRCS)
	mkdep ${CFLAGS} ${SRCS}

# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

_setjmp.o: _setjmp.s DEFS.h
abs.o: abs.s DEFS.h
alloca.o: alloca.s DEFS.h
atof.o: atof.c /usr/include/math.h /usr/include/ctype.h
bcmp.o: bcmp.s DEFS.h
bcopy.o: bcopy.s DEFS.h
bzero.o: bzero.s DEFS.h
ecvt.o: ecvt.c
fabs.o: fabs.s DEFS.h
ffs.o: ffs.s DEFS.h
frexp.o: frexp.s DEFS.h
index.o: index.s DEFS.h
insque.o: insque.s DEFS.h
ldexp.o: ldexp.s DEFS.h
ldfps.o: ldfps.s DEFS.h
modf.o: modf.s DEFS.h
nsym.o: nsym.c /usr/include/a.out.h /usr/include/sys/exec.h
nsym.o: /usr/include/sys/types.h /usr/include/sys/select.h
nsym.o: /usr/include/sys/types.h /usr/include/nlist.h /usr/include/sys/types.h
remque.o: remque.s DEFS.h
rindex.o: rindex.s DEFS.h
rollback.o: rollback.s
setjmp.o: setjmp.s DEFS.h
setjmperr.o: setjmperr.c /usr/include/setjmp.h
strcat.o: strcat.s DEFS.h
strcmp.o: strcmp.s DEFS.h
strcpy.o: strcpy.s DEFS.h
strlen.o: strlen.s DEFS.h
strncat.o: strncat.s DEFS.h
strncmp.o: strncmp.s DEFS.h
strncpy.o: strncpy.s DEFS.h

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
