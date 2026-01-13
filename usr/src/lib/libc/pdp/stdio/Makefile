#
# Copyright (c) 1987 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.6.3 (2.11BSD) 2025/11/29
#
# SRCS=	doprnt.s ffltpr.s fgetc.s fgets.s fltpr.s fputc.s puts.s gets.s
# OBJS=	doprnt.o ffltpr.o fgetc.o fgets.o fltpr.o fputc.o puts.o gets.o
SRCS=	doprnt.s ffltpr.s fgetc.s fgets.s fltpr.s fputc.s gets.s
OBJS=	doprnt.o ffltpr.o fgetc.o fgets.o fltpr.o fputc.o gets.o
CFLAGS= -O ${DEFS}
TAGSFILE=tags

.s.o:
	/lib/cpp -t -E -DPROF ${DEFS} $< | sed -e 's;^#;/;' | as -V -o $@
	-ld -X -r $*.o
	mv a.out profiled/$*.o
	/lib/cpp -t -E ${DEFS} $< | sed -e 's;^#;/;' | as -V -o $@
	-ld -x -r $*.o
	mv a.out $*.o

.c.o:
	cc -p -c ${CFLAGS} $*.c
	-ld -X -r $*.o
	mv a.out profiled/$*.o
	cc -c ${CFLAGS} $*.c
	-ld -x -r $*.o
	mv a.out $*.o

stdiolib stdiolib_p: ${OBJS}
	@echo "building profiled stdio"
	@cd profiled; ar cru ../stdiolib_p ${OBJS}
	@echo "building normal stdio"
	@ar cru stdiolib ${OBJS}

tags:

clean:
	rm -f *.o profiled/*.o errs a.out core stdiolib stdiolib_p \
		tags Makefile.bak

depend:	${SRCS}
	mkdep ${CFLAGS} ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

doprnt.o: doprnt.s DEFS.h
ffltpr.o: ffltpr.s
fgetc.o: fgetc.s DEFS.h STDIO.h
fgets.o: fgets.s DEFS.h STDIO.h
fltpr.o: fltpr.s DEFS.h
fputc.o: fputc.s DEFS.h STDIO.h
gets.o: gets.s DEFS.h STDIO.h

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
