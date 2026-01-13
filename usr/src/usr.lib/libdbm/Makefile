#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.4 (2.11BSD) 2025/11/27
#
CFLAGS=-O -DNOSCCS
TAGSFILE=tags

all: libdbm.a libdbm_p.a

libdbm.a: dbm.c
	${CC} -c ${CFLAGS} dbm.c
	ld -r -o libdbm.a -x dbm.o
	rm -f dbm.o

libdbm_p.a: dbm.c
	${CC} -p -c ${CFLAGS} dbm.c
	ld -r -o libdbm_p.a -X dbm.o
	rm -f dbm.o

install: all
	install -c -m 644 libdbm.a /usr/lib
	install -c -m 644 libdbm_p.a /usr/lib
	-cmp -s dbm.h /usr/include/dbm.h || \
		install -c -m 444 dbm.h /usr/include

tags:
	ctags -a -f ${TAGSFILE} `pwd`/dbm.c

clean:
	rm -f *.o libdbm.a libdbm_p.a errs a.out core
