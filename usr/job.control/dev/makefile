OBJS=	kl.o tty.o ttyold.o ttynew.o dz.o bio.o rm.o tm.o rk.o\
	mem.o partab.o sys.o dsort.o dkleave.o  cary.o

CFILES=	kl.c tty.c ttyold.c ttynew.c dz.c bio.c rm.c tm.c rk.c \
	mem.c partab.c sys.c dsort.c dkleave.c  cary.c

#DEFS= -DKERNEL -DMENLO_JCL
DEFS= -DKERNEL

INCLUDES= ../include

CFLAGS = -O ${DEFS} -I${INCLUDES}
CC = cc

.c.o:
	${CC} -c ${CFLAGS} $*.c 

lib:	LIB2
	ranlib LIB2

remake:	${OBJS}
	ar rcv LIB2 ${OBJS}

LIB2: ${OBJS}
	ar rv LIB2 $?
	ranlib LIB2

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

bio.o: ../include/sys/buf.h
bio.o: ../include/sys/conf.h
bio.o: ../include/sys/dir.h
bio.o: ../include/sys/param.h
bio.o: ../include/sys/proc.h
bio.o: ../include/sys/seg.h
bio.o: ../include/sys/systm.h
bio.o: ../include/sys/user.h
bio.o: ../include/sys/vm.h
cary.o: ../include/sys/conf.h
cary.o: ../include/sys/dir.h
cary.o: ../include/sys/param.h
cary.o: ../include/sys/tty.h
cary.o: ../include/sys/user.h
dkleave.o: ../include/sys/buf.h
dkleave.o: ../include/sys/param.h
dsort.o: ../include/sys/buf.h
dsort.o: ../include/sys/param.h
dsort.o: ../include/sys/systm.h
dz.o: ../include/sys/conf.h
dz.o: ../include/sys/dir.h
dz.o: ../include/sys/param.h
dz.o: ../include/sys/tty.h
dz.o: ../include/sys/user.h
kl.o: ../include/sys/conf.h
kl.o: ../include/sys/dir.h
kl.o: ../include/sys/param.h
kl.o: ../include/sys/systm.h
kl.o: ../include/sys/tty.h
kl.o: ../include/sys/user.h
mem.o: ../include/sys/conf.h
mem.o: ../include/sys/dir.h
mem.o: ../include/sys/param.h
mem.o: ../include/sys/seg.h
mem.o: ../include/sys/user.h
partab.o: ../include/sys/param.h
rk.o: ../include/sys/buf.h
rk.o: ../include/sys/conf.h
rk.o: ../include/sys/dir.h
rk.o: ../include/sys/param.h
rk.o: ../include/sys/systm.h
rk.o: ../include/sys/user.h
rm.o: ../include/sys/buf.h
rm.o: ../include/sys/conf.h
rm.o: ../include/sys/dir.h
rm.o: ../include/sys/param.h
rm.o: ../include/sys/systm.h
rm.o: ../include/sys/user.h
sys.o: ../include/sys/conf.h
sys.o: ../include/sys/dir.h
sys.o: ../include/sys/param.h
sys.o: ../include/sys/proc.h
sys.o: ../include/sys/tty.h
sys.o: ../include/sys/user.h
tm.o: ../include/sys/buf.h
tm.o: ../include/sys/conf.h
tm.o: ../include/sys/dir.h
tm.o: ../include/sys/file.h
tm.o: ../include/sys/param.h
tm.o: ../include/sys/user.h
tty.o: ../include/sys/buf.h
tty.o: ../include/sys/conf.h
tty.o: ../include/sys/dir.h
tty.o: ../include/sys/file.h
tty.o: ../include/sys/inode.h
tty.o: ../include/sys/mx.h
tty.o: ../include/sys/param.h
tty.o: ../include/sys/proc.h
tty.o: ../include/sys/reg.h
tty.o: ../include/sys/systm.h
tty.o: ../include/sys/tty.h
tty.o: ../include/sys/user.h
ttynew.o: ../include/sys/buf.h
ttynew.o: ../include/sys/conf.h
ttynew.o: ../include/sys/dir.h
ttynew.o: ../include/sys/file.h
ttynew.o: ../include/sys/inode.h
ttynew.o: ../include/sys/mx.h
ttynew.o: ../include/sys/param.h
ttynew.o: ../include/sys/proc.h
ttynew.o: ../include/sys/reg.h
ttynew.o: ../include/sys/systm.h
ttynew.o: ../include/sys/tty.h
ttynew.o: ../include/sys/user.h
ttyold.o: ../include/sys/buf.h
ttyold.o: ../include/sys/conf.h
ttyold.o: ../include/sys/dir.h
ttyold.o: ../include/sys/file.h
ttyold.o: ../include/sys/inode.h
ttyold.o: ../include/sys/mx.h
ttyold.o: ../include/sys/param.h
ttyold.o: ../include/sys/proc.h
ttyold.o: ../include/sys/reg.h
ttyold.o: ../include/sys/systm.h
ttyold.o: ../include/sys/tty.h
ttyold.o: ../include/sys/user.h
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
