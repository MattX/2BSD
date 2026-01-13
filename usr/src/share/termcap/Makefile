#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.2.5 (2.11BSD) 2025/11/25
#
# reorder gives an editor command for most common terminals
# (in reverse order from n'th to 1'st most commonly used)
# to move them to the front of termcap
#

termcap: reorder termcap.src
	ex - termcap.src < reorder

install: termcap tabset map3270
	install -c -m 444 termcap /usr/share/misc/termcap
	rm -f /etc/termcap
	ln -s ../usr/share/misc/termcap /etc/termcap
	install -c -m 444 map3270 /usr/share/misc/map3270

tabset: FRC
	rm -fr /usr/share/tabset
	mkdir /usr/share/tabset
	chmod 755 /usr/share/tabset
	install -c -m 444 tabset/* /usr/share/tabset

clean:
	rm -f termcap

FRC:
