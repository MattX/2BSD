#	Makefile for Berkeley UNIX kernel, overlay version
#
#	Makefile Configuration:
#		First, edit the Depend file to include any optional
#		device drivers you need to include.
#		Then check the global definitions, MAXUSERS, and include file
#		search paths below.
#		INCLUDE is the directory with the normal complement of
#		include files, normally /usr/include.
#		Finally, run "make depend" and stand back.
#
#	After having made a unix binary the first time, the overlay
#	structure specified below will probably have to change.
#	The base segment must be between 8194 and 16384 inclusive,
#	and each overlay (7 maximum) must be <= 8192.

# global definitions
DEFS =	-DKERNEL -Dpdp11

# maxusers is used to scale the size of kernel data structures.
MAXUSERS=2

# SPLFIX should be one of :splfix.movb, :splfix.mtps or :splfix.spl
SPLFIX=	:splfix.net23

# include file search path
INCLUDE= /usr/include

CPPFLAGS= ${DEFS} -I. -I${INCLUDE}
CFLAGS=	-O ${CPPFLAGS}
CC =	cc -V
CPP =	/lib/cpp -P
AS =	as -V
LD =	ld

#  shorthand to avoid running make out of space
C =	${CC} -S ${CFLAGS}
E =	ed - < ${SPLFIX}
A =	${AS} - -o
K =	ed - < ../net/cksum-fix
I =	${INCLUDE}

OFILE =	unix

# move nami.o into the base segment if possible
OV1 =	sys2.o sys4.o fio.o pipe.o alloc.o iget.o  slp.o rdwri.o dsort.o ra.o \
	sys.o ttyold.o tm.o dh.o
OV2 =	text.o ureg.o malloc.o sys1.o main.o mem.o sig.o trap.o clock.o rd.o \
	xp.o hp.o
OV3 =	bio.o dkbad.o dkleave.o subr.o machdep.o sys3.o syslocal.o \
	acct.o prf.o ioctl.o cn.o dz.o prostuff.o
OV4 =	ttyold.o ttynew.o tty.o prim.o kl.o partab.o nami.o hk.o pc.o r5.o
OV5 =	if_dc.o if_qn.o if_ec.o if_il.o if_sri.o if_uba.o in.o \
	in_pcb.o ip_icmp.o ip_input.o \
	ip_output.o raw_cb.o raw_imp.o
OV6 =	ipc.o mbuf.o mbuf11.o proto.o socket.o socketsubr.o pty.o \
	tcp_debug.o tcp_input.o
OV7 =	af.o if.o if_loop.o if_imp.o \
	if_imphost.o if_ether.o \
	raw_ip.o raw_pup.o raw_usrreq.o \
	tcp_output.o tcp_subr.o tcp_timer.o \
	tcp_usrreq.o udp_usrreq.o in_cksum.o route.o
BASE =	sysent.o

#	boot.o must be loaded in the base well above 0 data
CONFOBJ = l.o mch.o c.o ioconf.o boot.o 


#	l.o MUST be loaded first, param.o MUST be loaded last
${OFILE}: Nofile ${OV1} ${OV2} ${OV3} ${OV4} ${OV5} ${OV6} ${OV7} ${BASE} ${CONFOBJ} \
    param.o Makefile checksys
	sh newvers.sh
	${CC} -c vers.c
	${LD} -X -n -o ${OFILE}\
		${CONFOBJ}\
		-Z ${OV1} -Z ${OV2} -Z ${OV3} -Z ${OV4}\
		-Z ${OV5} -Z ${OV6} -Z ${OV7}\
		-L ${BASE} vers.o param.o
	size ${OFILE}
	./checksys -b ${OFILE}

.PRECIOUS: whoami.h param.h Makefile ../sys/sig.c

Nofile:
	@make -f Makeobj1 > /dev/null
	@make -f Makeobj2 > /dev/null

whoami.h:  localopts.h
	touch whoami.h

param.h: whoami.h ${I}/signal.h ${I}/sys/psw.h \
	${I}/sys/types.h 
	touch param.h

