#
CFLAGS=	-O
#
LIB =   # the pathname of lib directory

OBJS =  clrerr.o data.o doscan.o \
	exit.o fdopen.o fgetc.o fgets.o filbuf.o flsbuf.o fopen.o \
	fprintf.o fputc.o fputs.o freopen.o fseek.o ftell.o \
	getchar.o gets.o getw.o printf.o putchar.o puts.o \
	putw.o rdwr.o rew.o scanf.o setbuf.o setbuffer.o \
	sprintf.o strout.o ungetc.o \
	closedir.o opendir.o readdir.o seekdir.o \
	telldir.o scandir.o perror.o popen.o strcmpfold.o

.c.o:
	${CC} ${CFLAGS} -c $*.c
	-ld -x -r $*.o
	mv a.out $*.o

libra:  ${OBJS}
	ar cru $(LIB)/libra.a ${OBJS}
	ranlib $(LIB)/libra.a

#  The source codes of above functions are exactly the same as those
#  in /usr/src/lib/libc/stdio and /usr/src/lib/libc/gen, except
#  strcmpfold.c, which is listed in this directory.
#


