#
# Copyright (c) 1983 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.1 (Berkeley) 5/15/85
#
#
# makefile for line printer filters and associated programs
#

DESTDIR=
CFLAGS=	-O
SEPFLAG=-i
LIBDIR=	/usr/lib
CXREF=	/usr/ucb/ctags -x
#
# vpsf, fcvt and vdmp were excluded on the PDP because they require more than
# 64K of bss and I don't have time to figure out how it works.
#
SRCS=	lpf.c necf.c \
	chrtab.c rvsort.c rvcat.c vsort.c vcat.c vpf.c vpsf.c vdmp.c vpltdmp.c \
	vplotf.c vfontinfo.c vwidth.c vfw.c fcvt.c rotate.c rotprt.c
PRINTER=lpf necf
VARIAN=	rvsort rvcat vsort vcat vpf vpltdmp vplotf vfontinfo vwidth \
	vfw rotate rotprt

all:	printer varian

printer: ${PRINTER}

varian:	${VARIAN}

lpf:	lpf.c
	${CC} -o lpf ${CFLAGS} ${SEPFLAG} lpf.c

necf:	necf.c
	${CC} -o necf ${CFLAGS} ${SEPFLAG} necf.c

rvsort:	rvsort.c
	${CC} -o rvsort ${CFLAGS} ${SEPFLAG} rvsort.c

rvcat:	rvcat.c
	${CC} -o rvcat ${CFLAGS} ${SEPFLAG} rvcat.c

vsort:	vsort.c
	${CC} -o vsort ${CFLAGS} ${SEPFLAG} vsort.c

vcat:	vcat.c
	${CC} -o vcat ${CFLAGS} ${SEPFLAG} vcat.c

vpf:	vpf.c
	${CC} -o vpf ${CFLAGS} ${SEPFLAG} vpf.c

vpsf:	vpsf.c
	${CC} -o vpsf ${CFLAGS} ${SEPFLAG} vpsf.c

vdmp: vdmp.c
	${CC} -o vdmp ${CFLAGS} ${SEPFLAG} vdmp.c

vpltdmp: vpltdmp.c
	${CC} -o vpltdmp ${CFLAGS} ${SEPFLAG} vpltdmp.c

vplotf: vplotf.c chrtab.o
	${CC} -o vplotf ${CFLAGS} ${SEPFLAG} vplotf.c chrtab.o

vfontinfo: vfontinfo.c
	${CC} -o vfontinfo ${CFLAGS} ${SEPFLAG} vfontinfo.c

vwidth: vwidth.c
	${CC} -o vwidth ${CFLAGS} ${SEPFLAG} vwidth.c

vfw: vfw.c
	${CC} -o vfw ${CFLAGS} ${SEPFLAG} vfw.c

fcvt: fcvt.c
	${CC} -o fcvt ${CFLAGS} ${SEPFLAG} fcvt.c

rotate: rotate.c
	${CC} -o rotate ${CFLAGS} ${SEPFLAG} rotate.c

rotprt: rotprt.c
	${CC} -o rotprt ${CFLAGS} ${SEPFLAG} rotprt.c

install: printer
	install -s lpf ${DESTDIR}/${LIBDIR}/lpf
	install -s necf ${DESTDIR}/${LIBDIR}/necf

installv: varian
	install -s vfontinfo ${DESTDIR}/${LIBDIR}/vfontinfo
	install -s vwidth ${DESTDIR}/${LIBDIR}/vwidth
	install -s vfw ${DESTDIR}/${LIBDIR}/vfw
	install -s rotate ${DESTDIR}/${LIBDIR}/rotate
	install -s rotprt ${DESTDIR}/${LIBDIR}/rotprt
	install -s rvsort ${DESTDIR}/${LIBDIR}/rvsort
	install -s rvcat ${DESTDIR}/${LIBDIR}/rvcat
	install -s vsort ${DESTDIR}/${LIBDIR}/vsort
	install -s vcat ${DESTDIR}/${LIBDIR}/vcat
	install -s vpf ${DESTDIR}/${LIBDIR}/vpf
	ln ${DESTDIR}/${LIBDIR}/vpf ${DESTDIR}/${LIBDIR}/vpfW
	install -s vdmp ${DESTDIR}/${LIBDIR}/vdmp
	install -s vpltdmp ${DESTDIR}/${LIBDIR}/vpltdmp
	install -s vplotf ${DESTDIR}/${LIBDIR}/vplotf

#	install -s fcvt ${DESTDIR}/${LIBDIR}/fcvt
#	install -s vpsf ${DESTDIR}/${LIBDIR}/vpsf

clean:
	rm -f ${PRINTER} ${VARIAN} *.o

print:
	@pr makefile
	@${CXREF} *.c | pr -h XREF
	@pr *.h *.c
