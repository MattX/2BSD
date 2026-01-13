#
# Copyright (c) 1987 Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms are permitted
# provided that the above copyright notice and this paragraph are
# duplicated in all such forms and that any documentation,
# advertising materials, and other materials related to such
# distribution and use acknowledge that the software was developed
# by the University of California, Berkeley.  The name of the
# University may not be used to endorse or promote products derived
# from this software without specific prior written permission.
# THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
# WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
#	@(#)Makefile	5.20.3 (2.11BSD) 2025/11/28
#
CFLAGS=	-O
LIBS=	-ll
CSRCS=	main.c getinfo.c debug.c send.c skip.c list.c subr.c res_debug.c
SRCS=	${CSRCS} commands.c
OBJS=	main.o getinfo.o debug.o send.o skip.o list.o subr.o commands.o \
	res_debug.o

all: nslookup nslookup.0

nslookup: ${OBJS}
	${CC} -i -o $@ ${OBJS} ${LIBS}

clean:
	rm -f ${OBJS} core nslookup nslookup.0 commands.c lex.yy.c lex.yy.o

cleandir: clean
	rm -f tags .depend

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: nslookup nslookup.0
	install -c -s -o root -g staff -m 755 nslookup /usr/bin/nslookup
	install -c -o root -g staff -m 444 nslookup.help /usr/share/misc/nslookup.help
	install -c -o root -g staff -m 444 nslookup.0 /usr/man/cat1/nslookup.0

nslookup.0: nslookup.8
	/usr/man/manroff nslookup.8 > nslookup.0

lint: ${SRCS}
	lint -ax ${SRCS}

tags: ${CSRCS}
	ctags ${CSRCS}
