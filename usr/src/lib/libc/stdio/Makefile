#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.4.2 (2.11BSD) 2025/11/29

CFLAGS=	-O ${DEFS}

SRCS=	clrerr.c exit.c fclose.c fdopen.c findfp.c fflush.c fopen.c \
	fprintf.c fpurge.c fread.c freopen.c fseek.c ftell.c fwrite.c fscanf.c \
	fvwrite.c getc.c getchar.c getw.c makebuf.c rget.c fputc.c \
	printf.c putc.c putchar.c putw.c rewind.c scanf.c setbuf.c setbuffer.c \
	setvbuf.c snprintf.c sscanf.c stdio.c tmpfile.c tmpnam.c tempnam.c \
	sprintf.c ungetc.c vfprintf.c vfscanf.c vprintf.c vscanf.c fpfcom.c \
	vsnprintf.c vsprintf.c wbuf.c flags.c refill.c wsetup.c pfcom.c acvt.c \
	puts.c fputs.c fgetc.c fgets.c gets.c
OBJS=	clrerr.o exit.o fclose.o fdopen.o findfp.o fflush.o fopen.o \
	fprintf.o fpurge.o fread.o freopen.o fseek.o ftell.o fwrite.o fscanf.o \
	fvwrite.o getc.o getchar.o getw.o makebuf.o rget.o fputc.o \
	printf.o putc.o putchar.o putw.o rewind.o scanf.o setbuf.o setbuffer.o \
	setvbuf.o snprintf.o sscanf.o stdio.o tmpfile.o tmpnam.o tempnam.o \
	sprintf.o ungetc.o vfprintf.o vfscanf.o vprintf.o vscanf.o fpfcom.o \
	vsnprintf.o vsprintf.o wbuf.o flags.o refill.o wsetup.o pfcom.o acvt.o \
	puts.o fputs.o fgetc.o fgets.o gets.o
TAGSFILE=tags

.c.o:
	${CC} -p ${CFLAGS} -c $*.c
	-ld -X -r $*.o
	mv a.out profiled/$*.o
	${CC} ${CFLAGS} -c $*.c
	-ld -x -r $*.o
	mv a.out $*.o

stdiolib stdiolib_p: ${OBJS}
	@echo "building profiled stdiolib"
	@cd profiled; ar cru ../stdiolib_p ${OBJS}
	@echo "building normal stdiolib"
	@ar cru stdiolib ${OBJS}

tags:
	cwd=`pwd`; \
	for i in ${SRCS}; do \
		ctags -a -f ${TAGSFILE} $$cwd/$$i; \
	done

