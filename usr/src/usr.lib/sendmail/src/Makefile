#
#  Sendmail
#  Copyright (c) 1983  Eric P. Allman
#  Berkeley, California
#
#  Copyright (c) 1983 Regents of the University of California.
#  All rights reserved.  The Berkeley software License Agreement
#  specifies the terms and conditions for redistribution.
#
#	@(#)Makefile.m4	5.10 (Berkeley) 5/2/86
#
#
#  SENDMAIL Makefile.
#
#

# Uncomment these two lines to enable string extractions
CC= ./mkstrcc
XSTR=/usr/ucb/xstr
EXTRACT=extract.o

LIBS=	-ldbm
DESTDIR=

OBJS1=	conf.o main.o collect.o parseaddr.o alias.o deliver.o \
	savemail.o err.o readcf.o stab.o headers.o recipient.o \
	stats.o daemon.o usersmtp.o srvrsmtp.o queue.o \
	macro.o util.o clock.o trace.o envelope.o
OBJS2=	sysexits.o arpadate.o convtime.o ctime.o
OBJS=	$(OBJS1) $(OBJS2) $(EXTRACT) str.o

SBASE=	conf.o collect.o parseaddr.o alias.o deliver.o headers.o \
	recipient.o srvrsmtp.o queue.o util.o \
	envelope.o sysexits.o arpadate.o convtime.o Version.o \
	ctime.o $(EXTRACT) str.o
SOV1=	main.o readcf.o macro.o
SOV2=	daemon.o savemail.o usersmtp.o err.o clock.o stats.o trace.o stab.o

SRCS1=	conf.h sendmail.h \
	conf.c deliver.c main.c parseaddr.c err.c alias.c savemail.c \
	sysexits.c util.c arpadate.c version.c collect.c \
	macro.c headers.c readcf.c stab.c recipient.c stats.c daemon.c \
	usersmtp.c srvrsmtp.c queue.c clock.c trace.c envelope.c
SRCS2=	TODO convtime.c ctime.c
SRCS=	Version.c $(SRCS1) $(SRCS2)
ALL=	sendmail ctimed

CHOWN=	-echo chown
CHMOD=	chmod
O=	-O
COPTS=
CCONFIG=-I../include -DVMUNIX -DNOSCCS
CFLAGS=	$O $(COPTS) $(CCONFIG)
SEPFLAG=-i
AR=	-ar
ARFLAGS=rvu
LINT=	lint
XREF=	ctags -x
CP=	cp
MV=	mv
INSTALL=install -c -s -o root
M4=	m4
TOUCH=	touch
ABORT=	false

GET=	sccs get
DELTA=	sccs delta
WHAT=	what
PRT=	sccs prt
REL=

ROOT=	root
OBJMODE=755

.c.o:
	$(CC) ${CFLAGS} -c $*.c

sendmail: $(OBJS) Version.o
	ld $(SEPFLAG) $(COPTS) /lib/crt0.o -o sendmail \
		-Z $(SOV1) -Z $(SOV2) -Y $(SBASE) $(LIBS) -lc
	$(CHMOD) $(OBJMODE) sendmail
	size sendmail; ls -l sendmail; 

ctimed:
	cc $(SEPFLAG) $(CFLAGS) ctimed.c -o ctimed

install: all
	$(INSTALL) -m 4755 sendmail $(DESTDIR)/usr/lib
	chgrp kmem $(DESTDIR)/usr/lib/sendmail
	$(CP) /dev/null $(DESTDIR)/usr/lib/sendmail.fc
	-if [ -s sendmail.sr ]; then \
		install -c -o bin -m 644 sendmail.sr \
			$(DESTDIR)/usr/lib/sendmail.sr; \
	fi
	install -c -s -o bin -m 0755 ctimed $(DESTDIR)/usr/lib/ctimed

version: newversion $(OBJS) Version.c

newversion:
	@rm -f SCCS/p.version.c version.c
	@$(GET) $(REL) -e SCCS/s.version.c
	@$(DELTA) -s SCCS/s.version.c
	@$(GET) -t -s SCCS/s.version.c

fullversion: $(OBJS) dumpVersion Version.o

dumpVersion:
	rm -f Version.c


$(OBJS1): sendmail.h
$(OBJS): conf.h
stats.o: mailstats.h

sendmail.h util.o: ../include/useful.h

all: $(ALL)

#
#  Auxiliary support entries
#

clean:
	rm -f core sendmail rmail usersmtp uucp a.out XREF sendmail.cf
	rm -f sendmail.sr *.o ctimed strings

sources: $(SRCS)


print: $(SRCS)
	@ls -l | pr -h "sendmail directory"
	@$(XREF) *.c | pr -h "cross reference listing"
	@size *.o | pr -h "object code sizes"
	@pr Makefile *.m4 *.h *.[cs]

lint:
	$(LINT) $(CCONFIG) $(SRCS1)

str.o:	strings
	${XSTR}
	cc -c xs.c
	mv xs.o str.o
	rm -f xs.c

