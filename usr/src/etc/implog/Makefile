ALL	= implog implogd
CFLAGS	= -O -DUCB_NET

all:	${ALL}

implog:	implog.o
	${CC} ${LFLAG} implog.o -o implog

implogd: implogd.o
	${CC} ${LFLAG} implogd.o -o implogd

clean:
	rm -f ${ALL} *.o *.s errs core a.out t.?

install: ${ALL}
	install -s -o bin -g bin -m 755 implog ${DESTDIR}/etc
	install -s -o bin -g bin -m 755 implogd ${DESTDIR}/etc
