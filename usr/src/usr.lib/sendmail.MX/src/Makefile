#
# Copyright (c) 1988 Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms are permitted
# provided that this notice is preserved and that due credit is given
# to the University of California at Berkeley. The name of the University
# may not be used to endorse or promote products derived from this
# software without specific prior written permission. This software
# is provided ``as is'' without express or implied warranty.
#
#	%W% (Berkeley) %G%
#
# Uncomment these two lines to enable string extractions
CC=	./mkstrcc
EXTRACT=extract.o

SRCS=	alias.c arpadate.c clock.c collect.c conf.c convtime.c daemon.c \
	deliver.c domain.c envelope.c err.c headers.c macro.c main.c \
	parseaddr.c queue.c recipient.c savemail.c srvrsmtp.c \
	stab.c stats.c sysexits.c trace.c usersmtp.c util.c Version.c
BASE=	clock.o conf.o daemon.o envelope.o macro.o main.o queue.o \
	parseaddr.o recipient.o savemail.o stab.o srvrsmtp.o \
	trace.o util.o $(EXTRACT)
OVLY1=	sysexits.o arpadate.o convtime.o readcf.o headers.o
OVLY2=	collect.o deliver.o
OVLY3=	alias.o err.o domain.o stats.o usersmtp.o
OBJS=	${BASE} ${OVLY1} ${OVLY2} ${OVLY3}

CFLAGS=	-O -I. -I../include -DVMUNIX -DMXDOMAIN -DNOSCCS
# CFLAGS=	-O -I. -I../include -DVMUNIX -DNOSCCS
SEPFLAG=-i
ASMSED=	../include/asm.sed

GET=	sccs get
DELTA=	sccs delta
WHAT=	what
PRT=	sccs prt

.c.o:
	$(CC) -S ${CFLAGS} $*.c
	sed -f $(ASMSED) $*.s >_xx.s
	as -V - -o $*.o _xx.s
	rm -f $*.s _xx.s

sendmail: ${OBJS} Version.o
	-if [ X${SEPFLAG} = X ]; then \
		${CC} ${COPTS} -o $@ Version.o ${OBJS} -ldbm; \
	elif [ X${SEPFLAG} = X-i ]; then \
		cc ${SEPFLAG} ${COPTS} -o sendmail Version.o ${BASE} \
			-Z ${OVLY1} -Z ${OVLY2} -Z ${OVLY3} -Y -ldbm; \
	else \
		echo "Need an overlay scheme for non-separate I&D load"; \
	fi
	size sendmail; ls -l sendmail; ${WHAT} < Version.o

dsendmail: ${OBJS} Version.o
	${CC} ${COPTS} -o sendmail Version.o ${OBJS} -ldbm -lresolv
	size sendmail; ls -l sendmail; ${WHAT} < Version.o

install: sendmail 
	install -c -s -o root -g kmem -m 6755 sendmail ${DESTDIR}/usr/lib
	install -c -o bin -m 644 /dev/null ${DESTDIR}/usr/lib/sendmail.fc
	install -c -o bin -m 666 /dev/null ${DESTDIR}/usr/lib/sendmail.st
	rm -f ${DESTDIR}/usr/ucb/newaliases
	ln -s ${DESTDIR}/usr/lib/sendmail ${DESTDIR}/usr/ucb/newaliases
	rm -f ${DESTDIR}/usr/ucb/mailq
	ln -s ${DESTDIR}/usr/lib/sendmail ${DESTDIR}/usr/ucb/mailq
	-if [ -s sendmail.sr ]; then \
		install -c -o bin -m 644 sendmail.sr \
			$(DESTDIR)/usr/lib/sendmail.sr; \
	fi

version: newversion ${OBJS} Version.c

newversion:
	@rm -f SCCS/p.version.c version.c
	@${GET} -e SCCS/s.version.c
	@${DELTA} -s SCCS/s.version.c
	@${GET} -t -s SCCS/s.version.c

Version.c: version.c
	@echo generating Version.c from version.c
	@cp version.c Version.c
	@chmod 644 Version.c
	@echo "" >> Version.c
	@echo "# ifdef COMMENT" >> Version.c
#	@${PRT} SCCS/s.version.c >> Version.c
	@echo "" >> Version.c
	@echo "code versions:" >> Version.c
	@echo "" >> Version.c
	@${WHAT} ${OBJS} >> Version.c
	@echo "" >> Version.c
	@echo "# endif COMMENT" >> Version.c

