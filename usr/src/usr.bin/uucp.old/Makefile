#	$Header: Makefile,v 1.26 85/05/20 20:03:24 rick Exp $
#	from: Makefile	5.5	83/07/18
#
# This version expects the 4.2BSD directory reading routines to
# located in libc.a.  If they aren't, augment LIBS.  Also, LIBS
# can be set to reference other object modules and libraries needed
# by uucico, such as -lunet (needed when UNET is defined).
#
# Common object files are assembled into a library {uulib.a}
#
ALL=	uucico uucp uux uuxqt uulog uuclean uuname uupoll uusnap \
	uuencode uudecode uusend uucpd uuq acucntrl
# Common object files placed in uulib.a
COMMON=	anlwrk.o anyread.o assert.o cfgets.o chkpth.o \
	cpmv.o expfile.o gename.o getargs.o getprm.o \
	getpwinfo.o gnamef.o gnxseq.o lastpart.o logent.o mailst.o \
	prefix.o setline.o strpbrk.o subdir.o sysacct.o systat.o \
	ulockf.o uucpname.o versys.o xqt.o uucpdefs.o\
	mkdir.o getwd.o
SRCS=	${USRCS} ${OSRCS} ${LSRCS}
USRCS=	cico.c cntrl.c conn.c condevs.c gnsys.c pk0.c chksum.c pk1.c tio.c \
	fio.c gio.c imsg.c 
OSRCS=	uuclean.c uucp.c uulog.c uuname.c uupoll.c \
	uusend.c uusnap.c uux.c uuxqt.c uuq.c acucntrl.c
LSRCS=	anlwrk.c anyread.c assert.c cfgets.c chkpth.c \
	cpmv.c expfile.c \
	gename.c getargs.c getprm.c getpwinfo.c \
	gnamef.c gnxseq.c \
	lastpart.c logent.c mailst.c prefix.c \
	setline.c strpbrk.c subdir.c sysacct.c systat.c ulockf.c \
	uucpdefs.c uucpname.c versys.c xqt.c

#These might be useful on non 4.2bsd systems.
MISC= htonl.c ioctl.c index.c mkdir.c getwd.c

DESTDIR=

OWNER=	uucp
GROUP=	uucp
CFLAGS=	-O
LFLAG=
LINTFLAGS= -chbap
LIBS=
LIBDIR=	${DESTDIR}/usr/lib/uucp
BIN=	${DESTDIR}/usr/bin
PUBDIR=	${DESTDIR}/usr/spool/uucppublic
SPOOL=	${DESTDIR}/usr/spool/uucp
XQTDIR=	${SPOOL}/XTMP
CORRUPT= ${SPOOL}/CORRUPT
AUDIT=	${SPOOL}/AUDIT
LCK= ${SPOOL}/LCK
LOG= ${SPOOL}/LOG
STST=	${SPOOL}/STST
# The six subdirs {not counting XTMP}.
SUBDIRS=C. D.`uuname -l`X D.`uuname -l` D. X. TM.
# for make depend
UNDEF=	-UNDIR -UUNET -USYSIII -UUNAME -UCCWHOAMI -UGETMYHNAME -UDATAKIT \
	-UBSD4_2 -UUSG -UVMS

all:	${ALL} 

lint: ${SRCS}
	lint ${LINTFLAGS} ${USRCS} ${LSRCS}
	-for i in ${OSRCS} ; do \
		lint ${LINTFLAGS} $$i ${LSRCS} ;\
	done
print:	$(SRCS)
	vgrind $?
	vgrind -x index
	touch print

uucp:	uucp.o uulib.a
	${CC} -o uucp ${LFLAG} uucp.o uulib.a ${LIBS}

uux:	uux.o uulib.a
	${CC} -o uux ${LFLAG} uux.o uulib.a ${LIBS}

uuxqt:	uuxqt.o uulib.a
	${CC} -o uuxqt ${LFLAG} uuxqt.o uulib.a ${LIBS}

