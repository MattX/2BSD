#
# Copyright (c) 1986 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	6.2.2 (2.11BSD) 2025/12/10
#

SRC=	0.t 1.t 2.t 3.t 4.t 5.t 6.t a.t b.t

all:
	@echo pick one of troff or nroff

troff: ${SRC}
	eqn ${SRC} | tbl | troff -t ${PAGES} -ms > setup.out

nroff: ${SRC}
	eqn ${SRC} | tbl | nroff -Tlpr ${PAGES} -ms > setup.out

clean:
	rm -f setup.out ?.t.spell

spell:	${SRC}
	@for i in ${SRC}; do \
		echo $$i; spell $$i | sort | comm -23 - spell.ok > $$i.spell; \
	done
