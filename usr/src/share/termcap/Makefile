#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.2.1 (2.11BSD GTE) 1/23/94
#
# reorder gives an editor command for most common terminals
# (in reverse order from n'th to 1'st most commonly used)
# to move them to the front of termcap
#
DESTDIR =

termcap: reorder termcap.src
	ex - termcap.src < reorder

install: termcap tabset
	install -m 444 termcap ${DESTDIR}/etc/termcap

tabset: FRC
	rm -fr ${DESTDIR}/usr/lib/tabset
	mkdir ${DESTDIR}/usr/lib/tabset
	chmod 755 ${DESTDIR}/usr/lib/tabset
	install -c -m 444 tabset/* ${DESTDIR}/usr/lib/tabset

clean:
	rm -f termcap

FRC:
