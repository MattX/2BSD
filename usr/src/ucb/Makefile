#
#ucbmakefile
#

CFLAGS=	-O

arcv: arcv.o
	${CC} ${LFLAG} $@.o -o $@

checknr: checknr.o
	${CC} ${LFLAG} $@.o -o $@

chfn: chfn.o
	${CC} ${LFLAG} $@.o -o $@

chsh: chsh.o
	${CC} ${LFLAG} $@.o -o $@

ckdir: ckdir.o
	${CC} ${LFLAG} $@.o -o $@

clock: clock.o
	${CC} ${LFLAG} $@.o -o $@

colcrt: colcrt.o
	${CC} ${LFLAG} $@.o -o $@

colrm: colrm.o
	${CC} ${LFLAG} $@.o -o $@

ctags: ctags.o
	${CC} ${LFLAG} $@.o -o $@

daytime: daytime.o
	${CC} ${LFLAG} $@.o -o $@

double: double.o
	${CC} ${LFLAG} $@.o -o $@

expand: expand.o
	${CC} ${LFLAG} $@.o -o $@

finger: finger.o
	${CC} ${LFLAG} $@.o -o $@

fleece: fleece.o
	${CC} ${LFLAG} $@.o -o $@

fold: fold.o
	${CC} ${LFLAG} $@.o -o $@

from: from.o
	${CC} ${LFLAG} $@.o -o $@

getNAME: getNAME.o
	${CC} ${LFLAG} $@.o -o $@

groups: groups.o
	${CC} ${LFLAG} $@.o -o $@

gets: gets.o
	${CC} ${LFLAG} $@.o -o $@

head: head.o
	${CC} ${LFLAG} $@.o -o $@

last: last.o
	${CC} ${LFLAG} $@.o -o $@

leave: leave.o
	${CC} ${LFLAG} $@.o -o $@

lock: lock.o
	${CC} ${LFLAG} $@.o -o $@

man: man.o
	${CC} ${LFLAG} $@.o -o $@

mkstr: mkstr.o
	${CC} ${LFLAG} $@.o -o $@

printenv: printenv.o
	${CC} ${LFLAG} $@.o -o $@

prmail: prmail.o
	${CC} ${LFLAG} $@.o -o $@

pti: pti.o
	${CC} ${LFLAG} $@.o -o $@

rewind: rewind.o
	${CC} ${LFLAG} $@.o -o $@

script: script.o
	${CC} ${LFLAG} $@.o -o $@

setquota: setquota.o
	${CC} ${LFLAG} $@.o -o $@

soelim: soelim.o
	${CC} ${LFLAG} $@.o -o $@

sq: sq.o
	${CC} ${LFLAG} $@.o -o $@

strings: strings.o
	${CC} ${LFLAG} $@.o -o $@

submit: submit.o
	${CC} ${LFLAG} $@.o -o $@

tail: tail.o
	${CC} ${LFLAG} $@.o -o $@

tod: tod.o
	${CC} ${LFLAG} $@.o -o $@

unexpand: unexpand.o
	${CC} ${LFLAG} $@.o -o $@

unifdef: unifdef.o
	${CC} ${LFLAG} $@.o -o $@

untmp: untmp.o
	${CC} ${LFLAG} $@.o -o $@

users: users.o
	${CC} ${LFLAG} $@.o -o $@

vmstat: vmstat.o
	${CC} ${LFLAG} $@.o -o $@

w: w.o
	${CC} ${LFLAG} $@.o -o $@

wc: wc.o
	${CC} ${LFLAG} $@.o -o $@

what: what.o
	${CC} ${LFLAG} $@.o -o $@

whereis: whereis.o
	${CC} ${LFLAG} $@.o -o $@

whoami: whoami.o
	${CC} ${LFLAG} $@.o -o $@

xstr: xstr.o
	${CC} ${LFLAG} $@.o -o $@

yes: yes.o
	${CC} ${LFLAG} $@.o -o $@

lastcomm: lastcomm.o
	${CC} ${LFLAG} $@.o -o $@

pq: pq.o
	${CC} ${LFLAG} $@.o -o $@

la: la.o
	${CC} ${LFLAG} $@.o -o $@

more: more.o
	${CC} ${LFLAG} $@.o -o $@ -ltermcap

ul: ul.o
	${CC} ${LFLAG} $@.o -o $@ -ltermcap

clear: clear.o
	${CC} ${LFLAG} $@.o -o $@ -ltermcap

sysline: sysline.o
	${CC} ${LFLAG} $@.o -o $@ -ltermcap -lcurses

rlogin: rlogin.o
	${CC} ${LFLAG} $@.o -o $@

rsh: rsh.o
	${CC} ${LFLAG} $@.o -o $@

ruptime: ruptime.o
	${CC} ${LFLAG} $@.o -o $@

telnet: telnet.c
	${CC} -S ${CFLAGS} telnet.c
	sed -f ../asm.sed telnet.s > telnet.S
	as - -o telnet.o telnet.S
	rm -f telnet.s telnet.S
	${CC} ${LFLAG} $@.o -o $@

rwho: rwho.o
	${CC} ${LFLAG} $@.o -o $@

.c.o:
	${CC} ${CFLAGS} ${FP} -c $*.c

arcv.o:		arcv.c
checknr.o:	checknr.c
chfn.o:		chfn.c
chsh.o:		chsh.c
ckdir.o:	ckdir.c
clock.o:	clock.c
colcrt.o:	colcrt.c
colrm.o:	colrm.c
ctags.o:	ctags.c
daytime.o:	daytime.c
double.o:	double.c
expand.o:	expand.c
finger.o:	finger.c
fleece.o:	fleece.c
fold.o:		fold.c
from.o:		from.c
getNAME.o:	getNAME.c
gets.o:		gets.c
groups.o:	groups.c
head.o:		head.c
last.o:		last.c
leave.o:	leave.c
lock.o:		lock.c
man.o:		man.c
mkstr.o:	mkstr.c
printenv.o:	printenv.c
prmail.o:	prmail.c
pti.o:		pti.c
rewind.o:	rewind.c
script.o:	script.c
setquota.o:	setquota.c
soelim.o:	soelim.c
sq.o:		sq.c
strings.o:	strings.c
submit.o:	submit.c
tail.o:		tail.c
tod.o:		tod.c
unexpand.o:	unexpand.c
unifdef.o:	unifdef.c
untmp.o:	untmp.c
users.o:	users.c
vmstat.o:	vmstat.c
w.o:		w.c
wc.o:		wc.c
what.o:		what.c
whereis.o:	whereis.c
whoami.o:	whoami.c
xstr.o:		xstr.c
yes.o:		yes.c
lastcomm.o:	lastcomm.c
pq.o:		pq.c
la.o:		la.c
more.o:		more.c
ul.o:		ul.c
clear.o:	clear.c
sysline.o:	sysline.c
rlogin.o:	rlogin.c
rsh.o:		rsh.c
ruptime.o:	ruptime.c
rwho.o:		rwho.c
