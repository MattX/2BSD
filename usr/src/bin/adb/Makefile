#
# makefile for adb
#
# FP should be -f for machines without floating point

CFLAGS=	-O
FP=
OV1=	input.o main.o command.o setup.o expr.o
OV2=	format.o print.o findfn.o
OV3=	output.o opset.o pcs.o
BASE=	access.o sym.o runpcs.o message.o

all:	adb

ovall:	ovadb

install:
	install -s -o bin -g bin -m 711 adb ${DESTDIR}/bin

adb: ${OV1} ${OV2} ${OV3} ${BASE}
	${CC} ${LFLAG} -o adb ${OV1} ${OV2} ${OV3} ${BASE}

ovadb: ${OV1} ${OV2} ${OV3} ${BASE}
	ld -X -n -o adb ${CRT} \
	-Z ${OV1} -Z ${OV2} -Z ${OV3} -L ${BASE} -lovc

clean:
	rm -f *.o ${ALL}

.c.o:
	${CC} ${CFLAGS} ${FP} ${OV} -c $*.c

access.o:	defs.h access.c
command.o:	defs.h command.c
expr.o:		defs.h expr.c
findrtn.o:	defs.h findrtn.c
findfn.o:	findfn.c
format.o:	defs.h format.c
input.o:	defs.h input.c
main.o:		defs.h main.c
message.o:	mac.h mode.h message.c
opset.o:	opset.c
output.o:	defs.h output.c
pcs.o:		defs.h pcs.c
print.o:	defs.h print.c
runpcs.o:	defs.h runpcs.c
setup.o:	defs.h setup.c
sym.o:		defs.h sym.c
