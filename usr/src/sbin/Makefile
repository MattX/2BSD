#
# Public domain - 1996/10/26 - sms
#
#	@(#)Makefile	1.2 (2.11BSD) 1997/1/10
#
DESTDIR=
CFLAGS=	-O
SEPFLAG= -i

SUBDIR= XNSrouted badsect clri dcheck dmesg dump dumpfs fsck icheck \
	ifconfig init mkfs mknod mkpasswd mount ncheck newfs \
	quotacheck reboot restor route routed savecore \
	shutdown slattach swapon testnet tunefs umount

all:	${SUBDIR}

${SUBDIR}: FRC
	cd $@; make ${MFLAGS} SEPFLAG=${SEPFLAG} all

FRC:

install: FRC
	-for i in ${SUBDIR}; do \
		(cd $$i; make ${MFLAGS} DESTDIR=${DESTDIR} install); done

clean:
	for i in ${SUBDIR}; do (cd $$i; make ${MFLAGS} clean); done
