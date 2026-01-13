#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.5 (2.11BSD) 2025/11/25
#
CFLAGS=	-O

# Programs that live in subdirectories, and have makefiles of their own.
SUBDIR=	chess zork

# Assembly programs that live in the current directory and need explicit
# make lines.
#
NSTD=	bj factor moo primes ttt

all:	${SUBDIR} ${NSTD}

${SUBDIR}: FRC
	cd $@; make ${MFLAGS}

FRC:

install: ${NSTD}
	-for i in ${SUBDIR}; do \
		(cd $$i; make ${MFLAGS} install); done
	-for i in ${NSTD}; do \
		(install -c -s -m 755 $$i /usr/games/$$i); done

clean:
	rm -f a.out core *.o
	-for i in ${SUBDIR}; do (cd $$i; make ${MFLAGS} clean); done
	rm -f ${NSTD}

depend:

# Files listed in ${NSTD} have explicit make lines given below.

bj: bj.s etc.s us.s
	as -o bj.o etc.s bj.s us.s
	ld -o $@ bj.o -lc

factor: factor.s
	as -o factor.o factor.s
	ld -o $@ factor.o -lc

moo: moo.s etc.s
	as -o moo.o etc.s moo.s
	ld -o $@ moo.o -lc

primes: primes.s
	as -o primes.o primes.s
	ld -o $@ primes.o -lc

ttt: ttt.s etc.s
	as -o ttt.o etc.s ttt.s
	ld -o $@ ttt.o -lc
