#
# nroff/troff makefile
#
# CFLAGS=	"-O -DNROFF" for nroff

CFLAGS=	-O
NOBJ=	n1.o n2.o n3.o n4.o n5.o n6.o n7.o n8.o \
	n9.o n10.o ni.o nii.o ntab.o hytab.o suftab.o
TOBJ=	n1.o n2.o n3.o n4.o n5.o t6.o n7.o n8.o n9.o \
	t10.o ni.o nii.o tab3.o hytab.o suftab.o

nroff: clean ${NOBJ}
	${CC} ${LFLAG} -o $@ ${NOBJ}

troff: clean ${TOBJ}
	${CC} ${LFLAG} -o $@ ${TOBJ}

install: ${ALL}
	install -s -o bin -g bin -m 711 nroff ${DESTDIR}/usr/bin
	install -s -o bin -g bin -m 711 troff ${DESTDIR}/usr/bin

clean:
	rm -f ${NOBJ} ${TOBJ}

.c.o:
	${CC} ${CFLAGS} -c $*.c

n1.o:	tdef.h d.h v.h tw.h s.h n1.c
n2.o:	tdef.h d.h v.h tw.h s.h n2.c
n3.o:	tdef.h d.h v.h tw.h s.h n3.c
n4.o:	tdef.h d.h v.h tw.h s.h n4.c
n5.o:	tdef.h d.h v.h tw.h s.h n5.c
n6.o:	tdef.h d.h v.h tw.h s.h n6.c
t6.o:	tdef.h d.h v.h tw.h s.h t6.c
n7.o:	tdef.h d.h v.h tw.h s.h n7.c
n8.o:	tdef.h d.h v.h tw.h s.h n8.c
n9.o:	tdef.h d.h v.h tw.h s.h n9.c
n10.o:	tdef.h d.h v.h tw.h s.h n10.c
t10.o:	tdef.h d.h v.h tw.h s.h t10.c
ni.o:	tdef.h d.h v.h tw.h s.h ni.c
nii.o:	tdef.h d.h v.h tw.h s.h nii.c
tab3.o:	tab3.c
ntab.o:	ntab.c

hytab.o: hytab.c
	${CC} -S hytab.c
	ed hytab.s <textscript
	as -o hytab.o hytab.s
	rm hytab.s

suftab.o: suftab.c
	${CC} -S suftab.c
	ed suftab.s <textscript
	as -o suftab.o suftab.s
	rm suftab.s
