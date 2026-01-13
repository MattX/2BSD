#
# Public domain - 1997/1/10 - sms
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/25
#
# This file is not intended to be run but rather to serve as a guide when
# (manually) creating a distribution image.  To guard against corrupting a
# live/running system DESTDIR is defined here to be something which does 
# *NOT* exist.

DESTDIR=/foobar
OWN=root
GRP=wheel

SUBDIR= root uucp
FILES= aliases crontab disktab dtab fstab ftpusers gettytab group hosts \
	hosts.allow hosts.deny inetd.conf inetd.conf.nowrappers \
	motd netstart networks ntp.conf passwd phones printcap protocols \
	rc rc.local remote services shells syslog.conf ttys

# NOP targets
all install depend lint clean tags:

# this will populate /foobar with prototype /dev and /etc files
distribution:
	-@mkdir -p ${DESTDIR}/dev
	@chmod 755 ${DESTDIR}/dev
	-@mkdir -p ${DESTDIR}/etc
	@chmod 755 ${DESTDIR}/etc
	@chown ${OWN}.${GRP} ${DESTDIR}/dev ${DESTDIR}/etc
	-@mkdir -p ${DESTDIR}/usr/spool/mqueue
	@chmod 755 ${DESTDIR}/usr/spool/mqueue
	@chown ${OWN}.${GRP} ${DESTDIR}/usr/spool/mqueue

	install -c -o ${OWN} -g ${GRP} -m 644 ${FILES} ${DESTDIR}/etc
	install -c -o root -g wheel -m  600 master.passwd ${DESTDIR}/etc
	install -c -o ${OWN} -g ${GRP} -m 600 hosts.equiv ${DESTDIR}/etc
	install -c -o ${OWN} -g ${GRP} -m 555 MAKEDEV.local MAKEDEV \
		${DESTDIR}/dev
	install -c -o ${OWN} -g operator -m 664 /dev/null \
		${DESTDIR}/etc/dumpdates
	install -c -o ${OWN} -g operator -m 664 /dev/null \
		${DESTDIR}/etc/sendmail.cw
	install -c -o ${OWN} -g operator -m 664 /dev/null \
		${DESTDIR}/etc/sendmail.fc
	-@mkdir -p ${DESTDIR}/usr/adm ${DESTDIR}/var/run ${DESTDIR}/var/db
	@chmod 655 ${DESTDIR}/usr/adm ${DESTDIR}/var/run ${DESTDIR}/var/db
	@chown ${OWN}.${GRP} ${DESTDIR}/usr/adm ${DESTDIR}/var/run \
		${DESTDIR}/var/db
	install -c -o ${OWN} -g ${GRP} -m 644 /dev/null \
	    ${DESTDIR}/usr/adm/messages
	install -c -o ${OWN} -g ${GRP} -m 644 /dev/null \
	    ${DESTDIR}/usr/adm/daemonlog
	install -c -o ${OWN} -g ${GRP} -m 644 /dev/null \
	    ${DESTDIR}/usr/adm/debuglog
	install -c -o ${OWN} -g operator -m 664 /dev/null \
	    ${DESTDIR}/usr/adm/shutdownlog
	install -c -o ${OWN} -g ${GRP} -m 644 /dev/null \
	    ${DESTDIR}/usr/spool/mqueue/syslog
	install -c -o ${OWN} -g ${GRP} -m 644 /dev/null \
	    ${DESTDIR}/usr/adm/lpd-errs
	install -c -o ${OWN} -g ${GRP} -m 644 /dev/null \
	    ${DESTDIR}/var/run/utmp
	install -c -o nobody -g ${GRP} -m 644 /dev/null \
	    ${DESTDIR}/var/db/find.codes
	install -c -o ${OWN} -g ${GRP} -m 644 /dev/null \
	    ${DESTDIR}/usr/adm/lastlog
	install -c -o ${OWN} -g ${GRP} -m 644 /dev/null \
	    ${DESTDIR}/usr/adm/wtmp
	install -c -o ${OWN} -g ${GRP} -m 644 /dev/null \
	    ${DESTDIR}/usr/adm/acct
	install -c -o ${OWN} -g ${GRP} -m 644 /dev/null \
	    ${DESTDIR}/usr/adm/usracct
	install -c -o ${OWN} -g ${GRP} -m 644 /dev/null \
	    ${DESTDIR}/usr/adm/savacct
	install -c -o uucp -g daemon -m 600 /dev/null \
	    ${DESTDIR}/usr/adm/aculog
	-for i in ${SUBDIR}; do \
		(cd $$i; make DESTDIR=${DESTDIR} distribution); done
