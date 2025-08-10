#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	7.0 (2.11BSD) 2025/3/17
#
SRCS=	adb.ms

adb.n:	${SRCS}
	neqn ${SRCS} | tbl | nroff -ms >adb.n

clean:
	rm -f adb.n

spell:	${SRCS}
	@for i in ${SRCS}; do \
		echo $$i; spell $$i | sort | comm -23 - spell.ok > $$i.spell; \
	done