mch.o:	assym.s ../sys/mch.s whoami.h dz.h ht.h tm.h \
	${I}/a.out.h ${I}/sys/cpu.m ${I}/sys/trap.h \
	${I}/sys/reboot.h ${I}/sys/iopage.m \
	${I}/sys/koverlay.h
	${CPP} ${CPPFLAGS} ../sys/mch.s >mch.i
	${AS} -o mch.o assym.s mch.i
	-rm mch.i

l.o:	l.s whoami.h ${I}/a.out.h \
	${I}/sys/trap.h ${I}/sys/koverlay.h \
	dh.h dn.h dz.h hk.h hp.h hs.h ht.h kl.h lp.h \
	rk.h rl.h rm.h rp.h tm.h ts.h vp.h xp.h
	${CPP} ${CPPFLAGS} l.s >l.i
	${AS} -o l.o l.i
	-rm l.i

boot.o:	boot.s whoami.h
	${CPP} ${CPPFLAGS} boot.s >boot.i
	${AS} -o boot.o boot.i
	-rm boot.i

assym.s: genassym.c param.h dz.h \
	${I}/sys/user.h  ${I}/sys/vm.h ${I}/sys/proc.h \
	${I}/sys/dir.h ${I}/sys/inode.h ${I}/sys/seg.h
	cc ${CFLAGS} genassym.c; ./a.out >assym.s; rm -f a.out

paramed:  localopts.h
	${CPP} ${CPPFLAGS} :comm-to-bss|sed -e '/^[ 	]*$$/d'>paramed

param.o: paramed param.c param.h Makefile ${I}/sys/proc.h \
	${I}/sys/text.h ${I}/sys/file.h ${I}/sys/inode.h \
	${I}/sys/dir.h ${I}/sys/buf.h ${I}/sys/tty.h \
	${I}/sys/conf.h ${I}/sys/user.h ${I}/sys/filsys.h \
	${I}/sys/mount.h ${I}/sys/acct.h ${I}/sys/systm.h \
	${I}/sys/map.h
	${CC} -S -DMAXUSERS=${MAXUSERS} ${CFLAGS} param.c
	ed - param.s <paramed
	${AS} -o param.o param.s
	-rm param.s

in_cksum.o:
	${C} ../net/in_cksum.c
	${K} in_cksum.s
	${E} in_cksum.s
	${A} in_cksum.o in_cksum.s
	-rm in_cksum.s

checksys: checksys.c
	${CC} -I. -I${INCLUDE} -O -o checksys checksys.c

../sys/sig.c: ../sys/sigjcl.c ../sys/signojcl.c
	touch ../sys/sig.c

sig.o: param.h ${I}/sys/systm.h ${I}/sys/dir.h \
	${I}/sys/user.h ${I}/sys/proc.h ${I}/sys/inode.h \
	${I}/sys/reg.h ${I}/sys/text.h ${I}/sys/seg.h \
	${I}/sys/vm.h
	${C} ../sys/sig.c
	${E} sig.s
	${A} sig.o sig.s
	-rm sig.s

depend:
	@make -f Objdep1 depend
	@make -f Objdep2 depend
	@make -f Depend depend

clean:
	rm -f *.o *.i

# DO NOT DELETE THIS LINE

c.o:
	${C} c.c
	${E} c.s
	${A} c.o c.s
	-rm c.s

ioconf.o:
	${C} ioconf.c
	${E} ioconf.s
	${A} ioconf.o ioconf.s
	-rm ioconf.s

acct.o:
	${C} ../sys/acct.c
	${E} acct.s
	${A} acct.o acct.s
	-rm acct.s

alloc.o:
	${C} ../sys/alloc.c
	${E} alloc.s
	${A} alloc.o alloc.s
	-rm alloc.s

clock.o:
	${C} ../sys/clock.c
	${E} clock.s
	${A} clock.o clock.s
	-rm clock.s

fio.o:
	${C} ../sys/fio.c
	${E} fio.s
	${A} fio.o fio.s
	-rm fio.s

iget.o:
	${C} ../sys/iget.c
	${E} iget.s
	${A} iget.o iget.s
	-rm iget.s

