#
# Public domain - 1996/10/21 - sms
#
#	@(#)Makefile	1.2 (2.11BSD) 2025/11/25
#
CFLAGS=	-O

SUBDIR=	adb font fontinfo learn me misc ms tmac termcap zoneinfo

all:	${SUBDIR}

${SUBDIR}: FRC
	cd $@; make ${MFLAGS}

FRC:

install: FRC
	-for i in ${SUBDIR}; do \
		(cd $$i; make ${MFLAGS} install); done

clean:
	for i in ${SUBDIR}; do (cd $$i; make ${MFLAGS} clean); done
