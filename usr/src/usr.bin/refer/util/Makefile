CFLAGS =-O

ALL=	mkey inv hunt

all:	${ALL}

install:
	install -s -o bin -g bin -m 711 inv ${DESTDIR}/usr/lib/refer
	install -s -o bin -g bin -m 711 hunt ${DESTDIR}/usr/lib/refer
	install -s -o bin -g bin -m 711 mkey ${DESTDIR}/usr/lib/refer

MKOBJ=	mkey1.o mkey2.o mkey3.o deliv2.o
INOBJ=	inv1.o inv2.o inv3.o inv5.o inv6.o deliv2.o
HNOBJ=	hunt1.o hunt2.o hunt3.o hunt5.o hunt6.o hunt7.o glue5.o \
	refer3.o hunt9.o shell.o deliv2.o hunt8.o glue4.o
DLOBJ=	deliv1.o deliv2.o
RFOBJ=	refer0.o refer1.o refer2.o refer3.o refer4.o refer5.o \
	refer6.o refer7.o refer8.o deliv2.o glue4.o
REOBJ=	glue1.o refer1.o refer2.o refer4.o refer5.o refer6.o mkey3.o \
	refer7.o refer8.o hunt2.o hunt3.o deliv2.o hunt5.o hunt6.o hunt8.o \
	glue3.o hunt7.o hunt9.o glue2.o glue4.o glue5.o refer0.o shell.o
WHOBJ=	what1.o what2.o what3.o what4.o shell.o mkey3.o

mkey: ${MKOBJ}
	${CC} ${LFLAG} ${MKOBJ} -o $@

inv: ${INOBJ}
	${CC} ${LFLAG} ${INOBJ} -o $@

hunt: ${HNOBJ}
	${CC} ${LFLAG} ${HNOBJ} -o $@

deliv: ${DLOBJ}
	${CC} ${LFLAG} ${DLOBJ} -o $@

refpart: ${RFOBJ}
	${CC} ${LFLAG} ${RFOBJ} -o $@

refer: ${REOBJ}
	${CC} ${LFLAG} ${REOBJ} -o $@

whatabout: ${WHOBJ}
	${CC} ${LFLAG} ${WHOBJ} -o $@

clean:
	rm -f *.o ${ALL}

deliv1.o:	deliv1.c
deliv2.o:	deliv2.c
flagger.o:	flagger.c
glue1.o:	glue1.c
glue2.o:	glue2.c
glue3.o:	glue3.c
glue4.o:	glue4.c
glue5.o:	glue5.c
hunt1.o:	hunt1.c
hunt2.o:	hunt2.c
hunt3.o:	hunt3.c
hunt5.o:	hunt5.c
hunt6.o:	hunt6.c
hunt7.o:	hunt7.c
hunt8.o:	hunt8.c
hunt9.o:	hunt9.c
inv1.o:		inv1.c
inv2.o:		inv2.c
inv3.o:		inv3.c
inv5.o:		inv5.c
inv6.o:		inv6.c
kaiser.o:	kaiser.c
mkey1.o:	mkey1.c
mkey2.o:	mkey2.c
mkey3.o:	mkey3.c
refer..o:	refer..c
refer0.o:	refer0.c refer..c
refer1.o:	refer1.c refer..c
refer2.o:	refer2.c refer..c
refer3.o:	refer3.c refer..c
refer4.o:	refer4.c refer..c
refer5.o:	refer5.c
refer6.o:	refer6.c refer..c
refer7.o:	refer7.c
refer8.o:	refer8.c
shell.o:	shell.c
thash.o:	thash.c
types.o:	types.c
what..o:	what..c
what1.o:	what1.c
what2.o:	what2.c
what3.o:	what3.c
what4.o:	what4.c
