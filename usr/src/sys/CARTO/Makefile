# source paths
S=	../sys
D=	../dev
N=	../net

# Optional files -- add your local hacks here
# Make sure you add "opt" as a dependency for novunix and ovunix.
# Also, uncomment the line in "depend" that makes the optional
# dependencies.
#OPT=	$S/enable34.c $D/bk.c $D/cr.c $D/rf.c

OPT=

# Configuration files
CONF=	c.c ioconf.c

# System files
SYS=	$S/acct.c $S/alloc.c $S/clock.c  $S/fio.c $S/iget.c $S/ioctl.c \
	$S/machdep.c $S/main.c $S/malloc.c $S/nami.c $S/pipe.c $S/prf.c \
	$S/prim.c $S/rdwri.c $S/sig.c $S/slp.c $S/subr.c $S/sys1.c \
	$S/sys2.c $S/sys3.c $S/sys4.c $S/sysent.c $S/syslocal.c $S/text.c \
	$S/trap.c $S/ureg.c $S/ipc.c $S/mbuf.c $S/mbuf11.c $S/proto.c \
	$S/socket.c $S/socketsubr.c

# Device files
DEV=	$D/bio.c $D/cary.c $D/cat.c $D/dh.c $D/dhdm.c $D/dhfdm.c $D/dkbad.c \
	$D/dkleave.c $D/dn.c $D/dsort.c $D/dvhp.c $D/dz.c $D/hk.c $D/hp.c \
	$D/hs.c $D/ht.c $D/kl.c $D/lp.c $D/mem.c $D/partab.c $D/tty_pty.c \
	$D/rf.c $D/rk.c $D/rl.c $D/rm.c $D/rp.c $D/sys.c $D/tm.c $D/ts.c \
	$D/tty.c $D/ttynew.c $D/ttyold.c $D/vp.c $D/xp.c $D/dr.c

# Network files
NET=	$N/af.c $N/if.c $N/if_acc.c $N/if_css.c $N/if_dmc.c $N/if_ec.c \
	$N/if_en.c $N/if_ether.c $N/if_il.c $N/if_imp.c $N/if_imphost.c \
	$N/if_loop.c $N/if_sri.c $N/if_uba.c $N/if_un.c $N/in.c $N/in_pcb.c \
	$N/ip_icmp.c $N/ip_input.c $N/ip_output.c $N/pup.c $N/raw_cb.c \
	$N/if_vv.c $N/raw_imp.c $N/raw_ip.c $N/raw_pup.c $N/raw_usrreq.c \
	$N/route.c $N/tcp_debug.c $N/tcp_input.c $N/tcp_output.c $N/tcp_subr.c \
	$N/tcp_timer.c $N/tcp_usrreq.c $N/udp_usrreq.c

# OV should be -V for overlaid systems, empty otherwise
OV=-V

# maxusers is used to scale the size of kernel data structures.
MAXUSERS=7

# SPLFIX should be one of :splfix.movb, :splfix.mtps or :splfix.spl
SPLFIX=	:splfix.spl

I=	/usr/include
AS=	as ${OV}
DEFS=	-DKERNEL -Dpdp11
INC=	-I. -I${I}
CFLAGS=	-O ${OV} ${DEFS} ${INC}
CPP=	/lib/cpp -P ${DEFS} ${INC}
C=	${CC} ${CFLAGS} -S
E=	ed - < ${SPLFIX}
A=	${AS} - -o

OFILE=	unix

BASE=	alloc.o bio.o clock.o dkleave.o fio.o iget.o machdep.o main.o \
	malloc.o mem.o nami.o partab.o pipe.o prf.o prim.o proto.o rdwri.o \
	sig.o slp.o subr.o sys.o sys1.o sys2.o sys3.o sys4.o sysent.o \
	syslocal.o text.o trap.o ureg.o xp.o ip_input.o ip_output.o dr.o \
	dz.o
OV1=	tty.o ttynew.o ttyold.o kl.o dsort.o
OV2=	tty_pty.o if_vv.o mbuf11.o af.o acct.o
OV3=	ipc.o route.o mbuf.o in.o in_cksum.o
OV4=	tcp_usrreq.o tcp_input.o tcp_output.o
OV5=	rl.o lp.o tm.o ioctl.o ip_icmp.o in_pcb.o
OV6=	if.o if_loop.o if_uba.o udp_usrreq.o raw_cb.o raw_ip.o \
	raw_usrreq.o dkbad.o
OV7=	socket.o socketsubr.o tcp_debug.o tcp_subr.o tcp_timer.o
OV8=	
OV9=	
OVa=	
OVb=	
OVc=	
OVd=	
OVe=	
OVf=	

OBJS =	acct.o alloc.o clock.o fio.o iget.o ioctl.o machdep.o main.o \
	malloc.o nami.o pipe.o prf.o prim.o rdwri.o sig.o slp.o subr.o \
	sys1.o sys2.o sys3.o sys4.o sysent.o syslocal.o text.o trap.o \
	ureg.o bio.o dh.o dkbad.o dkleave.o dn.o dsort.o dz.o hk.o \
	hp.o ht.o kl.o lp.o mem.o partab.o rk.o rl.o rm.o rp.o sys.o \
	tm.o ts.o tty.o ttynew.o ttyold.o xp.o vp.o dr.o isr.o

