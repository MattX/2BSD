#
# net makefile
#

COMMON=	gethostnam.o gethostadd.o gethostent.o getnetaddr.o getnetent.o \
	getnetname.o getproto.o getprotent.o getprotnum.o getservent.o \
	getservpor.o getservnam.o raddr.o rcmd.o rexec.o rhost.o ruserpass.o

INET=	inet_addr.o inet_network.o inet_netof.o inet_lnaof.o inet_maddr.o \
	inet_ntoa.o

SYS=	accept.o connect.o gethostid.o gethostname.o hton.o receive.o \
	select.o send.o sethostid.o sethostname.o setregid.o setreuid.o \
	socket.o socketa.o

GEN=	bcopy.o bcmp.o insque.o bzero.o

CFLAGS=	-DUCB_NET -O
OV=

libc.a: ${COMMON} ${INET} ${SYS} ${GEN}
	ar q ../$@ ${COMMON} ${INET} ${SYS} ${GEN}

libovc.a: ${COMMON} ${INET} ${SYS} ${GEN}
	ar q ../$@ ${COMMON} ${INET} ${SYS} ${GEN}

.s.o:
	as - -o $*.o $*.s

.c.o:
	${CC} -S ${CFLAGS} ${FP} -c $*.c
	sed -f ../../../asm.sed $*.s > $*.S
	as ${OV} - -o $*.o $*.S
	rm -f $*.s $*.S

clean:
	rm -f ${COMMON} ${INET} ${SYS} ${GEN}

accept.o:	accept.s
bcmp.o:		bcmp.c
bcopy.o:	bcopy.c
bzero.o:	bzero.s
connect.o:	connect.s
gethostadd.o:	gethostadd.c
gethostent.o:	gethostent.c
gethostid.o:	gethostid.s
gethostnam.o:	gethostnam.c
gethostname.o:	gethostname.s
getnetaddr.o:	getnetaddr.c
getnetent.o:	getnetent.c
getnetname.o:	getnetname.c
getprotent.o:	getprotent.c
getprotnum.o:	getprotnum.c
getproto.o:	getproto.c
getservent.o:	getservent.c
getservnam.o:	getservnam.c
getservpor.o:	getservpor.c
hton.o:		hton.s
inet_addr.o:	inet_addr.c
inet_lnaof.o:	inet_lnaof.c
inet_maddr.o:	inet_maddr.c
inet_netof.o:	inet_netof.c
inet_network.o:	inet_network.c
inet_ntoa.o:	inet_ntoa.c
insque.o:	insque.c
raddr.o:	raddr.c
rcmd.o:		rcmd.c
receive.o:	receive.s
rexec.o:	rexec.c
rhost.o:	rhost.c
ruserpass.o:	ruserpass.c
select.o:	select.s
send.o:		send.s
sethostid.o:	sethostid.s
sethostname.o:	sethostname.s
setregid.o:	setregid.s
setreuid.o:	setreuid.s
socket.o:	socket.s
socketa.o:	socketa.s
