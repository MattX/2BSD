#
# struct makefile
#

CFLAGS=	-O
YFLAGS=	-d
ALL=	structure beautify
OVALL=	ovstructure beautify
BOBJ=	beauty.o tree.o lextab.o bdef.o
F1=	0.alloc.o 0.args.o 0.def.o 0.extr.o 0.graph.o 0.list.o \
	0.parts.o 0.string.o
F2=	1.finish.o 1.form.o 1.fort.o 1.hash.o 1.init.o 1.line.o \
	1.main.o 1.node.o 1.recog.o 1.tables.o
F3=	2.dfs.o 2.dom.o 2.head.o 2.inarc.o 2.main.o 2.tree.o
F4=	3.branch.o 3.flow.o 3.loop.o 3.main.o 3.reach.o 3.then.o
F5=	4.brace.o 4.form.o 4.main.o 4.out.o
F6=	1.finish.o 1.form.o 1.init.o 1.line.o 1.main.o 1.node.o 1.tables.o
F7=	1.fort.o 1.hash.o 1.recog.o

all: ${ALL}

ovall: ${OVALL}

install:
	-mkdir ${DESTDIR}/usr/lib/struct
	-mkdir ${DESTDIR}/usr/bin/struct
	chmod 755 ${DESTDIR}/usr/lib/struct ${DESTDIR}/usr/bin/struct
	install -s -o bin -g bin -m 711 structure ${DESTDIR}/usr/lib/struct
	install -s -o bin -g bin -m 711 beautify ${DESTDIR}/usr/lib/struct
	install -c -o bin -g bin -m 755 SHELL ${DESTDIR}/usr/bin/struct

clean:
	rm -f main.o ${F1} ${F2} ${F3} ${F4} ${F5} y.tab.h beauty.c ${ALL}

structure: main.o ${F1} ${F2} ${F3} ${F4} ${F5}
	${CC} ${LFLAG} ${SOBJ} -o $@

ovstructure: main.o ${F1} ${F2} ${F3} ${F4} ${F5}
	ld ${LFLAG} -X ${CRT} main.o -Z ${F4} -Z ${F3} ${F6} \
		-Z ${F7} -Z ${F1} -L ${F5} -lovc -o structure

beautify: ${BOBJ}
	${CC} ${LFLAG} ${BOBJ} -o $@ -lln

.c.o:
	${CC} ${CFLAGS} -c $*.c

lextab.o: lextab.l y.tab.h
	lex lextab.l
	mv lex.yy.c lextab.c
	${CC} ${CFLAGS} -c lextab.c

beauty.o: b.h beauty.y
	yacc beauty.y
	mv y.tab.c beauty.c
	${CC} ${CFLAGS} -c beauty.c

0.alloc.o:	0.alloc.c def.h
0.args.o:	0.args.c def.h
0.def.o:	0.def.c def.h
0.extr.o:	0.extr.c def.h
0.graph.o:	0.graph.c def.h
0.list.o:	0.list.c def.h
0.parts.o:	0.parts.c def.h
0.string.o:	0.string.c def.h
1.finish.o:	1.finish.c 1.defs.h 1.incl.h def.h
1.form.o:	1.form.c 1.defs.h 1.incl.h def.h
1.fort.o:	1.fort.c 1.defs.h 1.incl.h def.h
1.hash.o:	1.hash.c 1.defs.h 1.incl.h def.h
1.init.o:	1.init.c 1.defs.h 1.incl.h def.h
1.line.o:	1.line.c 1.defs.h 1.incl.h def.h
1.main.o:	1.main.c 1.defs.h 1.incl.h def.h
1.node.o:	1.node.c 1.defs.h 1.incl.h def.h
1.recog.o:	1.recog.c 1.defs.h 1.incl.h def.h
1.tables.o:	1.tables.c 1.defs.h 1.incl.h def.h
2.dfs.o:	2.dfs.c 2.def.h def.h
2.dom.o:	2.dom.c 2.def.h def.h
2.head.o:	2.head.c 2.def.h def.h
2.inarc.o:	2.inarc.c 2.def.h def.h
2.main.o:	2.main.c 2.def.h def.h
2.test.o:	2.test.c 2.def.h def.h
2.tree.o:	2.tree.c 2.def.h def.h
3.branch.o:	3.branch.c 3.def.h def.h
3.flow.o:	3.flow.c 3.def.h def.h
3.loop.o:	3.loop.c 3.def.h def.h
3.main.o:	3.main.c 3.def.h def.h
3.reach.o:	3.reach.c 3.def.h def.h
3.test.o:	3.test.c 3.def.h def.h
3.then.o:	3.then.c 3.def.h def.h
4.brace.o:	4.brace.c 4.def.h def.h
4.form.o:	4.form.c 4.def.h def.h
4.main.o:	4.main.c 4.def.h def.h
4.out.o:	4.out.c 4.def.h def.h
bdef.o:		bdef.c def.h
main.o:		main.c def.h
tree.o:		tree.c y.tab.h
y.tab.h:	beauty.y beauty.c
