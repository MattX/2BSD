h29756
s 00001/00000/00009
d D 2.2 80/09/02 13:55:05 ucb 2 1
c UCB version, SCCS id keyword added (mss)
e
s 00009/00000/00000
d D 2.1 80/09/02 13:46:54 ucb 1 0
c date and time created 80/09/02 13:46:54 by ucb
e
u
U
f b 
f n 
t
T
I 2
#	%W%	SCCS id keyword
E 2
I 1
CFLAGS=-O
OBJS=openlp.o openl.o ucbpath.o concat.o
libucbpath.a : ${OBJS}
	rm -f libucbpath.a
	ar r libucbpath.a ${OBJS}
	ranlib libucbpath.a

clean:
	rm -f ${OBJS}
E 1
