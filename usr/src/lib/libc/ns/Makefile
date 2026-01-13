#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	6.2 (2.11BSD) 2025/11/29
#

SRCS=	ns_addr.c ns_ntoa.c
OBJS=	ns_addr.o ns_ntoa.o
CFLAGS=	-O ${DEFS}
TAGSFILE=tags

.c.o:
	${CC} -p -c ${CFLAGS} $*.c
	-ld -X -r $*.o
	mv a.out profiled/$*.o
	${CC} ${CFLAGS} -c $*.c
	-ld -x -r $*.o
	mv a.out $*.o

nslib nslib_p: ${OBJS}
	@echo "building profiled nslib"
	@cd profiled; ar cru ../nslib_p ${OBJS}
	@echo "building normal nslib"
	@ar cru nslib ${OBJS}

tags:
	cwd=`pwd`; \
	for i in ${SRCS}; do \
		ctags -a -f ${TAGSFILE} $$cwd/$$i; \
	done

clean:
	rm -f nslib nslib_p *.o profiled/*.o errs a.out core \
		tags Makefile.bak

depend:	${SRCS}
	mkdep ${CFLAGS} ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

ns_addr.o: ns_addr.c /usr/include/sys/types.h /usr/include/sys/select.h
ns_addr.o: /usr/include/sys/types.h /usr/include/netns/ns.h
ns_ntoa.o: ns_ntoa.c /usr/include/sys/types.h /usr/include/sys/select.h
ns_ntoa.o: /usr/include/sys/types.h /usr/include/netns/ns.h

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