uucico:	cico.o cntrl.o conn.o condevs.o gnsys.o pk0.o chksum.o pk1.o tio.o \
	fio.o gio.o imsg.o uulib.a aculib/aculib.a
	${CC} -o uucico ${LFLAG} cico.o cntrl.o conn.o condevs.o gnsys.o \
	pk0.o chksum.o pk1.o tio.o fio.o gio.o imsg.o \
	uulib.a aculib/aculib.a ${LIBS}

uulog:	uulog.o uulib.a
	${CC} -o uulog ${LFLAG} uulog.o uulib.a ${LIBS}

uuclean: uuclean.o uulib.a
	${CC} -o uuclean ${LFLAG} uuclean.o uulib.a ${LIBS}

uuname:	uuname.o uulib.a
	${CC} -o uuname ${LFLAG} uuname.o uulib.a ${LIBS}

uupoll:	uupoll.o uulib.a
	${CC} -o uupoll ${LFLAG} uupoll.o uulib.a ${LIBS}

uusnap:	uusnap.o
	${CC} -o uusnap ${LFLAG} uusnap.o ${LIBS}

uuencode: uuencode.o
	${CC} -o uuencode ${LFLAG} uuencode.o

uudecode: uudecode.o
	${CC} -o uudecode ${LFLAG} uudecode.o

uusend: uusend.o
	${CC} -o uusend ${LFLAG} uusend.o uulib.a

uucpd: uucpd.o uucp.h
	${CC} -o uucpd ${LFLAG} uucpd.o ${LIBS}

uuq: uuq.o uulib.a
	${CC} -o uuq ${LFLAG} uuq.o uulib.a ${LIBS}

acucntrl: acucntrl.o
	${CC} -o acucntrl ${LFLAG} acucntrl.o

# Note: mode 700 on ${LIBDIR} would be safer.
# Also,  L.sys etc should be 600.

mkdirs:
	-for i in ${LIBDIR} ${SPOOL} ${PUBDIR} ${XQTDIR} ${CORRUPT} \
		${AUDIT} ${LCK} ${LOG} ${STST} ; do \
		rm -f $$i ; mkdir $$i ; \
		chmod 755 $$i; chown ${OWNER} $$i; chgrp ${GROUP} $$i ; \
	done
	chmod 777 ${PUBDIR} ${LCK}
	-cd ${LOG}; for i in uucico uux uuxqt uucp xferstats; do\
		rm -f $$i ; mkdir $$i ; \
		chmod 755 $$i; chown ${OWNER} $$i; chgrp ${GROUP} $$i ; \
	done
	-cd ${SPOOL}; for i in ${SUBDIRS} ; do \
		mkdir $$i; \
		chown ${OWNER} $$i; chgrp ${GROUP} $$i; chmod 755 $$i; \
	done

# The lorder...tsort below is for systems w/o ranlib
# But it doesnt hurt to use it. Ignore the spurious "cycle" messages
uulib.a: ${COMMON}
	rm -f $@
	ar cr $@ `lorder ${COMMON} | tsort`
	ranlib $@

aculib/aculib.a:
	cd aculib; make ${MFLAGS} "CFLAGS=${CFLAGS}"

install: ${ALL}
	chown ${OWNER} ${LIBDIR}
	chgrp ${GROUP} ${LIBDIR}
	chmod 755 ${LIBDIR}
	for i in uucico uuclean; do \
		install -s -m 6111 -o ${OWNER} -g ${GROUP} $$i ${LIBDIR} ;\
	done
	for i in uucp uux uulog uuname uusnap uupoll uuq uusend; do \
		install -s -m 6111 -o ${OWNER} -g ${GROUP} $$i ${BIN} ;\
	done
	rm -f ${BIN}/ruusend
	ln ${BIN}/uusend ${BIN}/ruusend
	install -s -m 6110 -o ${OWNER} -g ${GROUP} uuxqt ${LIBDIR}
	install -s -m 6110 -o root -g ${GROUP} acucntrl ${LIBDIR}
	install -s -m 700 -o root uucpd /etc/uucpd
	@echo Dont forget to fork off /etc/uucpd in /etc/rc.local
	@echo if you are not running /etc/inetd
	install -s uuencode ${BIN}
	install -s uudecode ${BIN}

clean:
	rm -f *.o ${ALL} uulib.a
	cd aculib; make ${MFLAGS} clean

