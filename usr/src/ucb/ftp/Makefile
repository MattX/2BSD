#	@{#}Makefile	4.6	6/15/83
#
CFLAGS=	-O
ALL=	ftp
OBJ=	cmds.o cmdtab.o ftp.o getpass.o glob.o main.o

all:	${ALL}

ftp:	${OBJ}
	${CC} ${OBJ} ${LFLAG} -o ftp

install:
	install -s -o bin -g bin -m 755 ftp ${DESTDIR}/usr/ucb

clean:
	rm -f ${ALL} *.o *.s errs core a.out t.?

cmds.o:		ftp_var.h
cmdtab.o:	ftp_var.h
main.o:		ftp_var.h