#	boot.o must be loaded in the base well above 0 data
CONFOBJ = l.o mch.o c.o ioconf.o boot.o

#	l.o MUST be loaded first, param.o MUST be loaded last
#	don't forget the "-i" flag for split I/D, use "-n" otherwise

all:
	@echo "Make novunix or ovunix."

novunix: sys dev mch.o l.o boot.o param.o
	sh -x ../conf/newvers.sh
	${CC} -c vers.c
	ld -X -n -o ${OFILE} ${CONFOBJ} ${OBJS} vers.o param.o
	size ${OFILE}
	/usr/local/checksys ${OFILE}

ovunix: net sys dev mch.o l.o boot.o param.o in_cksum.o
	sh ../conf/newvers.sh
	${CC} -c vers.c
	ld -X -i -o ${OFILE} ${CONFOBJ} -Z ${OV1} -Z ${OV2} -Z ${OV3} \
		-Z ${OV4} -Z ${OV5} -Z ${OV6} -Z ${OV7} -Z ${OV8} -Z ${OV9} \
		-Z ${OVa} -Z ${OVb} -Z ${OVc} -Z ${OVd} -Z ${OVe} -Z ${OVf} \
		-L ${BASE} vers.o param.o
	size ${OFILE}
	/usr/local/checksys ${OFILE}

.PRECIOUS: whoami.h param.h Makefile

whoami.h:  localopts.h
	touch whoami.h

param.h: whoami.h ${I}/signal.h ${I}/sys/psw.h ${I}/sys/types.h
	touch param.h

mch.o:	assym.s ${S}/mch.s whoami.h dz.h ht.h tm.h ${I}/a.out.h \
	${I}/sys/cpu.m ${I}/sys/trap.h ${I}/sys/reboot.h ${I}/sys/iopage.m \
	${I}/sys/koverlay.h
	${CPP} ${CPPFLAGS} ${S}/mch.s > mch.i
	${AS} -o mch.o assym.s mch.i
	-rm mch.i

l.o:	l.s whoami.h ${I}/sys/trap.h ${I}/sys/iopage.h ${I}/sys/koverlay.h \
	acc.h bk.h dh.h dmc.h dn.h dr.h dvhp.h dz.h hk.h hp.h hs.h ht.h \
	il.h imp.h kl.h lp.h pty.h pup.h rf.h rk.h rl.h rm.h rp.h sri.h \
	tm.h ts.h un.h vp.h vv.h xp.h
	${CPP} ${CPPFLAGS} l.s >l.i
	${AS} -o l.o l.i
	-rm l.i

boot.o:	boot.s whoami.h
	${CPP} ${CPPFLAGS} boot.s >boot.i
	${AS} -o boot.o boot.i
	-rm boot.i

assym.s: genassym.c param.h dz.h ${I}/sys/user.h  ${I}/sys/vm.h \
	${I}/sys/proc.h ${I}/sys/dir.h ${I}/sys/inode.h ${I}/sys/seg.h
	cc ${CFLAGS} genassym.c
	./a.out > assym.s
	-rm a.out

param.o: param.c param.h Makefile ${I}/sys/proc.h ${I}/sys/text.h \
	${I}/sys/file.h ${I}/sys/inode.h ${I}/sys/dir.h ${I}/sys/buf.h \
	${I}/sys/tty.h ${I}/sys/conf.h ${I}/sys/user.h ${I}/sys/filsys.h \
	${I}/sys/mount.h ${I}/sys/acct.h ${I}/sys/systm.h ${I}/sys/map.h
	${CC} -S -DMAXUSERS=${MAXUSERS} ${CFLAGS} param.c
	ed - param.s < :comm-to-bss
	${AS} -o param.o param.s
	-rm param.s

in_cksum.o: ../net/in_cksum.c ../net/cksum-fix param.h ${I}/sys/types.h \
	${I}/sys/mbuf.h ${I}/netinet/in.h ${I}/netinet/in_systm.h
	${C} ../net/in_cksum.c
	ed - <../net/cksum-fix >/dev/null in_cksum.s
	${E} in_cksum.s
	${AS} - -o in_cksum.o in_cksum.s
	-rm in_cksum.s

depend:
	sh ../conf/depend.sh Make.net ${NET}
	sh ../conf/depend.sh Make.sys ${SYS} ${CONF}
	sh ../conf/depend.sh Make.dev ${DEV}

#	sh ../conf/depend.sh Make.opt ${OPT}

sys:
	make_small -f Make.sys OV=${OV}

dev:
	make_small -f Make.dev OV=${OV}

net:
	make_small -f Make.net OV=${OV}

opt:
	make_small -f Make.opt OV=${OV}

clean:
	rm -f *.o *.i

install:
	install -o bin -g bin -m 744 unix /
