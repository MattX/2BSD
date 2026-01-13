#       Makefile        2.0 (2.11BSD) 2025/11/25

LDFLAGS=-i
CFLAGS=	-O

HDRS=	link.h
SRCS=	in.c link.c pass2.c sup.c 
OBJS=	in.o link.o pass2.o sup.o 

l11: ${OBJS}
	cc -o $@ ${LDFLAGS} ${OBJS}

install: l11
	install -c -s l11 /usr/new/l11

cmp: l11
	cmp l11 /usr/new/l11

lint:	
	lint link.c pass2.c in.c sup.c > FLUFF

tags: ${HDRS} ${SRCS}
	ctags $<

clean:
	rm -f *.o l11 tags FLUFF

in.o:		in.c link.h
link.o:		link.c link.h
obint.o:	obint.c
pass2.o:	pass2.c link.h
sup.o:		sup.c link.h
