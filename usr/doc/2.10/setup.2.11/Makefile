#
# Copyright (c) 1986 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	6.2 (Berkeley) 9/8/88
#
PRINTER=-Plz001
PDEV=	-TLN03
TROFF=	ditroff
NROFF=	nroff
EQN=	dieqn
TBL=	ditbl 
LPR=	lpr -h -n ${PRINTER}

SRC=	0.t 1.t 2.t 3.t 4.t 5.t 6.t a.t

all: ${SRC}
	${EQN} ${SRC} | ${TBL} | ${TROFF} ${PAGES} -ms | ${LPR}

nroff: ${SRC}
	rm -f setup.NROFF
	${EQN} ${SRC} | ${TBL} | ${NROFF} ${PAGES} -ms > setup.NROFF

clean:
	rm -f setup.out ?.t.spell errs Errs MAKELOG

spell:	${SRC}
	@for i in ${SRC}; do \
		echo $$i; spell $$i | sort | comm -23 - spell.ok > $$i.spell; \
	done
