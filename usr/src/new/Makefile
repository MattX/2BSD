#
DESTDIR=

DEFS=	
CFLAGS=	-O ${DEFS}
SEPFLAG=-i

#
# Default programs that are installed
#
STDINST=jove l11 m11 patch rcs

#
# Special purpose software that requires custom installation,
# or may not be needed by all sites
#
OPTDIR=	ansi bm checkobj crash kermit la oldassn news nmcoll pathalias rn \
	shar shortc

SUBDIR=	ansi bm checkobj crash jove kermit l11 la m11 oldassn news nmcoll \
	patch pathalias rcs rn shar shortc

all:	${STDINST}

${STDINST}: FRC
	cd $@; make ${MFLAGS} SEPFLAG=${SEPFLAG}

FRC:

install:
	-for i in ${STDINST}; do \
		(cd $$i; make ${MFLAGS} DESTDIR=${DESTDIR} install); done

clean:
	for i in ${SUBDIR}; do (cd $$i; make ${MFLAGS} clean); done

depend:
tags:
lint:
