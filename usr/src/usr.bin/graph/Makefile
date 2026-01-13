#	Makefile	4.2	(2.11BSD) 2025/11/25
#
CFLAGS=-O
LDFLAGS= -i

graph: graph.c
	${CC} ${LDFLAGS} $(CFLAGS) -o graph graph.c -lplot -lm

install: graph
	install -c -s -m 755 graph /usr/bin/graph

clean:
	rm -f *.o graph
