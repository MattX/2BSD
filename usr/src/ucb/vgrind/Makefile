#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.1.2 (2.11BSD) 2025/11/25
#
CFLAGS=-O 
LDFLAGS= -i
SOURCES=vfontedpr.c vgrindefs.c regexp.c retest.c
CMDS=vfontedpr retest
OBJS=retest.o regexp.o vfontedpr.o vgrindefs.o

all:	$(CMDS) ${OBJS}

vfontedpr: vfontedpr.o vgrindefs.o regexp.o
	cc ${LDFLAGS} ${CFLAGS} -o vfontedpr vfontedpr.o vgrindefs.o regexp.o
retest: retest.o regexp.o
	cc ${LDFLAGS} ${CFLAGS} -o retest retest.c regexp.o

install: all
	install -c -s -m 755 -o root vfontedpr /usr/libexec/vfontedpr
	install -c -m 755 -o root vgrind.sh /usr/ucb/vgrind
	install -c -m 444 -o root vgrindefs.src /usr/share/misc/vgrindefs
	install -c -m 444 tmac.vgrind /usr/share/tmac/tmac.vgrind

clean:
	rm -f ${CMDS} ${OBJS}

vprintall:
	csh /usr/ucb/vprint ${SOURCES}
