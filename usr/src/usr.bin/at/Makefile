#
# Copyright (c) 1983 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.4.2 (2.11BSD) 2025/11/25
#
#       Makefile for at, atq, atrm, and atrun
#
ALL=		atq atrm at atrun
SRCS=		atq.c atrm.c at.c atrun.c
CFLAGS=		-O
SPOOLDIRS=	/usr/spool/at /usr/spool/at/past
SPOOLFILES=	/usr/spool/at/lasttimedone
LDFLAGS= -i

all: ${ALL}

at: at.c
	$(CC) $(CFLAGS) ${LDFLAGS} -o at at.c

atq: atq.c
	$(CC) $(CFLAGS) ${LDFLAGS} -o atq atq.c

atrm: atrm.c
	$(CC) $(CFLAGS) ${LDFLAGS} -o atrm atrm.c

atrun: atrun.c
	$(CC) $(CFLAGS) ${LDFLAGS} -o atrun atrun.c

install: all
	-for i in $(SPOOLDIRS); \
		do mkdir $$i; \
		chown daemon.daemon $$i; \
		chmod 755    $$i; \
		done
	-for i in $(SPOOLFILES); \
		do if [ ! -f $$i ]; \
		then echo 0 > $$i; fi; \
		chown daemon.daemon $$i; \
		done
	install -c -m 755 -s -o daemon atrun /usr/libexec
	install -c -s -o daemon -m 4755 atq /usr/bin
	install -c -s -o daemon -m 4755 at /usr/bin
	install -c -s -o daemon -m 4755 atrm /usr/bin

clean:
	rm -f $(ALL) errs *.o

depend:
	chmod u+w Makefile
	mkdep ${CFLAGS} ${SRCS}
