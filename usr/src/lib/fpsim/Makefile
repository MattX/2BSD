#
# libfpsim makefile
#

SRC1=	fp1.s fp2.s fp3.s fpx.s
SRC2=	fp1_sep.s fp2.s fp3.s fpx_sep.s

all: libfpsim.a libfpsim_sep.a

libfpsim.a: ${SRC1}
	as -o $@ ${SRC1}

libfpsim_sep.a: ${SRC2}
	as -o $@ ${SRC2}

install:
	install -o bin -g bin -m 644 libfpsim.a ${DESTDIR}/lib
	install -o bin -g bin -m 644 libfpsim_sep.a ${DESTDIR}/lib

clean:
	rm -f libfpsim.a libfpsim_sep.a

