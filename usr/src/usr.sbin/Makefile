#
# Public domain - 1996/10/24 - sms
#
#	@(#)Makefile	1.0 (2.11BSD) 1996/10/24
#
DESTDIR=
CFLAGS=	-O
SEPFLAG= -i

SUBDIR=	lpr makewhatis sendmail

all:	${SUBDIR}

${SUBDIR}: FRC
	cd $@; make ${MFLAGS} SEPFLAG=${SEPFLAG} all

FRC:

install: FRC
	-for i in ${SUBDIR}; do \
		(cd $$i; make ${MFLAGS} DESTDIR=${DESTDIR} install); done

clean:
	for i in ${SUBDIR}; do (cd $$i; make ${MFLAGS} clean); done
