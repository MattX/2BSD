DEFS= -DKERNEL

INCLUDES= ../../include

CFLAGS=	-O ${DEFS} -I${INCLUDES}

CFILES=	c.c /dev/null

OFILE=	unix

SYS=	../sys/*.c

DEV=	../dev/*.c

HEAD=	${INCLUDES}/sys/*.h ${INCLUDES}/whoami.h

sys:	${OFILE}

${OFILE}: l.o mch.o c.o ../sys/LIB1 ../dev/LIB2 
	ld -o ${OFILE} -X -i l.o mch.o c.o ../sys/LIB1 ../dev/LIB2 

../sys/LIB1:	${SYS} ${HEAD}
	cd ../sys; make INCLUDES=${INCLUDES}

../dev/LIB2:	${DEV} ${HEAD}
	cd ../dev; make INCLUDES=${INCLUDES}

egrep:
	nm ${OFILE} | egrep ' _end| _etext| _edata| _inode| _abuff'

mch.o:	mch0.s mch.s
	as -o mch.o mch0.s mch.s


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

c.o: ../../include/sys/acct.h
c.o: ../../include/sys/buf.h
c.o: ../../include/sys/conf.h
c.o: ../../include/sys/dir.h
c.o: ../../include/sys/dk.h
c.o: ../../include/sys/file.h
c.o: ../../include/sys/inode.h
c.o: ../../include/sys/param.h
c.o: ../../include/sys/proc.h
c.o: ../../include/sys/systm.h
c.o: ../../include/sys/text.h
c.o: ../../include/sys/tty.h
c.o: ../../include/sys/user.h
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
