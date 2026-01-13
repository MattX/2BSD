#
# Copyright (c) 1985 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.2 (2.11BSD) 2025/11/25
#
# Make a Yacc library
#
CFLAGS=-O 

liby.a:	libmai.o libzer.o
	ar rv liby.a libmai.o libzer.o
	ranlib liby.a

install: liby.a
	install -c -m 644 -o root liby.a /usr/lib/liby.a
	ranlib /usr/lib/liby.a

clean:
	rm -f *.o *.a errs
