#
# refer/refer makefile
#

ALL=	refer
OVALL=	ovrefer
OVLIB=	/usr/lib/libovc.a

F1=	refer0.o refer1.o refer2.o refer4.o refer6.o refer7.o refer8.o
F2=	glue1.o glue2.o glue4.o glue5.o
F3=	hunt2.o hunt3.o hunt6.o hunt7.o hunt8.o mkey3.o deliv2.o \
	shell.o

all: ${ALL}

ovall: ${OVALL}

refer: ${F1} ${F2} ${F3} refer5.o
	${CC} ${LFLAG} -o ${ALL} ${F1} ${F2} ${F3} refer5.o

ovrefer: ${F1} ${F2} ${F3} refer5.o doscan.o
	ld -n -X ${CRT} -Z ${F1} -Z ${F2} refer5.o \
		-Z ${F3} doscan.o -L -lovc -o ${ALL}

install:
	install -s -o bin -g bin -m 711 refer ${DESTDIR}/usr/bin

clean:
	rm -f ${ALL} ${F1} ${F2} ${F3} refer5.o doscan.o

.c.o:
	${CC} -DMFLAG ${CFLAGS} -c $*.c

doscan.o:
	ar x ${OVLIB} doscan.o

refer0.o:	refer0.c refer.h
refer1.o:	refer1.c refer.h
refer2.o:	refer2.c refer.h
refer3.o:	refer3.c refer.h
refer4.o:	refer4.c refer.h
refer5.o:	refer5.c refer.h
refer6.o:	refer6.c refer.h
refer7.o:	refer7.c refer.h
refer8.o:	refer8.c refer.h
deliv2.o:	deliv2.c
glue1.o:	glue1.c
glue2.o:	glue2.c
glue4.o:	glue4.c
glue5.o:	glue5.c
hunt2.o:	hunt2.c
hunt3.o:	hunt3.c
hunt5.o:	hunt5.c
hunt6.o:	hunt6.c
hunt7.o:	hunt7.c
hunt8.o:	hunt8.c
hunt9.o:	hunt9.c
mkey3.o:	mkey3.c
shell.o:	shell.c