ioctl.o:
	${C} ../sys/ioctl.c
	${E} ioctl.s
	${A} ioctl.o ioctl.s
	-rm ioctl.s

machdep.o:
	${C} ../sys/machdep.c
	${E} machdep.s
	${A} machdep.o machdep.s
	-rm machdep.s

main.o:
	${C} ../sys/main.c
	${E} main.s
	${A} main.o main.s
	-rm main.s

malloc.o:
	${C} ../sys/malloc.c
	${E} malloc.s
	${A} malloc.o malloc.s
	-rm malloc.s

nami.o:
	${C} ../sys/nami.c
	${E} nami.s
	${A} nami.o nami.s
	-rm nami.s

pipe.o:
	${C} ../sys/pipe.c
	${E} pipe.s
	${A} pipe.o pipe.s
	-rm pipe.s

prf.o:
	${C} ../sys/prf.c
	${E} prf.s
	${A} prf.o prf.s
	-rm prf.s

prim.o:
	${C} ../sys/prim.c
	${E} prim.s
	${A} prim.o prim.s
	-rm prim.s

rdwri.o:
	${C} ../sys/rdwri.c
	${E} rdwri.s
	${A} rdwri.o rdwri.s
	-rm rdwri.s

sigjcl.o:
	${C} ../sys/sigjcl.c
	${E} sigjcl.s
	${A} sigjcl.o sigjcl.s
	-rm sigjcl.s

signojcl.o:
	${C} ../sys/signojcl.c
	${E} signojcl.s
	${A} signojcl.o signojcl.s
	-rm signojcl.s

slp.o:
	${C} ../sys/slp.c
	${E} slp.s
	${A} slp.o slp.s
	-rm slp.s

subr.o:
	${C} ../sys/subr.c
	${E} subr.s
	${A} subr.o subr.s
	-rm subr.s

sys1.o:
	${C} ../sys/sys1.c
	${E} sys1.s
	${A} sys1.o sys1.s
	-rm sys1.s

sys2.o:
	${C} ../sys/sys2.c
	${E} sys2.s
	${A} sys2.o sys2.s
	-rm sys2.s

sys3.o:
	${C} ../sys/sys3.c
	${E} sys3.s
	${A} sys3.o sys3.s
	-rm sys3.s

sys4.o:
	${C} ../sys/sys4.c
	${E} sys4.s
	${A} sys4.o sys4.s
	-rm sys4.s

sysent.o:
	${C} ../sys/sysent.c
	${E} sysent.s
	${A} sysent.o sysent.s
	-rm sysent.s

syslocal.o:
	${C} ../sys/syslocal.c
	${E} syslocal.s
	${A} syslocal.o syslocal.s
	-rm syslocal.s

text.o:
	${C} ../sys/text.c
	${E} text.s
	${A} text.o text.s
	-rm text.s

trap.o:
	${C} ../sys/trap.c
	${E} trap.s
	${A} trap.o trap.s
	-rm trap.s

ureg.o:
	${C} ../sys/ureg.c
	${E} ureg.s
	${A} ureg.o ureg.s
	-rm ureg.s

af.o:
	${C} ../net/af.c
	${E} af.s
	${A} af.o af.s
	-rm af.s

if.o:
	${C} ../net/if.c
	${E} if.s
	${A} if.o if.s
	-rm if.s

if_loop.o:
	${C} ../net/if_loop.c
	${E} if_loop.s
	${A} if_loop.o if_loop.s
	-rm if_loop.s

if_imp.o:
	${C} ../net/if_imp.c
	${E} if_imp.s
	${A} if_imp.o if_imp.s
	-rm if_imp.s

if_imphost.o:
	${C} ../net/if_imphost.c
	${E} if_imphost.s
	${A} if_imphost.o if_imphost.s
	-rm if_imphost.s

if_ether.o:
	${C} ../net/if_ether.c
	${E} if_ether.s
	${A} if_ether.o if_ether.s
	-rm if_ether.s

if_il.o:
	${C} ../net/if_il.c
	${E} if_il.s
	${A} if_il.o if_il.s
	-rm if_il.s

