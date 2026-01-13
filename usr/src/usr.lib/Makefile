#
# Copyright (c) 1983 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.12.5 (2.11BSD) 2025/11/29
#
CFLAGS=	-O
TAGSFILE=tags

# Subdirectories whose routines are included in the making of the
# master tags file (/usr/lib/tags); the Fortran libraries should
# be on this list.
#
TAGSDIR=libcurses libdbm libln libmp libtermlib libutil liberrlst

# Programs that live in subdirectories and have makefiles of their own.

SUBDIR=	lib2648 libF77 libI77 libU77 libcurses libdbm libln \
	libom libmp libplot libtermlib liby libutil libvmf \
	liberrlst libident libstubs

all:	${SUBDIR}

${SUBDIR}: FRC
	cd $@; make ${MFLAGS}

FRC:

install: FRC
	-for i in ${SUBDIR}; do \
		(cd $$i; make ${MFLAGS} install); done
	rm -f /usr/lib/libm.a /usr/lib/libm_p.a
	ln /usr/lib/libom.a /usr/lib/libm.a
	ln /usr/lib/libom_p.a /usr/lib/libm_p.a

tags:
	-for i in ${TAGSDIR}; do \
		(cd $$i; make ${MFLAGS} TAGSFILE=../${TAGSFILE} tags); \
	done

clean:
	rm -f a.out core *.s *.o tags
	-for i in ${SUBDIR}; do (cd $$i; make ${MFLAGS} clean); done

depend:
