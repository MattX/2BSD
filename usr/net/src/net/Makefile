# @(#)Makefile	4.3 10/12/82
ALL=	common inet

all:
	for i in ${ALL}; do (cd $$i; make); done
	cc -c -O bcopy.c
	cc -c -O bcmp.c
	cc -c -O insque.c
	as - -o bzero.o bzero.s
	as - -o netsys.o /usr/include/sys.s netsys.s
	ar rv netlib bcopy.o bzero.o insque.o bcmp.o netsys.o
	ranlib netlib

clean:
	rm -f netlib netlib_p
	for i in ${ALL}; do (cd $$i; make clean); done
