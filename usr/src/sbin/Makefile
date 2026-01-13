#
# Public domain - 1996/10/26 - sms
#
#	@(#)Makefile	1.4 (2.11BSD) 2025/11/25
#
CFLAGS=	-O

SUBDIR= badsect clri dcheck dmesg dump fsck gt307 icheck \
	ifconfig init mkfs mknod mkpasswd mount ncheck newfs \
	quotacheck reboot restor route routed savecore \
	shutdown slattach testnet umount

all:	${SUBDIR}

${SUBDIR}: FRC
	cd $@; make ${MFLAGS} all

FRC:

install: FRC
	-for i in ${SUBDIR}; do \
		(cd $$i; make ${MFLAGS} install); done

clean:
	for i in ${SUBDIR}; do (cd $$i; make ${MFLAGS} clean); done
