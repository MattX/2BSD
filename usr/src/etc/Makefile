#
# etc makefile
#

CFLAGS=	-O

ac: ac.o
	${CC} ${LFLAG} $@.o -o $@

pstat: pstat.o
	${CC} ${LFLAG} $@.o -o $@

sa: sa.o
	${CC} ${LFLAG} $@.o -o $@

accton: accton.o
	${CC} ${LFLAG} $@.o -o $@

bad144: bad144.o
	${CC} ${LFLAG} $@.o -o $@

catman: catman.o
	${CC} ${LFLAG} $@.o -o $@

badsect: badsect.o
	${CC} ${LFLAG} $@.o -o $@

chown: chown.o
	${CC} ${LFLAG} $@.o -o $@

cron: cron.o
	${CC} ${LFLAG} $@.o -o $@

dmesg: dmesg.o
	${CC} ${LFLAG} $@.o -o $@

file: file.o
	${CC} ${LFLAG} $@.o -o $@

fsck: fsck.o
	${CC} ${LFLAG} $@.o -o $@

getty: getty.o
	${CC} ${LFLAG} $@.o -o $@

init: init.o
	${CC} ${LFLAG} $@.o -o $@

mkfs: mkfs.o
	${CC} ${LFLAG} $@.o -o $@

mknod: mknod.o
	${CC} ${LFLAG} $@.o -o $@

mount: mount.o
	${CC} ${LFLAG} $@.o -o $@

quot: quot.o
	${CC} ${LFLAG} $@.o -o $@

reboot: reboot.o
	${CC} ${LFLAG} $@.o -o $@

savecore: savecore.o
	${CC} ${LFLAG} $@.o -o $@

shutdown: shutdown.o
	${CC} ${LFLAG} $@.o -o $@

umount: umount.o
	${CC} ${LFLAG} $@.o -o $@

update: update.o
	${CC} ${LFLAG} $@.o -o $@

dump: dump.o
	${CC} ${LFLAG} $@.o -o $@

dumpdir: dumpdir.o
	${CC} ${LFLAG} $@.o -o $@

renice: renice.o
	${CC} ${LFLAG} $@.o -o $@

restor: restor.o
	${CC} ${LFLAG} $@.o -o $@

rlogind: rlogind.o
	${CC} ${LFLAG} $@.o -o $@

rshd: rshd.o
	${CC} ${LFLAG} $@.o -o $@

route: route.o
	${CC} ${LFLAG} $@.o -o $@

rexecd: rexecd.o
	${CC} ${LFLAG} $@.o -o $@

trpt: trpt.o
	${CC} ${LFLAG} $@.o -o $@

rmt: rmt.o
	${CC} ${LFLAG} $@.o -o $@

ifconfig: ifconfig.o
	${CC} ${LFLAG} $@.o -o $@

comsat: comsat.o
	${CC} ${LFLAG} $@.o -o $@

arp: arp.o
	${CC} ${LFLAG} $@.o -o $@

rwhod: rwhod.o
	${CC} ${LFLAG} $@.o -o $@

tftpd: tftpd.c
	${CC} -S ${CFLAGS} tftpd.c
	sed -f ../asm.sed tftpd.s > tftpd.S
	as - -o tftpd.o tftpd.S
	rm -f tftpd.s tftpd.S
	${CC} ${LFLAG} $@.o -o $@

telnetd: telnetd.c
	${CC} -S ${CFLAGS} telnetd.c
	sed -f ../asm.sed telnetd.s > telnetd.S
	as - -o telnetd.o telnetd.S
	rm -f telnetd.s telnetd.S
	${CC} ${LFLAG} $@.o -o $@

.c.o:
	${CC} ${CFLAGS} ${FP} -c $*.c

ac.o:		ac.c
pstat.o:	pstat.c
sa.o:		sa.c
accton.o:	accton.c
bad144.o:	bad144.c
badsect.o:	badsect.c
catman.o:	catman.c
chown.o:	chown.c
cron.o:		cron.c
dmesg.o:	dmesg.c
file.o:		file.c
fsck.o:		fsck.c
getty.o:	getty.c
init.o:		init.c
mkfs.o:		mkfs.c
mknod.o:	mknod.c
mount.o:	mount.c
quot.o:		quot.c
reboot.o:	reboot.c
savecore.o:	savecore.c
shutdown.o:	shutdown.c
umount.o:	umount.c
update.o:	update.c
dump.o:		dump.c
dumpdir.o:	dumpdir.c
renice.o:	renice.c
restor.o:	restor.c
rlogind.o:	rlogind.c
rshd.o:		rshd.c
rwhod.o:	rwhod.c
arp.o:		arp.c
comsat.o:	comsat.c
ifconfig.o:	ifconfig.c
rexecd.o:	rexecd.c
rmt.o:		rmt.c
trpt.o:		trpt.c
route.o:	route.c
