#
# Copyright (c) 1983 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.5 (2.11BSD) 2025/11/29

OBJS=	gethnamadr.o gethostent.o
SRCS=	gethnamadr.c gethostent.c
CFLAGS=	-O ${DEFS}

.c.o: 
	${CC} -p -c ${CFLAGS} $*.c
	-ld -X -r $*.o
	mv a.out profiled/$*.o
	${CC} ${CFLAGS} -c $*.c
	-ld -x -r $*.o
	mv a.out $*.o

hostlib hostlib_p: ${OBJS} 
	@echo "building profiled hostlib"
	@cd profiled; ar cru ../hostlib_p ${OBJS}
	@echo "building normal netlib"
	@ar cru hostlib ${OBJS}
	
tags:
	cwd=`pwd`; \
	for i in ${SRCS}; do \
		ctags -a -f ${TAGSFILE} $$cwd/$$i; \
	done

clean:
	rm -f *.o errs a.out core hostlib hostlib_p profiled/*.o \
		tags Makefile.bak

depend:	${SRCS}
	mkdep ${CFLAGS} ${SRCS}
