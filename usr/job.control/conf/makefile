#DEFS=-DKERNEL -DMENLO_JCL
DEFS= -DKERNEL

INCLUDES= ../include

CFLAGS=	-O ${DEFS} -I${INCLUDES}
CC = ovcc
AS = ovas

CFILES=	c.c /dev/null

OFILE=	unix

SYS=	../sys/*.c

DEV=	../dev/*.c

HEAD=	${INCLUDES}/sys/*.h ${INCLUDES}/whoami.h

sys:	${OFILE} egrep

install: sys copy unix1

copy:
	-cp /ounix /vounix
	cp /unix /ounix
	cp ${OFILE} /unix
	@date >> systrace
	@echo 'Message for systrace (terminate with ^D):'
	@cat >> systrace
	@echo "" >> systrace

unix1:
	cp /unix /unix1
	adb -w /unix1 < :unix1

test:	sys
	cp ${OFILE} /xunix

${OFILE}: l.o m40.o c.o ../sys/LIB1 ../dev/LIB2 
	load

../sys/LIB1:	${SYS} ${HEAD}
	cd ../sys; make INCLUDES=${INCLUDES} DEFS=${DEFS}

../dev/LIB2:	${DEV} ${HEAD}
	cd ../dev; make INCLUDES=${INCLUDES} DEFS=${DEFS}

egrep:
	nm ${OFILE} | egrep ' _end| _etext| _edata| _inode| _abuff'

all:
	cd ../sys; ${CC} ${CFLAGS} *.c; mklib; rm *.o
	cd ../dev; ${CC} ${CFLAGS} *.c; mklib; rm *.o

m40.o:	mch0.s m40.s
	${AS} -o m40.o mch0.s m40.s

allsystems:
	mkconf <hphtconf
	make unix
	mv unix hphtunix
	mkconf <hptmconf
	make unix
	mv unix hptmunix
	mkconf <rptmconf
	make unix
	mv unix rptmunix
	mkconf <rphtconf
	make unix
	mv unix rphtunix
	mkconf <rktmconf
	make unix
	mv unix rktmunix
	mkconf <rkhtconf
	make unix
	mv unix rkhtunix

${INCLUDES}/sys/param.h: ${INCLUDES}/whoami.h
	touch ${INCLUDES}/sys/param.h

depend:
	grep '^#include' ${CFILES} | grep -v "<"  | \
		sed 's/:[^"]*"\([^"]*\)".*/: \1/' | \
		sed 's/\.c/.o/' >makedep
	grep '^#include' ${CFILES} | grep "<" | \
		sed 's|:[^<]*<\([^>]*\)>.*|: ${INCLUDES}/\1|' | \
		sed 's/\.c/.o/' >>makedep
	sort makedep -o makedep
	echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp makefile makefile.bak
	ed - makefile < eddep
	rm eddep makedep
	echo '# DEPENDENCIES MUST END AT END OF FILE' >> makefile
	echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> makefile
	echo '# see make depend above' >> makefile

# DO NOT DELETE THIS LINE

c.o: ../include/sys/acct.h
c.o: ../include/sys/buf.h
c.o: ../include/sys/conf.h
c.o: ../include/sys/dir.h
c.o: ../include/sys/dk.h
c.o: ../include/sys/file.h
c.o: ../include/sys/inode.h
c.o: ../include/sys/param.h
c.o: ../include/sys/proc.h
c.o: ../include/sys/systm.h
c.o: ../include/sys/text.h
c.o: ../include/sys/tty.h
c.o: ../include/sys/user.h
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
