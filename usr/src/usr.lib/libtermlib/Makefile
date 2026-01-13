#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.2 (2.11BSD) 2025/11/27
#
CFLAGS=	-O -DCM_N -DCM_GT -DCM_B -DCM_D -DNOSCCS
LD = ld
SRCS=	termcap.c tgoto.c tputs.c
OBJS=	termcap.o tgoto.o tputs.o
TAGSFILE=tags

.c.o:
	$(CC) $(CFLAGS) -c -p $*.c
	$(LD) -x -r -o profiled/$*.o $*.o
	$(CC) $(CFLAGS) -c $*.c
	$(LD) -X -r $*.o
	mv a.out $*.o

termcap.a termcap_p.a: ${OBJS}
	ar cr termcap.a ${OBJS}
	cd profiled; ar cr ../termcap_p.a ${OBJS}

install: termcap.a termcap_p.a
	install -c -m 644 -o root termcap.a /usr/lib/libtermcap.a
	-rm -f /usr/lib/libtermlib.a
	ln /usr/lib/libtermcap.a /usr/lib/libtermlib.a
	ranlib /usr/lib/libtermcap.a
	install -c -m 644 -o root termcap_p.a /usr/lib/libtermcap_p.a
	-rm -f /usr/lib/libtermlib_p.a
	ln /usr/lib/libtermcap_p.a /usr/lib/libtermlib_p.a
	ranlib /usr/lib/libtermcap_p.a

tags:
	cwd=`pwd`; \
	for i in ${SRCS}; do \
		ctags -a -f ${TAGSFILE} $$cwd/$$i; \
	done

clean:
	-rm -f *.o profiled/*.o
	-rm -f termcap.a termcap_p.a
