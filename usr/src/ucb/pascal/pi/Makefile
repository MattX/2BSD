BINDIR=	/usr/bin
LIBDIR=	/usr/lib
LIBS=	-lovm

HDRS=	0.h OPnames.h opcode.h tree.h yy.h

SRCS=	ato.c call.c case.c clas.c const.c conv.c cset.c error.c fdec.c \
	func.c gen.c hash.c lab.c lookup.c lval.c main.c nl.c proc.c put.c \
	rec.c rval.c stat.c string.c subr.c tree.c type.c var.c Version.c \
	TRdata.c treen.c putn.c yycopy.c yycosts.c yyerror.c yyget.c yyid.c \
	yylex.c yymain.c yyoptions.c yypanic.c yyparse.c yyprint.c yyput.c \
	yyrecover.c yyseman.c yytree.c

OBJS=	ato.o call.o case.o clas.o const.o conv.o cset.o error.o fdec.o \
	func.o gen.o hash.o lab.o lookup.o lval.o main.o nl.o proc.o put.o \
	rec.o rval.o stat.o string.o subr.o tree.o type.o var.o Version.o \
	TRdata.o treen.o putn.o yycopy.o y.tab.o yycosts.o yyerror.o yyget.o \
	yyid.o yylex.o yymain.o yyoptions.o yypanic.o yyparse.o yyprint.o \
	yyput.o yyrecover.o yyseman.o yytree.o

OTHERS=	pas.y gram

BASE=	main.o Version.o putn.o rec.o
OV1=	TRdata.o ato.o call.o case.o clas.o const.o conv.o cset.o stat.o
OV4=	rval.o proc.o
OV2=	gen.o hash.o lab.o lookup.o string.o subr.o tree.o treen.o var.o \
	nl.o put.o
OV5=	fdec.o func.o lval.o type.o
OV3=	yycopy.o yycosts.o yyerror.o yyget.o yyid.o yylex.o yymain.o \
	yyoptions.o yypanic.o yyparse.o yyprint.o yyput.o yyseman.o \
	yytree.o error.o
OV6=	y.tab.o yyrecover.o

MKSTR=	/usr/ucb/mkstr
EYACC=	/usr/ucb/eyacc
GET=	sccs get

CFLAGS=
LFLAG=	-n

TEMPDIR=	tmp
ERRORSTRINGS=	pi1.2strings

all: pix px_header.out pi

ovall: pix px_header.out ovpi

install:
	install -c -o bin -g bin -m 644 ${ERRORSTRINGS} ${DESTDIR}${LIBDIR}
	install -c -o bin -g bin -m 644 how_pi ${DESTDIR}${LIBDIR}
	install -c -o bin -g bin -m 644 how_pix ${DESTDIR}${LIBDIR}
	dd if=px_header.out of=${DESTDIR}${LIBDIR}/npx_header conv=sync
	install -s -o bin -g bin -m 711 pi ${DESTDIR}${BINDIR}
	install -s -o bin -g bin -m 711 pix ${DESTDIR}${BINDIR}

pix: pix.c
	${CC} ${LFLAG} -O -o pix pix.c

pi: ${OBJS} version
	./version > Version.c
	${CC} -o pi ${CFLAGS} ${FP} ${LFLAG} ${OBJS} Version.c

ovpi: ${OBJS} version
	./version > Version.c
	${CC} -c Version.c
	ld -n -X ${CRT} ${BASE} -Z ${OV1} -Z ${OV2} -Z ${OV3} \
	-Z ${OV4} -Z ${OV5} -Z ${OV6} -L ${LIBS} -lovc -o pi
	size pi

y.tab.h y.tab.c: pas.y
	${EYACC} pas.y >pas.eyacc.list
	ex - y.tab.c < gram

.c.o:
	rm -f ${TEMPDIR}/$*.c
	${MKSTR} - ${ERRORSTRINGS} ${TEMPDIR}/ $*.c
	cd ${TEMPDIR} ; ${CC} ${CFLAGS} -I.. -c $*.c ; mv $*.o ../$*.o
	rm -f ${TEMPDIR}/$*.c

version: version.c
	${CC} -o version version.c

clean:
	rm -f *.o ${TEMPDIR}/*.c y.tab.c y.tab.h ${ERRORSTRINGS} \
		pi core px_header.out *.list obj

px_header.out : px_header.c
	${CC} -s -O ${LFLAG} -o px_header.out px_header.c

print :
	@pr makefile READ_ME
	@ls -ls | pr
	@pr picture
	@pr 0.h whoami main.c pas.y
	@pr yy.h yy*.c
	@pr [a-ln-x]*.c

yy.h:		y.tab.h
Version.o:	Version.c
TRdata.o:	whoami 0.h
ato.o:		0.h whoami ato.c
call.o:		whoami 0.h tree.h opcode.h call.c
case.o:		whoami 0.h tree.h opcode.h case.c
clas.o:		whoami 0.h tree.h opcode.h clas.c
const.o:	whoami 0.h tree.h const.c
conv.o:		whoami 0.h opcode.h conv.c
cset.o:		whoami 0.h tree.h opcode.h cset.c
error.o:	whoami 0.h yy.h error.c
fdec.o:		whoami 0.h tree.h opcode.h fdec.c
func.o:		whoami 0.h tree.h func.c opcode.h
gen.o:		whoami 0.h tree.h opcode.h gen.c
hash.o:		whoami 0.h yy.h hash.c
lab.o:		whoami 0.h tree.h opcode.h lab.c
lookup.o:	whoami 0.h lookup.c
lval.o:		whoami 0.h tree.h opcode.h lval.c
main.o:		whoami 0.h yy.h main.c
nl.o:		whoami 0.h opcode.h nl.c
proc.o:		whoami 0.h tree.h opcode.h proc.c
put.o:		whoami opcode.h 0.h OPnames.h put.c
rec.o:		whoami 0.h tree.h opcode.h rec.c
rval.o:		whoami 0.h tree.h opcode.h rval.c
stat.o:		whoami 0.h tree.h opcode.h stat.c
string.o:	whoami 0.h send.h string.c
subr.o:		whoami 0.h subr.c
tree.o:		whoami 0.h tree.c
type.o:		whoami 0.h tree.h type.c
var.o:		whoami 0.h tree.h opcode.h var.c
yycopy.o:	0.h yy.h yycopy.c
yycosts.o:	whoami 0.h yy.h yycosts.c
yyerror.o:	whoami 0.h yy.h yyerror.c
yyget.o:	whoami 0.h yy.h yyget.c
yyid.o:		whoami 0.h yy.h yyid.c
yylex.o:	whoami 0.h yy.h yylex.c
yymain.o:	whoami 0.h yy.h yymain.c
yyoptions.o:	whoami 0.h yy.h yyoptions.c
yypanic.o:	whoami 0.h yy.h yypanic.c
yyparse.o:	whoami 0.h yy.h yyparse.c
yyprint.o:	whoami 0.h yy.h yyprint.c
yyput.o:	whoami 0.h tree.h yy.h yyput.c
yyrecover.o:	whoami 0.h yy.h yyrecover.c
yyseman.o:	whoami 0.h yy.h yyseman.c
yytree.o:	whoami 0.h tree.h yytree.c
