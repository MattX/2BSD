#
# Copyright (c) 1983 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.3 (2.11BSD) 2025/11/25
#
ALL=	tftp tftp.0
CFLAGS=-O
LDFLAGS=-i

all: ${ALL}

tftp:	main.o tftp.o tftpsubs.o
	${CC} main.o tftp.o tftpsubs.o ${LDFLAGS} -o tftp

tftp.0:	tftp.1
	/usr/man/manroff tftp.1 > tftp.0

clean:
	rm -f ${ALL} *.o *.s errs core a.out t.? tftp.0

install:  tftp tftp.0
	install -c -s -m 755 tftp /usr/ucb
	install -c -o root -g staff -m 444 tftp.0 /usr/man/cat1
