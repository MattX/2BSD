#
# Copyright (c) 1987 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.7.2 (2.11BSD) 2025/11/29
#
SRCS=	fetchi.s fperr.s l3.c lock.s phys.s ucall.s
OBJS=	fetchi.o fperr.o l3.o lock.o phys.o ucall.o
CFLAGS= -O ${DEFS}
TAGSFILE=tags

.s.o:
	/lib/cpp -E -DPROF ${DEFS} $< | sed -e 's;^#;/;' | as -V -o $@
	-ld -X -r $*.o
	mv a.out profiled/$*.o
	/lib/cpp -E ${DEFS} $< | sed -e 's;^#;/;' | as -V -o $@
	-ld -x -r $*.o
	mv a.out $*.o

.c.o:
	cc -p -c ${CFLAGS} $*.c
	-ld -X -r $*.o
	mv a.out profiled/$*.o
	cc -c ${CFLAGS} $*.c
	-ld -x -r $*.o
	mv a.out $*.o

compat-2.9lib compat-2.9lib_p: ${OBJS}
	@echo "building profiled compat-2.9"
	@cd profiled; ar cru ../compat-2.9lib_p ${OBJS}
	@echo "building normal compat-2.9"
	@ar cru compat-2.9lib ${OBJS}

tags:

clean:
	rm -f *.o profiled/*.o errs a.out core compat-2.9lib compat-2.9lib_p \
		tags Makefile.bak

depend:	${SRCS}
	mkdep ${CFLAGS} ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

fetchi.o: fetchi.s SYS.h /usr/include/syscall.h
fperr.o: fperr.s SYS.h /usr/include/syscall.h
l3.o: l3.c
lock.o: lock.s SYS.h /usr/include/syscall.h
phys.o: phys.s SYS.h /usr/include/syscall.h
ucall.o: ucall.s SYS.h /usr/include/syscall.h

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
