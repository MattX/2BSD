#
#	Makefile for sendmail base directory
#
#		@(#)Makefile	4.9.3	(2.11BSD) 2025/11/25
#

ALL=	src/sendmail
DIRS=	/usr/spool/mqueue
#
ALLDIRS=include adm src aux md lib doc adb test
SENDMAIL=/usr/sbin/sendmail
SYSLOG=	/usr/spool/mqueue/syslog
HELPFILE=/usr/share/misc/sendmail.hf
STATUSFILE=/var/log/sendmail.st
STRINGFILE=/usr/share/misc/sendmail.sr
FROZENFILE=/etc/sendmail.fc

LN=	ln -s

all:
	cd lib; make ${MFLAGS}
	cd src; m4 Makefile.m4 > Makefile; make ${MFLAGS} all
	-cd aux; m4 Makefile.m4 > Makefile; make ${MFLAGS}

install: $(ALL) $(DIRS)
	install -c -s -o root -m 4755	src/sendmail	$(SENDMAIL)
	install -c -o root -m 644	src/sendmail.sr	$(STRINGFILE)
	install -c -o root -m 644	lib/sendmail.hf	$(HELPFILE)
	install -c -o root -m 666	/dev/null	$(STATUSFILE)
	install -c -o root -m 644	/dev/null	$(FROZENFILE)
	rm -f /usr/bin/newaliases
	$(LN) ../sbin/sendmail /usr/bin/newaliases
	rm -f /usr/bin/mailq
	$(LN) ../sbin/sendmail  /usr/bin/mailq

src/sendmail:
	cd src; make ${MFLAGS} sendmail

aux/syslog:
	-cd aux; make ${MFLAGS} syslog

/usr/spool/mqueue:
	-mkdir			/usr/spool/mqueue
	-chmod	777		/usr/spool/mqueue

clean:
	-cd src; make ${MFLAGS} clean
	-cd cf;  make ${MFLAGS} clean
	-cd aux; make ${MFLAGS} clean
	-cd lib; make ${MFLAGS} clean
	-cd doc; make ${MFLAGS} clean
