#
# csu makefile
#

ALL=	crt0.o mcrt0.o
FALL=	fcrt0.o fmcrt0.o

csu:	${ALL}

fpcsu:	${FALL}

install: ${ALL}
	install -o bin -g bin -m 644 crt0.o ${DESTDIR}/lib
	install -o bin -g bin -m 644 mcrt0.o ${DESTDIR}/lib

fpinstall: ${FALL}
	install -o bin -g bin -m 644 fcrt0.o ${DESTDIR}/lib
	install -o bin -g bin -m 644 fmcrt0.o ${DESTDIR}/lib

clean:
	rm -f ${ALL} ${FALL}

.s.o:
	as - -o $*.o $*.s

crt0.o:		crt0.s
fcrt0.o:	fcrt0.s
fmcrt0.s:	fmcrt0.s
mcrt0.o:	mcrt0.s
