#	@(#)makefile	2.2	SCCS id keyword
CFLAGS=-O
OBJS=openlp.o openl.o ucbpath.o concat.o
libucbpath.a : ${OBJS}
	rm -f libucbpath.a
	ar r libucbpath.a ${OBJS}
	ranlib libucbpath.a

clean:
	rm -f ${OBJS}
