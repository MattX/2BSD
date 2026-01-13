# Copyright (c) 1987 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	4.3.2	(2.11BSD)	2025/11/25
#
LESSONS=C editor eqn files macros morefiles vi
FILES=	Linfo Xinfo

all:

install: ${LESSONS} ${FILES}
	-mkdir -p /usr/share/learn \
		/var/log/learn \
		/usr/share/learn/bin
	chown root.staff /usr/share/learn \
		/usr/share/learn/bin
	chmod 755 /usr/share/learn \
		/var/log/learn \
		/usr/share/learn/bin
	@for i in ${LESSONS}; do \
		touch /var/log/learn/$$i; \
		chmod 666 /var/log/learn/$$i; \
	done
	@for i in ${FILES}; do \
		echo copy $$i; \
		install -c -m 644 $$i /usr/share/learn/$$i; \
	done
# For some unknown reason the following line sometimes fails with an
# exit -115 ...
	-tar cf - ${LESSONS} | (cd /usr/share/learn; tar xpf -)
	-chmod 555 /usr/share/learn/*/Init
	cd /usr/share/learn/C; cc -O -c getline.c getnum.c

clean:
