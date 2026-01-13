#	@(#)Makefile	4.3	(2.11BSD) 2025/11/25
#
CFLAGS=-O
SRCS= pow.c gcd.c msqrt.c mdiv.c mout.c mult.c madd.c util.c
OBJS= pow.o gcd.o msqrt.o mdiv.o mout.o mult.o madd.o util.o
TAGSFILE=tags

libmp.a: $(OBJS)
	ar cr libmp.a $(OBJS)

install: libmp.a
	install -c -m 644 -o root libmp.a /usr/lib/libmp.a
	ranlib /usr/lib/libmp.a

tags:
	cwd=`pwd`; \
	for i in ${SRCS}; do \
		ctags -a -f ${TAGSFILE} $$cwd/$$i; \
	done

clean:
	rm -f ${OBJS} libmp.a tags
