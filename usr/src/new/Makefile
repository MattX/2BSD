#       @(#)Makefile    2.0   (2.11BSD)  2025/12/10

MANDIR= /usr/new/man

CFLAGS=	-O

#
# Default programs that are installed
#
STDINST=ansi bm jove l11 la m11 pathalias rcs vmsbackup

#
# Special purpose software that requires custom installation,
# or may not be needed by all sites
#
OPTDIR=	kermit5.188 pathalias rcs rn notes nntp

SUBDIR= ansi bm jove kermit5.188 l11 la m11 nntp \
	notes pathalias rcs rn vmsbackup

all:	${STDINST}

${STDINST}: FRC
	cd $@; make ${MFLAGS}

FRC:

install:
	-mkdir ${MANDIR}
	-chmod a+r ${MANDIR}
	-for i in 1 2 3 4 5 6 7 8; do \
		(mkdir ${MANDIR}/cat$$i; chmod a+r ${MANDIR}/cat$$i); done
	-for i in ${STDINST}; do \
		(cd $$i; make ${MFLAGS} install); done
	-/usr/sbin/makewhatis ${MANDIR}

clean:
	for i in ${SUBDIR}; do (cd $$i; make -k ${MFLAGS} clean); done

depend:
tags:
lint:
