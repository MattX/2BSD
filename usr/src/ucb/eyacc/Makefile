#
# eyacc makefile
#

CFLAGS=	-O ${OV}

all: eyacc

ovall: oveyacc

install:
	install -s -o bin -g bin -m 711 eyacc ${DESTDIR}/usr/ucb

eyacc: ey0.o ey1.o ey2.o ey3.o ey4.o ey5.o
	${CC} ${LFLAG} -o eyacc ey0.o ey1.o ey2.o ey3.o ey4.o ey5.o

oveyacc: ey0.o ey1.o ey2.o ey3.o ey4.o ey5.o
	ld ${LFLAG} -o eyacc ${CRT} ey0.o -Z ey1.o ey3.o \
	-Z ey2.o ey4.o -L ey5.o -lovc

clean:
	rm -f *.o eyacc

print:
	@ls -l | pr
	@pr READ_ME makefile
	@pr ey.h ey0.c ey1.c ey2.c ey3.c ey4.c ey5.c

ey0.o:	ey0.c
ey1.o:	ey.h ey1.c
ey2.o:	ey.h ey2.c
ey3.o:	ey.h ey3.c
ey4.o:	ey.h ey4.c
ey5.o:	ey5.c
