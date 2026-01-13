#
# 	#(@)Makefile	2.1 (2.11BSD) 2025/11/25
#
CFLAGS=	-O
LDFLAGS= -i

# Programs that live in subdirectories, and have makefiles of their own.
SUBDIR= afio cxref ddd less mkovmake mp mtools welcome qterm zmodem

# C source that lives in the current directory
SRCS= decompr16.c trace.c

# C programs that live in the current directory and do not need
# explicit make lines.
STD= decompr16 trace 

# Manpage directory
MANDIR= /usr/local/man

all:	${SUBDIR} ${STD}

${SUBDIR}: FRC
	cd $@; make ${MFLAGS}

FRC:

${STD}:
	cc ${CFLAGS} ${LDFLAGS} -o $@ $@.c

install: ${STD}
	-mkdir ${MANDIR}
	-chmod a+r ${MANDIR}
	-for i in 1 2 3 4 5 6 7 8; do \
		(mkdir ${MANDIR}/cat$$i; chmod a+r ${MANDIR}/cat$$i); done
	-for i in ${SUBDIR}; do \
		(cd $$i; make ${MFLAGS} install); done
	-install -s -o root -g staff  decompr16 /usr/local/decompr16
	-install -s -o root -g kmem -m 2755 trace /usr/local/trace
	-sh /usr/sbin/makewhatis ${MANDIR}

clean:
	rm -f a.out core *.s *.o ${STD}
	for i in ${SUBDIR}; do (cd $$i; make ${MFLAGS} clean); done

depend:
	-chmod +w Makefile
	mkdep -p ${CFLAGS} ${SRCS}

# DO NOT DELETE THIS LINE -- make depend uses it

# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
