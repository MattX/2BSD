#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.4.2 (2.11BSD) 2025/11/25
#
CFLAGS=	-O
TAGSFILE=tags

# Programs that live in subdirectories, and have makefiles of their own.
#
SUBDIR=	ccom c2 cpp libc libkern

all:	${SUBDIR}

${SUBDIR}: FRC
	cd $@; make ${MFLAGS}

FRC:
	
install:
	-for i in ${SUBDIR}; do \
		(cd $$i; make ${MFLAGS} \
			install); done

tags:
	cd libc; make ${MFLAGS} TAGSFILE=../${TAGSFILE} tags

clean:
	rm -f a.out core *.s *.o
	-for i in ${SUBDIR}; do (cd $$i; make ${MFLAGS} clean); done