depend:
	for i in *.c; do \
		(echo `basename $$i .c`.o: $$i >>makedep; \
			unifdef ${UNDEF} $$i | \
			/bin/grep '^#[ 	]*include' | sed \
			-e '/\.\.\/h/d' \
			-e 's,<\(.*\)>,"/usr/include/\1",' \
			-e 's/^[^"]*"\([^"]*\)".*/'"$$i"': \1/' \
			-e 's/\.c/.o/' >>makedep); done
	echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp Makefile Makefile.bak
	ed - Makefile < eddep
	rm eddep makedep
	echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
	echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
	echo '# see make depend above' >> Makefile

.c.o:
	${CC} ${CFLAGS} -c $*.c

# DO NOT DELETE THIS LINE -- make depend uses it
# DEPENDENCIES MUST END AT END OF FILE
acucntrl.o: acucntrl.c
acucntrl.o: uucp.h
anlwrk.o: anlwrk.c
anlwrk.o: uucp.h
anlwrk.o: uust.h
anyread.o: anyread.c
anyread.o: uucp.h
assert.o: assert.c
assert.o: uucp.h
cfgets.o: cfgets.c
chkpth.o: chkpth.c
chkpth.o: uucp.h
chksum.o: chksum.c
cico.o: cico.c
cico.o: uucp.h
cico.o: uust.h
cico.o: uusub.h
cntrl.o: cntrl.c
cntrl.o: uucp.h
cntrl.o: uust.h
condevs.o: condevs.c
condevs.o: condevs.h uucp.h
conn.o: conn.c
conn.o: uucp.h
cpmv.o: cpmv.c
cpmv.o: uucp.h
expfile.o: expfile.c
expfile.o: uucp.h
fio.o: fio.c
fio.o: uucp.h
gename.o: gename.c
gename.o: uucp.h
getargs.o: getargs.c
getargs.o: uucp.h
getprm.o: getprm.c
getpwinfo.o: getpwinfo.c
getpwinfo.o: uucp.h
gio.o: gio.c
gio.o: uucp.h
gio.o: pk.h
gnamef.o: gnamef.c
gnamef.o: uucp.h
gnsys.o: gnsys.c
gnsys.o: uucp.h
gnxseq.o: gnxseq.c
gnxseq.o: uucp.h
imsg.o: imsg.c
imsg.o: uucp.h
index.o: index.c
ioctl.o: ioctl.c
ioctl.o: uucp.h
lastpart.o: lastpart.c
logent.o: logent.c
logent.o: uucp.h
mailst.o: mailst.c
mailst.o: uucp.h
mkdir.o: mkdir.c
pk0.o: pk0.c
pk0.o: uucp.h
pk0.o: pk.h
pk1.o: pk1.c
pk1.o: uucp.h
pk1.o: pk.h
prefix.o: prefix.c
setline.o: setline.c
setline.o: uucp.h
subdir.o: subdir.c
subdir.o: uucp.h
sysacct.o: sysacct.c
systat.o: systat.c
systat.o: uucp.h
tio.o: tio.c
tio.o: uucp.h
ulockf.o: ulockf.c
ulockf.o: uucp.h
uuclean.o: uuclean.c
uuclean.o: uucp.h
uucp.o: uucp.c
uucp.o: uucp.h
uucp.o: uust.h
uucpd.o: uucpd.c
uucpd.o: uucp.h
uucpdefs.o: uucpdefs.c
uucpdefs.o: uucp.h
uucpname.o: uucpname.c
uucpname.o: uucp.h
uudecode.o: uudecode.c
uuencode.o: uuencode.c
uulog.o: uulog.c
uulog.o: uucp.h
uuname.o: uuname.c
uuname.o: uucp.h
uupoll.o: uupoll.c
uupoll.o: uucp.h
uuq.o: uuq.c
uuq.o: uucp.h
uusend.o: uusend.c
uusnap.o: uusnap.c
uusnap.o: uucp.h
uux.o: uux.c
uux.o: uucp.h
uuxqt.o: uuxqt.c
uuxqt.o: uucp.h
versys.o: versys.c
versys.o: uucp.h
xqt.o: xqt.c
xqt.o: uucp.h
getwd.o: getwd.c
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
