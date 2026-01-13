#
# Copyright (c) 1983 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.9 (2.11BSD) 2025/11/25
#
# Bug report processor and associated programs
#

BUGS_NAME = bugs
BUGS_HOME = @2BSD.COM
DEFS = -DBUGS_NAME='"$(BUGS_NAME)"' -DBUGS_HOME='"$(BUGS_HOME)"'
CFLAGS = -O -DUNIXCOMP $(DEFS)

all: bugfiler unixtomh

bugfiler: bugfiler.c
	${CC} ${CFLAGS} -i -DUNIXTOMH='"/usr/libexec/unixtomh"' -o bugfiler bugfiler.c

unixtomh: unixtomh.c
	${CC} -i ${CFLAGS} -o unixtomh unixtomh.c

install: bugfiler sendbug.sh bugformat unixtomh
	install -c -s -m 755 bugfiler /usr/libexec/bugfiler
	install -c -m 755 sendbug.sh /usr/ucb/sendbug
	install -c -s -m 755 unixtomh /usr/libexec/unixtomh
	install -c -m 644 bugformat /usr/share/misc/bugformat

clean:
	rm -f *.o bugfiler unixtomh
