#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.8 (Berkeley) 9/7/85
#
DESTDIR=
CFLAGS=	-O
LFLAGS=	-i

# The program itself
#
PROG=	fsck

# Sources
#
SRCS=	dir.c inode.c main.c pass1.c pass1b.c pass2.c \
	pass3.c pass4.c pass5.c setup.c utilities.c

# Objects
#
OBJS=	${LOCOBJS}
LOCOBJS=dir.o inode.o main.o pass1.o pass1b.o pass2.o \
	pass3.o pass4.o pass5.o setup.o utilities.o

# Header files
#
HDRS=	fsck.h

${PROG}: ${OBJS} ${HDRS}
	cc ${LFLAGS} -o ${PROG} ${OBJS}

${LOCOBJS}:
	cc ${CFLAGS} -c $*.c

install: ${PROG}
	install -s ${PROG} ${DESTDIR}/etc/${PROG}

clean:
	rm -f a.out core ${OBJS} ${PROG}

lint:
	lint ${INCPATH} ${SRCS}

depend:
	cc -M ${INCPATH} ${SRCS} | \
	awk ' { if ($$1 != prev) { print rec; rec = $$0; prev = $$1; } \
		else { if (length(rec $$2) > 78) { print rec; rec = $$0; } \
		       else rec = rec " " $$2 } } \
	      END { print rec } ' > makedep
	echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp Makefile Makefile.bak
	ed - Makefile < eddep
	rm eddep makedep
	echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
	echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
	echo '# see make depend above' >> Makefile

# DO NOT DELETE THIS LINE -- make depend uses it
