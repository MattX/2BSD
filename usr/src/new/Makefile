#
DESTDIR=
MANDIR= /usr/new/man

DEFS=	
CFLAGS=	-O ${DEFS}
SEPFLAG=-i

#
# Default programs that are installed
#
STDINST=ansi bm checkobj jove l11 la m11 nmcoll oldassn \
	patch pathalias rcs shar shortc tcsh vmsbackup

#
# Special purpose software that requires custom installation,
# or may not be needed by all sites
#
OPTDIR=	crash oldassn kermit5.188 pathalias rcs rn notes nntp

SUBDIR= ansi bm checkobj crash jove kermit5.188 l11 la m11 nmcoll nntp \
	notes oldassn patch pathalias rcs rn shar shortc tcsh \
	vmsbackup

all:	${STDINST}

${STDINST}: FRC
	cd $@; make ${MFLAGS} SEPFLAG=${SEPFLAG}

FRC:

install:
	-mkdir ${MANDIR}
	-chmod a+r ${MANDIR}
	-for i in 1 2 3 4 5 6 7 8; do \
		(mkdir ${MANDIR}/cat$$i; chmod a+r ${MANDIR}/cat$$i); done
	-for i in ${STDINST}; do \
		(cd $$i; make ${MFLAGS} DESTDIR=${DESTDIR} install); done
	-/usr/sbin/makewhatis ${MANDIR}

clean:
	for i in ${SUBDIR}; do (cd $$i; make -k ${MFLAGS} clean); done

depend:
tags:
lint:
