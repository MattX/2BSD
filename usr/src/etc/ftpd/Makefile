#	@{#}Makefile	4.6	6/15/83
#
CFLAGS=	-O
ALL=	ftpd

all:	${ALL}

ftpd:	ftpd.o ftpcmd.o glob.o vers.o
	${CC} ${LFLAG} -o ftpd ftpd.o ftpcmd.o glob.o vers.o

vers.o:	ftpd.c ftpcmd.y
	sh newvers.sh
	${CC} -c vers.c

install: all
	install -s -o bin -g bin -m 755 ftpd ${DESTDIR}/etc

clean:
	rm -f ${ALL} *.o *.s errs core a.out t.?