clean: FRC
	rm -f ${OBJS} core sendmail a.out Version.o sendmail.sr mk.c xxmk.c

depend: FRC
	mkdep ${CFLAGS} ${SRCS}

lint: FRC
	lint -chapbx ${CCONFIG} ${SRCS}

tags: FRC
	ctags ${SRCS}

FRC:

# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

alias.o: alias.c /usr/include/pwd.h /usr/include/sys/types.h
alias.o: /usr/include/sys/stat.h /usr/include/signal.h /usr/include/errno.h
alias.o: sendmail.h /usr/include/stdio.h /usr/include/ctype.h
alias.o: /usr/include/setjmp.h conf.h ../include/useful.h
alias.o: /usr/include/sys/syslog.h /usr/include/sys/socket.h
alias.o: /usr/include/short_names.h /usr/include/netinet/in.h
alias.o: /usr/include/short_names.h /usr/include/sysexits.h
alias.o: /usr/include/sys/file.h
arpadate.o: arpadate.c conf.h /usr/include/sys/time.h /usr/include/time.h
arpadate.o: /usr/include/sys/types.h /usr/include/sys/timeb.h
arpadate.o: ../include/useful.h
clock.o: clock.c sendmail.h /usr/include/stdio.h /usr/include/ctype.h
clock.o: /usr/include/setjmp.h conf.h ../include/useful.h
clock.o: /usr/include/sys/types.h /usr/include/sys/syslog.h
clock.o: /usr/include/sys/socket.h /usr/include/short_names.h
clock.o: /usr/include/netinet/in.h /usr/include/short_names.h
clock.o: /usr/include/sysexits.h /usr/include/signal.h
collect.o: collect.c /usr/include/errno.h sendmail.h /usr/include/stdio.h
collect.o: /usr/include/ctype.h /usr/include/setjmp.h conf.h
collect.o: ../include/useful.h /usr/include/sys/types.h
collect.o: /usr/include/sys/syslog.h /usr/include/sys/socket.h
collect.o: /usr/include/short_names.h /usr/include/netinet/in.h
collect.o: /usr/include/short_names.h /usr/include/sysexits.h
conf.o: conf.c /usr/include/pwd.h /usr/include/sys/ioctl.h
conf.o: /usr/include/sys/ttychars.h /usr/include/sys/ttydev.h sendmail.h
conf.o: /usr/include/stdio.h /usr/include/ctype.h /usr/include/setjmp.h conf.h
conf.o: ../include/useful.h /usr/include/sys/types.h /usr/include/sys/syslog.h
conf.o: /usr/include/sys/socket.h /usr/include/short_names.h
conf.o: /usr/include/netinet/in.h /usr/include/short_names.h
conf.o: /usr/include/sysexits.h /usr/include/sys/stat.h /usr/include/sys/wait.h
convtime.o: convtime.c /usr/include/ctype.h ../include/useful.h
convtime.o: /usr/include/sys/types.h
daemon.o: daemon.c /usr/include/errno.h sendmail.h /usr/include/stdio.h
daemon.o: /usr/include/ctype.h /usr/include/setjmp.h conf.h ../include/useful.h
daemon.o: /usr/include/sys/types.h /usr/include/sys/syslog.h
daemon.o: /usr/include/sys/socket.h /usr/include/short_names.h
daemon.o: /usr/include/netinet/in.h /usr/include/short_names.h
daemon.o: /usr/include/sysexits.h /usr/include/netdb.h
daemon.o: /usr/include/short_names.h /usr/include/sys/signal.h
daemon.o: /usr/include/sys/wait.h /usr/include/sys/time.h /usr/include/time.h
daemon.o: /usr/include/sys/resource.h /usr/include/arpa/inet.h
daemon.o: /usr/include/short_names.h
deliver.o: deliver.c sendmail.h /usr/include/stdio.h /usr/include/ctype.h
deliver.o: /usr/include/setjmp.h conf.h ../include/useful.h
deliver.o: /usr/include/sys/types.h /usr/include/sys/syslog.h
deliver.o: /usr/include/sys/socket.h /usr/include/short_names.h
deliver.o: /usr/include/netinet/in.h /usr/include/short_names.h
deliver.o: /usr/include/sysexits.h /usr/include/sys/signal.h
deliver.o: /usr/include/sys/stat.h /usr/include/sys/ioctl.h
deliver.o: /usr/include/sys/ttychars.h /usr/include/sys/ttydev.h
deliver.o: /usr/include/netdb.h /usr/include/short_names.h /usr/include/errno.h
deliver.o: /usr/include/arpa/nameser.h /usr/include/resolv.h
domain.o: domain.c sendmail.h /usr/include/stdio.h /usr/include/ctype.h
domain.o: /usr/include/setjmp.h conf.h ../include/useful.h
domain.o: /usr/include/sys/types.h /usr/include/sys/syslog.h
domain.o: /usr/include/sys/socket.h /usr/include/short_names.h
domain.o: /usr/include/netinet/in.h /usr/include/short_names.h
domain.o: /usr/include/sysexits.h /usr/include/sys/param.h
domain.o: /usr/include/sys/localopts.h /usr/include/machine/machparam.h
domain.o: /usr/include/signal.h /usr/include/sys/types.h
domain.o: /usr/include/arpa/nameser.h /usr/include/resolv.h
domain.o: /usr/include/netdb.h /usr/include/short_names.h
envelope.o: envelope.c /usr/include/pwd.h /usr/include/sys/time.h
envelope.o: /usr/include/time.h sendmail.h /usr/include/stdio.h
envelope.o: /usr/include/ctype.h /usr/include/setjmp.h conf.h
envelope.o: ../include/useful.h /usr/include/sys/types.h
envelope.o: /usr/include/sys/syslog.h /usr/include/sys/socket.h
envelope.o: /usr/include/short_names.h /usr/include/netinet/in.h
envelope.o: /usr/include/short_names.h /usr/include/sysexits.h
envelope.o: /usr/include/sys/stat.h
err.o: err.c sendmail.h /usr/include/stdio.h /usr/include/ctype.h
err.o: /usr/include/setjmp.h conf.h ../include/useful.h
err.o: /usr/include/sys/types.h /usr/include/sys/syslog.h
err.o: /usr/include/sys/socket.h /usr/include/short_names.h
err.o: /usr/include/netinet/in.h /usr/include/short_names.h
err.o: /usr/include/sysexits.h /usr/include/errno.h /usr/include/netdb.h
err.o: /usr/include/short_names.h
headers.o: headers.c /usr/include/errno.h sendmail.h /usr/include/stdio.h
headers.o: /usr/include/ctype.h /usr/include/setjmp.h conf.h
headers.o: ../include/useful.h /usr/include/sys/types.h
headers.o: /usr/include/sys/syslog.h /usr/include/sys/socket.h
headers.o: /usr/include/short_names.h /usr/include/netinet/in.h
headers.o: /usr/include/short_names.h /usr/include/sysexits.h
macro.o: macro.c sendmail.h /usr/include/stdio.h /usr/include/ctype.h
macro.o: /usr/include/setjmp.h conf.h ../include/useful.h
macro.o: /usr/include/sys/types.h /usr/include/sys/syslog.h
macro.o: /usr/include/sys/socket.h /usr/include/short_names.h
macro.o: /usr/include/netinet/in.h /usr/include/short_names.h
macro.o: /usr/include/sysexits.h
main.o: main.c /usr/include/signal.h /usr/include/sgtty.h
main.o: /usr/include/sys/ioctl.h /usr/include/sys/ttychars.h
main.o: /usr/include/sys/ttydev.h sendmail.h /usr/include/stdio.h
main.o: /usr/include/ctype.h /usr/include/setjmp.h conf.h ../include/useful.h
main.o: /usr/include/sys/types.h /usr/include/sys/syslog.h
main.o: /usr/include/sys/socket.h /usr/include/short_names.h
main.o: /usr/include/netinet/in.h /usr/include/short_names.h
main.o: /usr/include/sysexits.h /usr/include/arpa/nameser.h
main.o: /usr/include/resolv.h
parseaddr.o: parseaddr.c sendmail.h /usr/include/stdio.h /usr/include/ctype.h
parseaddr.o: /usr/include/setjmp.h conf.h ../include/useful.h
parseaddr.o: /usr/include/sys/types.h /usr/include/sys/syslog.h
parseaddr.o: /usr/include/sys/socket.h /usr/include/short_names.h
parseaddr.o: /usr/include/netinet/in.h /usr/include/short_names.h
parseaddr.o: /usr/include/sysexits.h
queue.o: queue.c sendmail.h /usr/include/stdio.h /usr/include/ctype.h
queue.o: /usr/include/setjmp.h conf.h ../include/useful.h
queue.o: /usr/include/sys/types.h /usr/include/sys/syslog.h
queue.o: /usr/include/sys/socket.h /usr/include/short_names.h
queue.o: /usr/include/netinet/in.h /usr/include/short_names.h
queue.o: /usr/include/sysexits.h /usr/include/sys/stat.h /usr/include/sys/dir.h
queue.o: /usr/include/signal.h /usr/include/errno.h
recipient.o: recipient.c /usr/include/pwd.h sendmail.h /usr/include/stdio.h
recipient.o: /usr/include/ctype.h /usr/include/setjmp.h conf.h
recipient.o: ../include/useful.h /usr/include/sys/types.h
recipient.o: /usr/include/sys/syslog.h /usr/include/sys/socket.h
recipient.o: /usr/include/short_names.h /usr/include/netinet/in.h
recipient.o: /usr/include/short_names.h /usr/include/sysexits.h
recipient.o: /usr/include/sys/stat.h
savemail.o: savemail.c /usr/include/pwd.h sendmail.h /usr/include/stdio.h
savemail.o: /usr/include/ctype.h /usr/include/setjmp.h conf.h
savemail.o: ../include/useful.h /usr/include/sys/types.h
savemail.o: /usr/include/sys/syslog.h /usr/include/sys/socket.h
savemail.o: /usr/include/short_names.h /usr/include/netinet/in.h
savemail.o: /usr/include/short_names.h /usr/include/sysexits.h
srvrsmtp.o: srvrsmtp.c sendmail.h /usr/include/stdio.h /usr/include/ctype.h
srvrsmtp.o: /usr/include/setjmp.h conf.h ../include/useful.h
srvrsmtp.o: /usr/include/sys/types.h /usr/include/sys/syslog.h
srvrsmtp.o: /usr/include/sys/socket.h /usr/include/short_names.h
srvrsmtp.o: /usr/include/netinet/in.h /usr/include/short_names.h
srvrsmtp.o: /usr/include/sysexits.h /usr/include/errno.h /usr/include/signal.h
stab.o: stab.c sendmail.h /usr/include/stdio.h /usr/include/ctype.h
stab.o: /usr/include/setjmp.h conf.h ../include/useful.h
stab.o: /usr/include/sys/types.h /usr/include/sys/syslog.h
stab.o: /usr/include/sys/socket.h /usr/include/short_names.h
stab.o: /usr/include/netinet/in.h /usr/include/short_names.h
stab.o: /usr/include/sysexits.h
stats.o: stats.c sendmail.h /usr/include/stdio.h /usr/include/ctype.h
stats.o: /usr/include/setjmp.h conf.h ../include/useful.h
stats.o: /usr/include/sys/types.h /usr/include/sys/syslog.h
stats.o: /usr/include/sys/socket.h /usr/include/short_names.h
stats.o: /usr/include/netinet/in.h /usr/include/short_names.h
stats.o: /usr/include/sysexits.h mailstats.h
sysexits.o: sysexits.c /usr/include/sysexits.h
trace.o: trace.c sendmail.h /usr/include/stdio.h /usr/include/ctype.h
trace.o: /usr/include/setjmp.h conf.h ../include/useful.h
trace.o: /usr/include/sys/types.h /usr/include/sys/syslog.h
trace.o: /usr/include/sys/socket.h /usr/include/short_names.h
trace.o: /usr/include/netinet/in.h /usr/include/short_names.h
trace.o: /usr/include/sysexits.h
usersmtp.o: usersmtp.c sendmail.h /usr/include/stdio.h /usr/include/ctype.h
usersmtp.o: /usr/include/setjmp.h conf.h ../include/useful.h
usersmtp.o: /usr/include/sys/types.h /usr/include/sys/syslog.h
usersmtp.o: /usr/include/sys/socket.h /usr/include/short_names.h
usersmtp.o: /usr/include/netinet/in.h /usr/include/short_names.h
usersmtp.o: /usr/include/sysexits.h /usr/include/sysexits.h
usersmtp.o: /usr/include/errno.h
util.o: util.c /usr/include/stdio.h /usr/include/sys/types.h
util.o: /usr/include/sys/stat.h /usr/include/sysexits.h /usr/include/errno.h
util.o: sendmail.h /usr/include/stdio.h /usr/include/ctype.h
util.o: /usr/include/setjmp.h conf.h ../include/useful.h
util.o: /usr/include/sys/syslog.h /usr/include/sys/socket.h
util.o: /usr/include/short_names.h /usr/include/netinet/in.h
util.o: /usr/include/short_names.h /usr/include/sysexits.h
Version.o: Version.c

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
