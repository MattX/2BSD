ED=	-ed
AS=	-as
VERSION= version
EYACC=	/usr/new/eyacc
CFLAGS=	-O -DPXP
OBJS=	call.o case.o const.o cset.o \
	error.o fdec.o func.o hash.o lab.o lval.o \
	main.o nl.o pmon.o pp.o proc.o rec.o rval.o \
	stat.o string.o subr.o tree.o type.o var.o y.tab.o \
	yycomm.o yycosts.o yyerror.o yyget.o yyid.o yylex.o yymain.o \
	yypanic.o yyparse.o yyprint.o yyput.o yyrecover.o \
	yyseman.o yytree.o
OBJS2=	printf.o treen.o yycopy.o

a.out:	${OBJS} ${OBJS2}
	${CC} -c Version.c
	${CC} -i ${OBJS} ${OBJS2} Version.o \
	    ../pascal/fpterp/fp.o ../pascal/opcodes/TRdata.o ../px/getc.o ../px/putc.o

a.outNOID:
	${VERSION} > Version.c
	${CC} -c Version.c
	${CC} -n -f ${OBJS} Version.o ../fpdata/*.o ../opcodes/TRdata.o  \
	    -o a.outNOID

y.tab.c: pas.y
	${EYACC} pas.y
	${ED} - y.tab.c < gram
clean:
	rm pi1:2strings a.out core y.tab.c
	rm *.o x*
install:
	cp a.out /usr/bin/pxp
	chmod 711 /usr/bin/pxp
	chown root /usr/bin/pxp
installNOID:
	cp a.outNOID /usr/bin/pxp
	chmod 711 /usr/bin/pxp
	chown root /usr/bin/pxp
print:
	@pr makefile READ_ME
	@ls -ls | pr
	@size a.out *.o | pr
	-@cxref *.c | pr
	@pr 0.h main.c pas.y
	@pr yy.h yy*.c
	@pr [a-ln-x]*.c
	@pr putn.s treen.s yycopy.s
