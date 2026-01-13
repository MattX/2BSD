#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.4.2 (2.11BSD) 2025/11/29
#
CFLAGS=	-O ${DEFS}
SRCS=ftime.c gtty.c nice.c pause.c rand.c stty.c \
	times.c utime.c
OBJS=	ftime.o gtty.o nice.o pause.o rand.o stty.o \
	times.o utime.o
TAGSFILE=tags

.c.o:
	${CC} -p ${CFLAGS} -c $*.c
	-ld -X -r $*.o
	mv a.out profiled/$*.o
	${CC} ${CFLAGS} -c $*.c
	-ld -x -r $*.o
	mv a.out $*.o

compat-4.1lib compat-4.1lib_p: ${OBJS}
	@echo "building profiled compat-4.1lib"
	@cd profiled; ar cru ../compat-4.1lib_p ${OBJS}
	@echo "buiding normal compat-4.1lib"
	@ar cru compat-4.1lib ${OBJS}

tags:
	cwd=`pwd`; \
	for i in ${SRCS}; do \
		ctags -a -f ${TAGSFILE} $$cwd/$$i; \
	done

clean:
	rm -f compat-4.1lib compat-4.1lib_p *.o profiled/*.o tags Makefile.bak

depend:	${SRCS}
	mkdep ${CFLAGS} ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

ftime.o: ftime.c /usr/include/sys/types.h /usr/include/sys/select.h
ftime.o: /usr/include/sys/types.h /usr/include/sys/time.h
ftime.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/sys/types.h
gtty.o: gtty.c /usr/include/sgtty.h /usr/include/sys/ioctl.h
gtty.o: /usr/include/sys/ttychars.h /usr/include/sys/ttydev.h
nice.o: nice.c /usr/include/sys/time.h /usr/include/sys/types.h
nice.o: /usr/include/sys/select.h /usr/include/sys/types.h /usr/include/time.h
nice.o: /usr/include/sys/types.h /usr/include/sys/resource.h
pause.o: pause.c /usr/include/signal.h
rand.o: rand.c
stty.o: stty.c /usr/include/sgtty.h /usr/include/sys/ioctl.h
stty.o: /usr/include/sys/ttychars.h /usr/include/sys/ttydev.h
times.o: times.c /usr/include/sys/time.h /usr/include/sys/types.h
times.o: /usr/include/sys/select.h /usr/include/sys/types.h /usr/include/time.h
times.o: /usr/include/sys/types.h /usr/include/sys/resource.h
times.o: /usr/include/sys/types.h /usr/include/sys/times.h
utime.o: utime.c /usr/include/sys/time.h /usr/include/sys/types.h
utime.o: /usr/include/sys/select.h /usr/include/sys/types.h /usr/include/time.h
utime.o: /usr/include/sys/types.h /usr/include/sys/types.h

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
