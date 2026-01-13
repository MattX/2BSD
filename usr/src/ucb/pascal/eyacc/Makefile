#       Makefile        2.0 (2.11BSD) 2025/11/25

CFLAGS=-O
LDFLAGS=-i

HDRS= ey.h
SRCS= ey0.c ey1.c ey2.c ey3.c ey4.c ey5.c
OBJS= ey0.o ey1.o ey2.o ey3.o ey4.o ey5.o

eyacc: ${OBJS}
	${CC} ${LDFLAGS} -o eyacc ${OBJS}

install:
	@echo "Eyacc is only used by pascal, we don't install it for general"
	@echo "use."

tags: FRC
	ctags ${HDRS} ${SRCS}

FRC:

clean:
	rm -f *.o eyacc tags

ey0.o:	ey0.c
ey1.o:	ey1.c ey.h
ey2.o:	ey2.c ey.h
ey3.o:	ey3.c ey.h
ey4.o:	ey4.c ey.h
ey5.o:	ey5.c
