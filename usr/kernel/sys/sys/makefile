#
OBJS=	main.o trap.o sig.o iget.o prf.o slp.o subr.o rdwri.o clock.o \
	fio.o malloc.o alloc.o machdep.o nami.o pipe.o prim.o fakemx.o \
	sysent.o sys3.o sys1.o sys4.o sys2.o syslocal.o acct.o ioctl.o\
	text.o ureg.o

CFILES=	main.c trap.c sig.c iget.c prf.c slp.c subr.c rdwri.c clock.c \
	fio.c malloc.c alloc.c machdep.c nami.c pipe.c prim.c fakemx.c \
	sysent.c sys3.c sys1.c sys4.c sys2.c syslocal.c acct.c ioctl.c\
	text.c ureg.c

INCLUDES= ../../include

DEFS=-DKERNEL

CFLAGS=	 -O -I${INCLUDES} ${DEFS}

.c.o:
	cc -c ${CFLAGS} $*.c 

LIB1: ${OBJS}
	ar rv LIB1 $?

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

acct.o: ../../include/sys/acct.h
acct.o: ../../include/sys/dir.h
acct.o: ../../include/sys/inode.h
acct.o: ../../include/sys/param.h
acct.o: ../../include/sys/proc.h
acct.o: ../../include/sys/seg.h
acct.o: ../../include/sys/systm.h
acct.o: ../../include/sys/user.h
alloc.o: ../../include/sys/buf.h
alloc.o: ../../include/sys/conf.h
alloc.o: ../../include/sys/dir.h
alloc.o: ../../include/sys/fblk.h
alloc.o: ../../include/sys/filsys.h
alloc.o: ../../include/sys/ino.h
alloc.o: ../../include/sys/inode.h
alloc.o: ../../include/sys/mount.h
alloc.o: ../../include/sys/param.h
alloc.o: ../../include/sys/quota.h
alloc.o: ../../include/sys/systm.h
alloc.o: ../../include/sys/user.h
clock.o: ../../include/sys/callo.h
clock.o: ../../include/sys/dir.h
clock.o: ../../include/sys/dk.h
clock.o: ../../include/sys/param.h
clock.o: ../../include/sys/proc.h
clock.o: ../../include/sys/reg.h
clock.o: ../../include/sys/seg.h
clock.o: ../../include/sys/systm.h
clock.o: ../../include/sys/text.h
clock.o: ../../include/sys/user.h
clock.o: ../../include/sys/vm.h
fakemx.o: ../../include/sys/dir.h
fakemx.o: ../../include/sys/inode.h
fakemx.o: ../../include/sys/mx.h
fakemx.o: ../../include/sys/param.h
fakemx.o: ../../include/sys/tty.h
fakemx.o: ../../include/sys/user.h
fio.o: ../../include/sys/acct.h
fio.o: ../../include/sys/conf.h
fio.o: ../../include/sys/dir.h
fio.o: ../../include/sys/file.h
fio.o: ../../include/sys/filsys.h
fio.o: ../../include/sys/inode.h
fio.o: ../../include/sys/param.h
fio.o: ../../include/sys/reg.h
fio.o: ../../include/sys/systm.h
fio.o: ../../include/sys/user.h
iget.o: ../../include/sys/buf.h
iget.o: ../../include/sys/conf.h
iget.o: ../../include/sys/dir.h
iget.o: ../../include/sys/filsys.h
iget.o: ../../include/sys/ino.h
iget.o: ../../include/sys/inode.h
iget.o: ../../include/sys/mount.h
iget.o: ../../include/sys/param.h
iget.o: ../../include/sys/quota.h
iget.o: ../../include/sys/systm.h
iget.o: ../../include/sys/user.h
ioctl.o: ../../include/sys/conf.h
ioctl.o: ../../include/sys/dir.h
ioctl.o: ../../include/sys/file.h
ioctl.o: ../../include/sys/inode.h
ioctl.o: ../../include/sys/mx.h
ioctl.o: ../../include/sys/param.h
ioctl.o: ../../include/sys/proc.h
ioctl.o: ../../include/sys/reg.h
ioctl.o: ../../include/sys/systm.h
ioctl.o: ../../include/sys/tty.h
ioctl.o: ../../include/sys/user.h
machdep.o: ../../include/sys/acct.h
machdep.o: ../../include/sys/buf.h
machdep.o: ../../include/sys/dir.h
machdep.o: ../../include/sys/inode.h
machdep.o: ../../include/sys/map.h
machdep.o: ../../include/sys/param.h
machdep.o: ../../include/sys/proc.h
machdep.o: ../../include/sys/reg.h
machdep.o: ../../include/sys/seg.h
machdep.o: ../../include/sys/systm.h
machdep.o: ../../include/sys/user.h
main.o: ../../include/sys/buf.h
main.o: ../../include/sys/conf.h
main.o: ../../include/sys/dir.h
main.o: ../../include/sys/filsys.h
main.o: ../../include/sys/inode.h
main.o: ../../include/sys/map.h
main.o: ../../include/sys/mount.h
main.o: ../../include/sys/param.h
main.o: ../../include/sys/proc.h
main.o: ../../include/sys/seg.h
main.o: ../../include/sys/systm.h
main.o: ../../include/sys/user.h
malloc.o: ../../include/sys/map.h
malloc.o: ../../include/sys/param.h
malloc.o: ../../include/sys/systm.h
malloc.o: ../../include/sys/vm.h
nami.o: ../../include/sys/buf.h
nami.o: ../../include/sys/dir.h
nami.o: ../../include/sys/inode.h
nami.o: ../../include/sys/mount.h
nami.o: ../../include/sys/param.h
nami.o: ../../include/sys/quota.h
nami.o: ../../include/sys/systm.h
nami.o: ../../include/sys/user.h
pipe.o: ../../include/sys/dir.h
pipe.o: ../../include/sys/file.h
pipe.o: ../../include/sys/inode.h
pipe.o: ../../include/sys/param.h
pipe.o: ../../include/sys/reg.h
pipe.o: ../../include/sys/systm.h
pipe.o: ../../include/sys/user.h
prf.o: ../../include/sys/buf.h
prf.o: ../../include/sys/conf.h
prf.o: ../../include/sys/filsys.h
prf.o: ../../include/sys/mount.h
prf.o: ../../include/sys/param.h
prf.o: ../../include/sys/seg.h
prf.o: ../../include/sys/systm.h
prim.o: ../../include/sys/buf.h
prim.o: ../../include/sys/conf.h
prim.o: ../../include/sys/map.h
prim.o: ../../include/sys/param.h
prim.o: ../../include/sys/seg.h
prim.o: ../../include/sys/systm.h
prim.o: ../../include/sys/tty.h
rdwri.o: ../../include/sys/buf.h
rdwri.o: ../../include/sys/conf.h
rdwri.o: ../../include/sys/dir.h
rdwri.o: ../../include/sys/inode.h
rdwri.o: ../../include/sys/param.h
rdwri.o: ../../include/sys/systm.h
rdwri.o: ../../include/sys/user.h
sig.o: ../../include/sys/dir.h
sig.o: ../../include/sys/inode.h
sig.o: ../../include/sys/param.h
sig.o: ../../include/sys/proc.h
sig.o: ../../include/sys/reg.h
sig.o: ../../include/sys/seg.h
sig.o: ../../include/sys/systm.h
sig.o: ../../include/sys/text.h
sig.o: ../../include/sys/user.h
sig.o: ../../include/sys/vm.h
slp.o: ../../include/sys/buf.h
slp.o: ../../include/sys/dir.h
slp.o: ../../include/sys/file.h
slp.o: ../../include/sys/inode.h
slp.o: ../../include/sys/map.h
slp.o: ../../include/sys/param.h
slp.o: ../../include/sys/proc.h
slp.o: ../../include/sys/systm.h
slp.o: ../../include/sys/text.h
slp.o: ../../include/sys/user.h
slp.o: ../../include/sys/vm.h
subr.o: ../../include/sys/buf.h
subr.o: ../../include/sys/conf.h
subr.o: ../../include/sys/dir.h
subr.o: ../../include/sys/inode.h
subr.o: ../../include/sys/param.h
subr.o: ../../include/sys/quota.h
subr.o: ../../include/sys/systm.h
subr.o: ../../include/sys/user.h
sys1.o: ../../include/sys/acct.h
sys1.o: ../../include/sys/buf.h
sys1.o: ../../include/sys/dir.h
sys1.o: ../../include/sys/inode.h
sys1.o: ../../include/sys/map.h
sys1.o: ../../include/sys/param.h
sys1.o: ../../include/sys/proc.h
sys1.o: ../../include/sys/reg.h
sys1.o: ../../include/sys/seg.h
sys1.o: ../../include/sys/systm.h
sys1.o: ../../include/sys/user.h
sys2.o: ../../include/sys/dir.h
sys2.o: ../../include/sys/file.h
sys2.o: ../../include/sys/inode.h
sys2.o: ../../include/sys/param.h
sys2.o: ../../include/sys/quota.h
sys2.o: ../../include/sys/reg.h
sys2.o: ../../include/sys/systm.h
sys2.o: ../../include/sys/user.h
sys3.o: ../../include/sys/buf.h
sys3.o: ../../include/sys/conf.h
sys3.o: ../../include/sys/dir.h
sys3.o: ../../include/sys/file.h
sys3.o: ../../include/sys/filsys.h
sys3.o: ../../include/sys/ino.h
sys3.o: ../../include/sys/inode.h
sys3.o: ../../include/sys/mount.h
sys3.o: ../../include/sys/param.h
sys3.o: ../../include/sys/reg.h
sys3.o: ../../include/sys/stat.h
sys3.o: ../../include/sys/systm.h
sys3.o: ../../include/sys/user.h
sys4.o: ../../include/sys/dir.h
sys4.o: ../../include/sys/inode.h
sys4.o: ../../include/sys/param.h
sys4.o: ../../include/sys/proc.h
sys4.o: ../../include/sys/quota.h
sys4.o: ../../include/sys/reg.h
sys4.o: ../../include/sys/systm.h
sys4.o: ../../include/sys/timeb.h
sys4.o: ../../include/sys/user.h
sysent.o: ../../include/sys/param.h
sysent.o: ../../include/sys/quota.h
sysent.o: ../../include/sys/systm.h
syslocal.o: ../../include/sys/conf.h
syslocal.o: ../../include/sys/dir.h
syslocal.o: ../../include/sys/file.h
syslocal.o: ../../include/sys/ino.h
syslocal.o: ../../include/sys/inode.h
syslocal.o: ../../include/sys/lstat.h
syslocal.o: ../../include/sys/param.h
syslocal.o: ../../include/sys/proc.h
syslocal.o: ../../include/sys/quota.h
syslocal.o: ../../include/sys/seg.h
syslocal.o: ../../include/sys/systm.h
syslocal.o: ../../include/sys/user.h
text.o: ../../include/sys/buf.h
text.o: ../../include/sys/dir.h
text.o: ../../include/sys/inode.h
text.o: ../../include/sys/map.h
text.o: ../../include/sys/param.h
text.o: ../../include/sys/proc.h
text.o: ../../include/sys/seg.h
text.o: ../../include/sys/systm.h
text.o: ../../include/sys/text.h
text.o: ../../include/sys/user.h
trap.o: ../../include/sys/dir.h
trap.o: ../../include/sys/param.h
trap.o: ../../include/sys/proc.h
trap.o: ../../include/sys/reg.h
trap.o: ../../include/sys/seg.h
trap.o: ../../include/sys/systm.h
trap.o: ../../include/sys/user.h
trap.o: ../../include/sys/vm.h
ureg.o: ../../include/sys/dir.h
ureg.o: ../../include/sys/param.h
ureg.o: ../../include/sys/proc.h
ureg.o: ../../include/sys/seg.h
ureg.o: ../../include/sys/systm.h
ureg.o: ../../include/sys/text.h
ureg.o: ../../include/sys/user.h
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
