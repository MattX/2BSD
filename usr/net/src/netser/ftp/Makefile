#	@(#)Makefile	4.2	1/15/83
#
ALL=	ftp ftpd
DESTDIR=
INCLUDE = -I/usr/include
LIBC	= -lnet -lndir
CFLAGS=	-O -I. ${INCLUDE}  -DTCP4_1a -DMENLO_JCL -DUCB_NET -Dpdp11
TYPE=	-n
LD= ld

all:	${ALL}

ftp:	cmds.o cmdtab.o ftp.o main.o compat.o
	${CC} ${TYPE} cmds.o cmdtab.o ftp.o main.o compat.o -o ftp -ljobs ${LIBC}

ftp.o:	ftp.h ftp_var.h
cmds.o:	ftp.h ftp_var.h
cmdtab.o: ftp_var.h
ftp.o:	ftp_var.h
main.o: ftp_var.h

ftp.h: compat.h
	touch ftp.h
ftpvar.h: varpat.h
	touch ftpvar.h

ftpd:	ftpd.o ftpcmd.o vers.o glob.o compat.o fakedir.o
	${CC} ${TYPE} ftpd.o ftpcmd.o glob.o compat.o fakedir.o \
	 -o ftpd vers.o -ljobs ${LIBC}

vers.o:	ftpd.c ftpcmd.y
	sh newvers.sh
	${CC} -c vers.c

install:
	install -s ftp ${DESTDIR}/usr/ucb/ftp
	install -s ftpd ${DESTDIR}/etc/ftpd

clean:
	rm -f ${ALL} *.o errs core a.out t.? yacc.* y.tab.c