if_sri.o:
	${C} ../net/if_sri.c
	${E} if_sri.s
	${A} if_sri.o if_sri.s
	-rm if_sri.s

if_uba.o:
	${C} ../net/if_uba.c
	${E} if_uba.s
	${A} if_uba.o if_uba.s
	-rm if_uba.s

in.o:
	${C} ../net/in.c
	${E} in.s
	${A} in.o in.s
	-rm in.s

in_pcb.o:
	${C} ../net/in_pcb.c
	${E} in_pcb.s
	${A} in_pcb.o in_pcb.s
	-rm in_pcb.s

ip_icmp.o:
	${C} ../net/ip_icmp.c
	${E} ip_icmp.s
	${A} ip_icmp.o ip_icmp.s
	-rm ip_icmp.s

ip_input.o:
	${C} ../net/ip_input.c
	${E} ip_input.s
	${A} ip_input.o ip_input.s
	-rm ip_input.s

ip_output.o:
	${C} ../net/ip_output.c
	${E} ip_output.s
	${A} ip_output.o ip_output.s
	-rm ip_output.s

raw_cb.o:
	${C} ../net/raw_cb.c
	${E} raw_cb.s
	${A} raw_cb.o raw_cb.s
	-rm raw_cb.s

raw_imp.o:
	${C} ../net/raw_imp.c
	${E} raw_imp.s
	${A} raw_imp.o raw_imp.s
	-rm raw_imp.s

raw_ip.o:
	${C} ../net/raw_ip.c
	${E} raw_ip.s
	${A} raw_ip.o raw_ip.s
	-rm raw_ip.s

raw_pup.o:
	${C} ../net/raw_pup.c
	${E} raw_pup.s
	${A} raw_pup.o raw_pup.s
	-rm raw_pup.s

raw_usrreq.o:
	${C} ../net/raw_usrreq.c
	${E} raw_usrreq.s
	${A} raw_usrreq.o raw_usrreq.s
	-rm raw_usrreq.s

route.o:
	${C} ../net/route.c
	${E} route.s
	${A} route.o route.s
	-rm route.s

tcp_debug.o:
	${C} ../net/tcp_debug.c
	${E} tcp_debug.s
	${A} tcp_debug.o tcp_debug.s
	-rm tcp_debug.s

tcp_input.o:
	${C} ../net/tcp_input.c
	${E} tcp_input.s
	${A} tcp_input.o tcp_input.s
	-rm tcp_input.s

tcp_output.o:
	${C} ../net/tcp_output.c
	${E} tcp_output.s
	${A} tcp_output.o tcp_output.s
	-rm tcp_output.s

tcp_subr.o:
	${C} ../net/tcp_subr.c
	${E} tcp_subr.s
	${A} tcp_subr.o tcp_subr.s
	-rm tcp_subr.s

tcp_timer.o:
	${C} ../net/tcp_timer.c
	${E} tcp_timer.s
	${A} tcp_timer.o tcp_timer.s
	-rm tcp_timer.s

if_ec.o:
	${C} ../net/if_ec.c
	${E} if_ec.s
	${A} if_ec.o if_ec.s
	-rm if_ec.s

if_qn.o:
	${C} ../net/if_qn.c
	${E} if_qn.s
	${A} if_qn.o if_qn.s
	-rm if_qn.s

if_dc.o:
	${C} ../net/if_dc.c
	${E} if_dc.s
	${A} if_dc.o if_dc.s
	-rm if_dc.s

tcp_usrreq.o:
	${C} ../net/tcp_usrreq.c
	${E} tcp_usrreq.s
	${A} tcp_usrreq.o tcp_usrreq.s
	-rm tcp_usrreq.s

udp_usrreq.o:
	${C} ../net/udp_usrreq.c
	${E} udp_usrreq.s
	${A} udp_usrreq.o udp_usrreq.s
	-rm udp_usrreq.s

ipc.o:
	${C} ../sys/ipc.c
	${E} ipc.s
	${A} ipc.o ipc.s
	-rm ipc.s

