#
# Copyright (c) 1983 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.1.2 (2.11BSD) 2025/11/25
#

CFLAGS=	-O
LDFLAGS=-i
LIBEXECDIR=	/usr/libexec/lpr
CXREF=	/usr/ucb/ctags -x

# vpsf, fcvt and vdmp were excluded on the PDP because they require more than
# 64K of bss and I don't have time to figure out how it works.

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
	${CC} -o lpf ${CFLAGS} ${LDFLAGS} lpf.c

necf:	necf.c
	${CC} -o necf ${CFLAGS} ${LDFLAGS} necf.c

rvsort:	rvsort.c
	${CC} -o rvsort ${CFLAGS} ${LDFLAGS} rvsort.c

rvcat:	rvcat.c
	${CC} -o rvcat ${CFLAGS} ${LDFLAGS} rvcat.c

vsort:	vsort.c
	${CC} -o vsort ${CFLAGS} ${LDFLAGS} vsort.c

vcat:	vcat.c
	${CC} -o vcat ${CFLAGS} ${LDFLAGS} vcat.c

vpf:	vpf.c
	${CC} -o vpf ${CFLAGS} ${LDFLAGS} vpf.c

vpsf:	vpsf.c
	${CC} -o vpsf ${CFLAGS} ${LDFLAGS} vpsf.c

vdmp: vdmp.c
	${CC} -o vdmp ${CFLAGS} ${LDFLAGS} vdmp.c

vpltdmp: vpltdmp.c
	${CC} -o vpltdmp ${CFLAGS} ${LDFLAGS} vpltdmp.c

vplotf: vplotf.c chrtab.o
	${CC} -o vplotf ${CFLAGS} ${LDFLAGS} vplotf.c chrtab.o

vfontinfo: vfontinfo.c
	${CC} -o vfontinfo ${CFLAGS} ${LDFLAGS} vfontinfo.c

vwidth: vwidth.c
	${CC} -o vwidth ${CFLAGS} ${LDFLAGS} vwidth.c

vfw: vfw.c
	${CC} -o vfw ${CFLAGS} ${LDFLAGS} vfw.c

fcvt: fcvt.c
	${CC} -o fcvt ${CFLAGS} ${LDFLAGS} fcvt.c

rotate: rotate.c
	${CC} -o rotate ${CFLAGS} ${LDFLAGS} rotate.c

rotprt: rotprt.c
	${CC} -o rotprt ${CFLAGS} ${LDFLAGS} rotprt.c

install: printer
	-mkdir -p ${LIBEXECDIR}
	install -c -s -m 755 lpf ${LIBEXECDIR}/lpf
	install -c -s -m 755 necf ${LIBEXECDIR}/necf

installv: varian
	-mkdir -p ${LIBEXECDIR}
	install -c -s -m 755 vfontinfo ${LIBEXECDIR}/vfontinfo
	install -c -s -m 755 vwidth ${LIBEXECDIR}/vwidth
	install -c -s -m 755 vfw ${LIBEXECDIR}/vfw
	install -c -s -m 755 rotate ${LIBEXECDIR}/rotate
	install -c -s -m 755 rotprt ${LIBEXECDIR}/rotprt
	install -c -s -m 755 rvsort ${LIBEXECDIR}/rvsort
	install -c -s -m 755 rvcat ${LIBEXECDIR}/rvcat
	install -c -s -m 755 vsort ${LIBEXECDIR}/vsort
	install -c -s -m 755 vcat ${LIBEXECDIR}/vcat
	install -c -s -m 755 vpf ${LIBEXECDIR}/vpf
	ln ${LIBEXECDIR}/vpf ${LIBEXECDIR}/vpfW
	install -c -s -m 755 vpltdmp ${LIBEXECDIR}/vpltdmp
	install -c -s -m 755 vplotf ${LIBEXECDIR}/vplotf
#	install -c -s -m 755 vdmp ${LIBEXECDIR}/vdmp
#	install -c -s -m 755 fcvt ${LIBEXECDIR}/fcvt
#	install -c -s -m 755 vpsf ${LIBEXECDIR}/vpsf

clean:
	rm -f ${PRINTER} ${VARIAN} *.o

print:
	@pr makefile
	@${CXREF} *.c | pr -h XREF
	@pr *.h *.c
