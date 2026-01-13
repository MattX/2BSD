#
# Public domain - 1996/10/24 - sms
#
#	@(#)Makefile	1.5 (2.11BSD) 2025/11/25

SUBDIR=	ac accton arp bad144 catman chown chroot cron dev_mkdb \
	edquota inetd \
	lpr makewhatis mkhosts mklost+found mkproto named nc ntp pstat quot \
	quotaon repquota rmt rwhod rxformat sa sendmail sysgen syslogd timed \
	traceroute trpt trsp update vipw

all:	${SUBDIR}

${SUBDIR}: FRC
	cd $@; make ${MFLAGS} all

FRC:

install: FRC
	-for i in ${SUBDIR}; do \
		(cd $$i; make ${MFLAGS} install); done

clean:
	for i in ${SUBDIR}; do (cd $$i; make ${MFLAGS} clean); done