clean:
	rm -f stdiolib stdiolib_p *.o profiled/*.o tags Makefile.bak

depend:
	mkdep ${CFLAGS} ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

clrerr.o: clrerr.c /usr/include/stdio.h /usr/include/sys/types.h
clrerr.o: /usr/include/sys/select.h /usr/include/sys/types.h
clrerr.o: /usr/include/stdarg.h
exit.o: exit.c
fclose.o: fclose.c /usr/include/errno.h /usr/include/stdio.h
fclose.o: /usr/include/sys/types.h /usr/include/sys/select.h
fclose.o: /usr/include/sys/types.h /usr/include/stdarg.h /usr/include/stdlib.h
fclose.o: local.h
fdopen.o: fdopen.c /usr/include/sys/types.h /usr/include/sys/select.h
fdopen.o: /usr/include/sys/types.h /usr/include/fcntl.h
fdopen.o: /usr/include/sys/types.h /usr/include/unistd.h
fdopen.o: /usr/include/sys/types.h /usr/include/stdint.h /usr/include/stdio.h
fdopen.o: /usr/include/sys/types.h /usr/include/stdarg.h /usr/include/errno.h
fdopen.o: local.h
findfp.o: findfp.c /usr/include/sys/param.h /usr/include/sys/localopts.h
findfp.o: /usr/include/sys/stddef.h /usr/include/machine/machparam.h
findfp.o: /usr/include/sys/types.h /usr/include/sys/select.h
findfp.o: /usr/include/sys/types.h /usr/include/signal.h
findfp.o: /usr/include/sys/types.h /usr/include/unistd.h
findfp.o: /usr/include/sys/types.h /usr/include/stdint.h /usr/include/stdio.h
findfp.o: /usr/include/sys/types.h /usr/include/stdarg.h /usr/include/errno.h
findfp.o: /usr/include/stdlib.h /usr/include/string.h /usr/include/sys/types.h
findfp.o: local.h
fflush.o: fflush.c /usr/include/errno.h /usr/include/stdio.h
fflush.o: /usr/include/sys/types.h /usr/include/sys/select.h
fflush.o: /usr/include/sys/types.h /usr/include/stdarg.h local.h
fopen.o: fopen.c /usr/include/sys/types.h /usr/include/sys/select.h
fopen.o: /usr/include/sys/types.h /usr/include/sys/stat.h /usr/include/fcntl.h
fopen.o: /usr/include/sys/types.h /usr/include/stdio.h /usr/include/sys/types.h
fopen.o: /usr/include/stdarg.h /usr/include/errno.h local.h
fprintf.o: fprintf.c /usr/include/stdio.h /usr/include/sys/types.h
fprintf.o: /usr/include/sys/select.h /usr/include/sys/types.h
fprintf.o: /usr/include/stdarg.h /usr/include/stdarg.h
fpurge.o: fpurge.c /usr/include/errno.h /usr/include/stdio.h
fpurge.o: /usr/include/sys/types.h /usr/include/sys/select.h
fpurge.o: /usr/include/sys/types.h /usr/include/stdarg.h /usr/include/stdlib.h
fpurge.o: local.h
fread.o: fread.c /usr/include/stdio.h /usr/include/sys/types.h
fread.o: /usr/include/sys/select.h /usr/include/sys/types.h
fread.o: /usr/include/stdarg.h /usr/include/string.h /usr/include/sys/types.h
freopen.o: freopen.c /usr/include/sys/types.h /usr/include/sys/select.h
freopen.o: /usr/include/sys/types.h /usr/include/sys/stat.h
freopen.o: /usr/include/fcntl.h /usr/include/sys/types.h /usr/include/errno.h
freopen.o: /usr/include/unistd.h /usr/include/sys/types.h /usr/include/stdint.h
freopen.o: /usr/include/stdio.h /usr/include/sys/types.h /usr/include/stdarg.h
freopen.o: /usr/include/stdlib.h local.h
fseek.o: fseek.c /usr/include/sys/types.h /usr/include/sys/select.h
fseek.o: /usr/include/sys/types.h /usr/include/fcntl.h /usr/include/sys/types.h
fseek.o: /usr/include/stdio.h /usr/include/sys/types.h /usr/include/stdarg.h
fseek.o: /usr/include/stdlib.h /usr/include/errno.h local.h
ftell.o: ftell.c /usr/include/stdio.h /usr/include/sys/types.h
ftell.o: /usr/include/sys/select.h /usr/include/sys/types.h
ftell.o: /usr/include/stdarg.h /usr/include/errno.h local.h
fwrite.o: fwrite.c /usr/include/stdio.h /usr/include/sys/types.h
fwrite.o: /usr/include/sys/select.h /usr/include/sys/types.h
fwrite.o: /usr/include/stdarg.h local.h fvwrite.h
fscanf.o: fscanf.c /usr/include/stdio.h /usr/include/sys/types.h
fscanf.o: /usr/include/sys/select.h /usr/include/sys/types.h
fscanf.o: /usr/include/stdarg.h /usr/include/stdarg.h
fvwrite.o: fvwrite.c /usr/include/stdio.h /usr/include/sys/types.h
fvwrite.o: /usr/include/sys/select.h /usr/include/sys/types.h
fvwrite.o: /usr/include/stdarg.h /usr/include/string.h /usr/include/sys/types.h
fvwrite.o: local.h fvwrite.h
getc.o: getc.c /usr/include/stdio.h /usr/include/sys/types.h
getc.o: /usr/include/sys/select.h /usr/include/sys/types.h
getc.o: /usr/include/stdarg.h
getchar.o: getchar.c /usr/include/stdio.h /usr/include/sys/types.h
getchar.o: /usr/include/sys/select.h /usr/include/sys/types.h
getchar.o: /usr/include/stdarg.h
getw.o: getw.c /usr/include/stdio.h /usr/include/sys/types.h
getw.o: /usr/include/sys/select.h /usr/include/sys/types.h
getw.o: /usr/include/stdarg.h
makebuf.o: makebuf.c /usr/include/sys/types.h /usr/include/sys/select.h
makebuf.o: /usr/include/sys/types.h /usr/include/sys/stat.h
makebuf.o: /usr/include/unistd.h /usr/include/sys/types.h /usr/include/stdint.h
makebuf.o: /usr/include/stdio.h /usr/include/sys/types.h /usr/include/stdarg.h
makebuf.o: /usr/include/stdlib.h local.h
rget.o: rget.c /usr/include/stdio.h /usr/include/sys/types.h
rget.o: /usr/include/sys/select.h /usr/include/sys/types.h
rget.o: /usr/include/stdarg.h
fputc.o: fputc.c /usr/include/stdio.h /usr/include/sys/types.h
fputc.o: /usr/include/sys/select.h /usr/include/sys/types.h
fputc.o: /usr/include/stdarg.h
printf.o: printf.c /usr/include/stdio.h /usr/include/sys/types.h
printf.o: /usr/include/sys/select.h /usr/include/sys/types.h
printf.o: /usr/include/stdarg.h /usr/include/stdarg.h
putc.o: putc.c /usr/include/stdio.h /usr/include/sys/types.h
putc.o: /usr/include/sys/select.h /usr/include/sys/types.h
putc.o: /usr/include/stdarg.h
putchar.o: putchar.c /usr/include/stdio.h /usr/include/sys/types.h
putchar.o: /usr/include/sys/select.h /usr/include/sys/types.h
putchar.o: /usr/include/stdarg.h
putw.o: putw.c /usr/include/stdio.h /usr/include/sys/types.h
putw.o: /usr/include/sys/select.h /usr/include/sys/types.h
putw.o: /usr/include/stdarg.h
rewind.o: rewind.c /usr/include/errno.h /usr/include/stdio.h
rewind.o: /usr/include/sys/types.h /usr/include/sys/select.h
rewind.o: /usr/include/sys/types.h /usr/include/stdarg.h
scanf.o: scanf.c /usr/include/stdio.h /usr/include/sys/types.h
scanf.o: /usr/include/sys/select.h /usr/include/sys/types.h
scanf.o: /usr/include/stdarg.h /usr/include/stdarg.h
setbuf.o: setbuf.c /usr/include/stdio.h /usr/include/sys/types.h
setbuf.o: /usr/include/sys/select.h /usr/include/sys/types.h
setbuf.o: /usr/include/stdarg.h
setbuffer.o: setbuffer.c /usr/include/stdio.h /usr/include/sys/types.h
setbuffer.o: /usr/include/sys/select.h /usr/include/sys/types.h
setbuffer.o: /usr/include/stdarg.h
setvbuf.o: setvbuf.c /usr/include/stdio.h /usr/include/sys/types.h
setvbuf.o: /usr/include/sys/select.h /usr/include/sys/types.h
setvbuf.o: /usr/include/stdarg.h /usr/include/stdlib.h local.h
snprintf.o: snprintf.c /usr/include/stdio.h /usr/include/sys/types.h
snprintf.o: /usr/include/sys/select.h /usr/include/sys/types.h
snprintf.o: /usr/include/stdarg.h /usr/include/stdarg.h
sscanf.o: sscanf.c /usr/include/stdio.h /usr/include/sys/types.h
sscanf.o: /usr/include/sys/select.h /usr/include/sys/types.h
sscanf.o: /usr/include/stdarg.h /usr/include/string.h /usr/include/sys/types.h
sscanf.o: /usr/include/stdarg.h local.h
stdio.o: stdio.c /usr/include/fcntl.h /usr/include/sys/types.h
stdio.o: /usr/include/sys/select.h /usr/include/sys/types.h
stdio.o: /usr/include/unistd.h /usr/include/sys/types.h /usr/include/stdint.h
stdio.o: /usr/include/stdio.h /usr/include/sys/types.h /usr/include/stdarg.h
stdio.o: local.h
tmpfile.o: tmpfile.c /usr/include/sys/types.h /usr/include/sys/select.h
tmpfile.o: /usr/include/sys/types.h /usr/include/signal.h /usr/include/unistd.h
tmpfile.o: /usr/include/sys/types.h /usr/include/stdint.h /usr/include/errno.h
tmpfile.o: /usr/include/stdio.h /usr/include/sys/types.h /usr/include/stdarg.h
tmpfile.o: /usr/include/paths.h
tmpnam.o: tmpnam.c /usr/include/sys/types.h /usr/include/sys/select.h
tmpnam.o: /usr/include/sys/types.h /usr/include/stdio.h
tmpnam.o: /usr/include/sys/types.h /usr/include/stdarg.h /usr/include/unistd.h
tmpnam.o: /usr/include/sys/types.h /usr/include/stdint.h
tempnam.o: tempnam.c /usr/include/sys/param.h /usr/include/sys/localopts.h
tempnam.o: /usr/include/sys/stddef.h /usr/include/machine/machparam.h
tempnam.o: /usr/include/sys/types.h /usr/include/sys/select.h
tempnam.o: /usr/include/sys/types.h /usr/include/signal.h
tempnam.o: /usr/include/sys/types.h /usr/include/errno.h /usr/include/stdio.h
tempnam.o: /usr/include/sys/types.h /usr/include/stdarg.h /usr/include/stdlib.h
tempnam.o: /usr/include/unistd.h /usr/include/sys/types.h /usr/include/stdint.h
tempnam.o: /usr/include/paths.h
sprintf.o: sprintf.c /usr/include/stdio.h /usr/include/sys/types.h
sprintf.o: /usr/include/sys/select.h /usr/include/sys/types.h
sprintf.o: /usr/include/stdarg.h /usr/include/stdarg.h /usr/include/limits.h
sprintf.o: /usr/include/machine/limits.h local.h
ungetc.o: ungetc.c /usr/include/stdio.h /usr/include/sys/types.h
ungetc.o: /usr/include/sys/select.h /usr/include/sys/types.h
ungetc.o: /usr/include/stdarg.h /usr/include/stdlib.h /usr/include/string.h
ungetc.o: /usr/include/sys/types.h local.h
vfprintf.o: vfprintf.c /usr/include/stdarg.h /usr/include/stdlib.h
vfprintf.o: /usr/include/stdio.h /usr/include/sys/types.h
vfprintf.o: /usr/include/sys/select.h /usr/include/sys/types.h
vfprintf.o: /usr/include/stdarg.h /usr/include/limits.h
vfprintf.o: /usr/include/machine/limits.h local.h
vfscanf.o: vfscanf.c /usr/include/stdio.h /usr/include/sys/types.h
vfscanf.o: /usr/include/sys/select.h /usr/include/sys/types.h
vfscanf.o: /usr/include/stdarg.h /usr/include/stdlib.h /usr/include/ctype.h
vfscanf.o: /usr/include/stdarg.h local.h
vprintf.o: vprintf.c /usr/include/stdio.h /usr/include/sys/types.h
vprintf.o: /usr/include/sys/select.h /usr/include/sys/types.h
vprintf.o: /usr/include/stdarg.h
vscanf.o: vscanf.c /usr/include/stdio.h /usr/include/sys/types.h
vscanf.o: /usr/include/sys/select.h /usr/include/sys/types.h
vscanf.o: /usr/include/stdarg.h
fpfcom.o: fpfcom.c /usr/include/stdio.h /usr/include/sys/types.h
fpfcom.o: /usr/include/sys/select.h /usr/include/sys/types.h
fpfcom.o: /usr/include/stdarg.h local.h
vsnprintf.o: vsnprintf.c /usr/include/stdio.h /usr/include/sys/types.h
vsnprintf.o: /usr/include/sys/select.h /usr/include/sys/types.h
vsnprintf.o: /usr/include/stdarg.h
vsprintf.o: vsprintf.c /usr/include/stdio.h /usr/include/sys/types.h
vsprintf.o: /usr/include/sys/select.h /usr/include/sys/types.h
vsprintf.o: /usr/include/stdarg.h /usr/include/limits.h
vsprintf.o: /usr/include/machine/limits.h
wbuf.o: wbuf.c /usr/include/stdio.h /usr/include/sys/types.h
wbuf.o: /usr/include/sys/select.h /usr/include/sys/types.h
wbuf.o: /usr/include/stdarg.h local.h
flags.o: flags.c /usr/include/sys/types.h /usr/include/sys/select.h
flags.o: /usr/include/sys/types.h /usr/include/sys/file.h /usr/include/fcntl.h
flags.o: /usr/include/sys/types.h /usr/include/stdio.h /usr/include/sys/types.h
flags.o: /usr/include/stdarg.h /usr/include/errno.h
refill.o: refill.c /usr/include/errno.h /usr/include/stdio.h
refill.o: /usr/include/sys/types.h /usr/include/sys/select.h
refill.o: /usr/include/sys/types.h /usr/include/stdarg.h /usr/include/stdlib.h
refill.o: local.h
wsetup.o: wsetup.c /usr/include/stdio.h /usr/include/sys/types.h
wsetup.o: /usr/include/sys/select.h /usr/include/sys/types.h
wsetup.o: /usr/include/stdarg.h /usr/include/stdlib.h local.h
pfcom.o: pfcom.c /usr/include/stdarg.h /usr/include/stdlib.h
pfcom.o: /usr/include/stdio.h /usr/include/sys/types.h
pfcom.o: /usr/include/sys/select.h /usr/include/sys/types.h
pfcom.o: /usr/include/stdarg.h local.h
acvt.o: acvt.c /usr/include/stdio.h /usr/include/sys/types.h
acvt.o: /usr/include/sys/select.h /usr/include/sys/types.h
acvt.o: /usr/include/stdarg.h local.h
puts.o: puts.c /usr/include/stdio.h /usr/include/sys/types.h
puts.o: /usr/include/sys/select.h /usr/include/sys/types.h
puts.o: /usr/include/stdarg.h /usr/include/string.h /usr/include/sys/types.h
puts.o: fvwrite.h
fputs.o: fputs.c /usr/include/stdio.h /usr/include/sys/types.h
fputs.o: /usr/include/sys/select.h /usr/include/sys/types.h
fputs.o: /usr/include/stdarg.h /usr/include/string.h /usr/include/sys/types.h
fputs.o: fvwrite.h
fgetc.o: fgetc.c /usr/include/stdio.h /usr/include/sys/types.h
fgetc.o: /usr/include/sys/select.h /usr/include/sys/types.h
fgetc.o: /usr/include/stdarg.h
fgets.o: fgets.c /usr/include/stdio.h /usr/include/sys/types.h
fgets.o: /usr/include/sys/select.h /usr/include/sys/types.h
fgets.o: /usr/include/stdarg.h
gets.o: gets.c /usr/include/unistd.h /usr/include/sys/types.h
gets.o: /usr/include/sys/select.h /usr/include/sys/types.h
gets.o: /usr/include/stdint.h /usr/include/stdio.h /usr/include/sys/types.h
gets.o: /usr/include/stdarg.h

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
