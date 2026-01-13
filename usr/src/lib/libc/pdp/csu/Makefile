#
# Copyright (c) 1987 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.6.4 (2.11BSD) 2025/11/29
#
#	crt0	Normal C run time startoff
#	mcrt0	C run time start off for profiling, ``prof'' conventions
#
SRCS=	crt0.s mon.c mcount.s
OBJS=	crt0.o mcrt0.o mon.o
CFLAGS=	-O ${DEFS}
TAGSFILE=tags

all: ${OBJS}

install: ${OBJS}
	install -c -m 0644 crt0.o /lib/crt0.o
	install -c -m 0644 mcrt0.o /lib/mcrt0.o

crt0.o:	crt0.s
	/lib/cpp -t ${DEFS} ${DFLAGS} crt0.s | sed 's;^#;/;' | as -o x.o
	ld -x -r -o crt0.o x.o
	rm -f x.o

moncrt0.o: crt0.s
	/lib/cpp -t ${DEFS} ${DFLAGS} -DMCRT0 crt0.s | sed 's;^#;/;' | as -o x.o
	ld -x -r -o moncrt0.o x.o
	rm -f x.o

mcrt0.o: moncrt0.o mon.o
	ld -x -r -o mcrt0.o moncrt0.o mon.o

mon.o: mon.c mcount.s mon.ex
	cc ${CFLAGS} -S ${DFLAGS} mon.c
	ex - mon.s < mon.ex
	cat mon.s mcount.s > x.s
	/lib/cpp -t ${DEFS} ${DFLAGS} x.s | sed 's;^#;/;' > xx.s
	as - -o x.o xx.s
	ld -x -r -o mon.o x.o
	rm -f x.o x.s xx.s mon.s

tags:	
	cwd=`pwd`; \
	for i in *.c; do \
		ctags -a -f ${TAGSFILE} $$cwd/$$i; \
	done

clean:
	rm -f ${OBJS} *.o core errs tags Makefile.bak

lint:

depend:	${SRCS}
	mkdep ${CFLAGS} ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

crt0.o: crt0.s
mon.o: mon.c
mcount.o: mcount.s ../sys/SYS.h /usr/include/syscall.h

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
