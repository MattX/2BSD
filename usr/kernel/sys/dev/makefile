OBJS=	kl.o tty.o ttynew.o ttyold.o bk.o lp.o dz.o bio.o hp.o ht.o rl.o \
	mem.o partab.o sys.o dh.o dhdm.o \
	mux.o dvhp.o hs.o rc.o xp.o dsort.o dkleave.o 

CFILES=	kl.c tty.c ttynew.c ttyold.c bk.c lp.c dz.c bio.c hp.c ht.c rl.c \
	mem.c partab.c sys.c dh.c dhdm.c \
	mux.c dvhp.c hs.c rc.c xp.c dsort.c dkleave.c 

DEFS= -DKERNEL 

INCLUDES= ../../include

CFLAGS = -O ${DEFS} -I${INCLUDES}

.c.o:
	cc -c ${CFLAGS} $*.c 

LIB2: ${OBJS}
	ar rv LIB2 $?

remake:	${OBJS}
	ar rcv LIB2 ${OBJS}

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

bio.o: ../../include/sys/buf.h
bio.o: ../../include/sys/conf.h
bio.o: ../../include/sys/dir.h
bio.o: ../../include/sys/param.h
bio.o: ../../include/sys/proc.h
bio.o: ../../include/sys/seg.h
bio.o: ../../include/sys/systm.h
bio.o: ../../include/sys/user.h
bio.o: ../../include/sys/vm.h
bk.o: ../../include/sys/buf.h
bk.o: ../../include/sys/conf.h
bk.o: ../../include/sys/dir.h
bk.o: ../../include/sys/file.h
bk.o: ../../include/sys/inode.h
bk.o: ../../include/sys/mx.h
bk.o: ../../include/sys/param.h
bk.o: ../../include/sys/proc.h
bk.o: ../../include/sys/systm.h
bk.o: ../../include/sys/tty.h
bk.o: ../../include/sys/user.h
dh.o: ../../include/sys/conf.h
dh.o: ../../include/sys/dir.h
dh.o: ../../include/sys/param.h
dh.o: ../../include/sys/tty.h
dh.o: ../../include/sys/user.h
dhdm.o: ../../include/sys/conf.h
dhdm.o: ../../include/sys/file.h
dhdm.o: ../../include/sys/param.h
dhdm.o: ../../include/sys/tty.h
dkleave.o: ../../include/sys/buf.h
dkleave.o: ../../include/sys/param.h
dsort.o: ../../include/sys/buf.h
dsort.o: ../../include/sys/param.h
dsort.o: ../../include/sys/systm.h
dvhp.o: ../../include/sys/buf.h
dvhp.o: ../../include/sys/conf.h
dvhp.o: ../../include/sys/dir.h
dvhp.o: ../../include/sys/dk.h
dvhp.o: ../../include/sys/param.h
dvhp.o: ../../include/sys/systm.h
dvhp.o: ../../include/sys/user.h
dz.o: ../../include/sys/conf.h
dz.o: ../../include/sys/dir.h
dz.o: ../../include/sys/file.h
dz.o: ../../include/sys/param.h
dz.o: ../../include/sys/tty.h
dz.o: ../../include/sys/user.h
hp.o: ../../include/sys/buf.h
hp.o: ../../include/sys/conf.h
hp.o: ../../include/sys/dir.h
hp.o: ../../include/sys/dk.h
hp.o: ../../include/sys/param.h
hp.o: ../../include/sys/seg.h
hp.o: ../../include/sys/systm.h
hp.o: ../../include/sys/user.h
hs.o: ../../include/sys/buf.h
hs.o: ../../include/sys/conf.h
hs.o: ../../include/sys/dir.h
hs.o: ../../include/sys/dk.h
hs.o: ../../include/sys/param.h
hs.o: ../../include/sys/seg.h
hs.o: ../../include/sys/systm.h
hs.o: ../../include/sys/user.h
ht.o: ../../include/sys/buf.h
ht.o: ../../include/sys/conf.h
ht.o: ../../include/sys/dir.h
ht.o: ../../include/sys/file.h
ht.o: ../../include/sys/param.h
ht.o: ../../include/sys/systm.h
ht.o: ../../include/sys/user.h
kl.o: ../../include/sys/conf.h
kl.o: ../../include/sys/dir.h
kl.o: ../../include/sys/param.h
kl.o: ../../include/sys/systm.h
kl.o: ../../include/sys/tty.h
kl.o: ../../include/sys/user.h
lp.o: ../../include/sys/dir.h
lp.o: ../../include/sys/param.h
lp.o: ../../include/sys/systm.h
lp.o: ../../include/sys/tty.h
lp.o: ../../include/sys/user.h
mem.o: ../../include/sys/conf.h
mem.o: ../../include/sys/dir.h
mem.o: ../../include/sys/param.h
mem.o: ../../include/sys/seg.h
mem.o: ../../include/sys/user.h
mux.o: ../../include/sys/conf.h
mux.o: ../../include/sys/dir.h
mux.o: ../../include/sys/param.h
mux.o: ../../include/sys/tty.h
mux.o: ../../include/sys/user.h
partab.o: ../../include/sys/param.h
rc.o: ../../include/sys/conf.h
rc.o: ../../include/sys/dir.h
rc.o: ../../include/sys/param.h
rc.o: ../../include/sys/seg.h
rc.o: ../../include/sys/time.h
rc.o: ../../include/sys/user.h
rl.o: ../../include/sys/buf.h
rl.o: ../../include/sys/dir.h
rl.o: ../../include/sys/param.h
rl.o: ../../include/sys/systm.h
rl.o: ../../include/sys/user.h
sys.o: ../../include/sys/conf.h
sys.o: ../../include/sys/dir.h
sys.o: ../../include/sys/param.h
sys.o: ../../include/sys/proc.h
sys.o: ../../include/sys/tty.h
sys.o: ../../include/sys/user.h
tty.o: ../../include/sys/buf.h
tty.o: ../../include/sys/conf.h
tty.o: ../../include/sys/dir.h
tty.o: ../../include/sys/file.h
tty.o: ../../include/sys/inode.h
tty.o: ../../include/sys/mx.h
tty.o: ../../include/sys/param.h
tty.o: ../../include/sys/proc.h
tty.o: ../../include/sys/reg.h
tty.o: ../../include/sys/systm.h
tty.o: ../../include/sys/tty.h
tty.o: ../../include/sys/user.h
ttynew.o: ../../include/sys/buf.h
ttynew.o: ../../include/sys/conf.h
ttynew.o: ../../include/sys/dir.h
ttynew.o: ../../include/sys/file.h
ttynew.o: ../../include/sys/inode.h
ttynew.o: ../../include/sys/mx.h
ttynew.o: ../../include/sys/param.h
ttynew.o: ../../include/sys/proc.h
ttynew.o: ../../include/sys/reg.h
ttynew.o: ../../include/sys/systm.h
ttynew.o: ../../include/sys/tty.h
ttynew.o: ../../include/sys/user.h
ttyold.o: ../../include/sys/buf.h
ttyold.o: ../../include/sys/conf.h
ttyold.o: ../../include/sys/dir.h
ttyold.o: ../../include/sys/file.h
ttyold.o: ../../include/sys/inode.h
ttyold.o: ../../include/sys/mx.h
ttyold.o: ../../include/sys/param.h
ttyold.o: ../../include/sys/proc.h
ttyold.o: ../../include/sys/reg.h
ttyold.o: ../../include/sys/systm.h
ttyold.o: ../../include/sys/tty.h
ttyold.o: ../../include/sys/user.h
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