mbuf.o:
	${C} ../sys/mbuf.c
	${E} mbuf.s
	${A} mbuf.o mbuf.s
	-rm mbuf.s

mbuf11.o:
	${C} ../sys/mbuf11.c
	${E} mbuf11.s
	${A} mbuf11.o mbuf11.s
	-rm mbuf11.s

proto.o:
	${C} ../sys/proto.c
	${E} proto.s
	${A} proto.o proto.s
	-rm proto.s

socket.o:
	${C} ../sys/socket.c
	${E} socket.s
	${A} socket.o socket.s
	-rm socket.s

socketsubr.o:
	${C} ../sys/socketsubr.c
	${E} socketsubr.s
	${A} socketsubr.o socketsubr.s
	-rm socketsubr.s

pty.o:
	${C} ../dev/pty.c
	${E} pty.s
	${A} pty.o pty.s
	-rm pty.s

bio.o:
	${C} ../dev/bio.c
	${E} bio.s
	${A} bio.o bio.s
	-rm bio.s

dh.o:
	${C} ../dev/dh.c
	${E} dh.s
	${A} dh.o dh.s
	-rm dh.s

dkbad.o:
	${C} ../dev/dkbad.c
	${E} dkbad.s
	${A} dkbad.o dkbad.s
	-rm dkbad.s

dkleave.o:
	${C} ../dev/dkleave.c
	${E} dkleave.s
	${A} dkleave.o dkleave.s
	-rm dkleave.s

dsort.o:
	${C} ../dev/dsort.c
	${E} dsort.s
	${A} dsort.o dsort.s
	-rm dsort.s

hk.o:
	${C} ../dev/hk.c
	${E} hk.s
	${A} hk.o hk.s
	-rm hk.s

dz.o:
	${C} ../dev/dz.c
	${E} dz.s
	${A} dz.o dz.s
	-rm dz.s

kl.o:
	${C} ../dev/kl.c
	${E} kl.s
	${A} kl.o kl.s
	-rm kl.s

hp.o:
	${C} ../dev/hp.c
	${E} hp.s
	${A} hp.o hp.s
	-rm hp.s

mem.o:
	${C} ../dev/mem.c
	${E} mem.s
	${A} mem.o mem.s
	-rm mem.s

partab.o:
	${C} ../dev/partab.c
	${E} partab.s
	${A} partab.o partab.s
	-rm partab.s

xp.o:
	${C} ../dev/xp.c
	${E} xp.s
	${A} xp.o xp.s
	-rm xp.s

sys.o:
	${C} ../dev/sys.c
	${E} sys.s
	${A} sys.o sys.s
	-rm sys.s

tm.o:
	${C} ../dev/tm.c
	${E} tm.s
	${A} tm.o tm.s
	-rm tm.s

tty.o:
	${C} ../dev/tty.c
	${E} tty.s
	${A} tty.o tty.s
	-rm tty.s

ttynew.o:
	${C} ../dev/ttynew.c
	${E} ttynew.s
	${A} ttynew.o ttynew.s
	-rm ttynew.s

ttyold.o:
	${C} ../dev/ttyold.c
	${E} ttyold.s
	${A} ttyold.o ttyold.s
	-rm ttyold.s

ra.o:
	${C} ../dev/ra.c
	${E} ra.s
	${A} ra.o ra.s
	-rm ra.s

rd.o:
	${C} ../dev/rd.c
	${E} rd.s
	${A} rd.o rd.s
	-rm rd.s

r5.o:
	${C} ../dev/r5.c
	${E} r5.s
	${A} r5.o r5.s
	-rm r5.s

pc.o:
	${C} ../dev/pc.c
	${E} pc.s
	${A} pc.o pc.s
	-rm pc.s

cn.o:
	${C} ../dev/cn.c
	${E} cn.s
	${A} cn.o cn.s
	-rm cn.s

prostuff.o:
	${C} ../sys/prostuff.c
	${E} prostuff.s
	${A} prostuff.o prostuff.s
	-rm prostuff.s

# DEPENDENCIES MUST END HERE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
