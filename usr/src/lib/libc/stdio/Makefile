#
# stdio makefile
#

CFLAGS=-O
OV=

OVER=	doprnt.o

STDIO=	clrerr.o data.o doscan.o endopen.o fdopen.o ffltpr.o fgetc.o \
	fgets.o filbuf.o findiop.o flsbuf.o fltpr.o fopen.o fprintf.o \
	fputc.o fputs.o freopen.o fseek.o fstab.o ftell.o gcvt.o getchar.o \
	getgrent.o getgrgid.o getgrnam.o getpw.o \
	getpwent.o getpwnam.o getpwuid.o gets.o getw.o printf.o \
	putchar.o puts.o putw.o rdwr.o rew.o scanf.o setbuf.o sprintf.o \
	strout.o system.o tabify.o ungetc.o closedir.o opendir.o readdir.o \
	scandir.o seekdir.o telldir.o

libc.a: ${STDIO} ${OVER}
	ar q ../$@ ${STDIO} ${OVER}

libovc.a: ${STDIO}
	ar q ../$@ ${STDIO}

clean:
	rm -f ${STDIO} ${OVER}

.c.o:
	${CC} ${CFLAGS} ${FP} ${OV} -c $*.c

.s.o:
	as ${OV} - -o $*.o $*.s

closedir.o:	closedir.c
clrerr.o:	clrerr.c
data.o:		data.c
doprnt.o:	doprnt.s
doscan.o:	doscan.c
endopen.o:	endopen.c
fdopen.o:	fdopen.c
ffltpr.o:	ffltpr.s
fgetc.o:	fgetc.c
fgets.o:	fgets.c
filbuf.o:	filbuf.c
findiop.o:	findiop.c
flsbuf.o:	flsbuf.c
fltpr.o:	fltpr.s
fopen.o:	fopen.c
fprintf.o:	fprintf.c
fputc.o:	fputc.c
fputs.o:	fputs.c
freopen.o:	freopen.c
fseek.o:	fseek.c
fstab.o:	fstab.c
ftell.o:	ftell.c
gcvt.o:		gcvt.c
getchar.o:	getchar.c
getgrent.o:	getgrent.c
getgrgid.o:	getgrgid.c
getgrnam.o:	getgrnam.c
getmap.o:	getmap.c
getpw.o:	getpw.c
getpwent.o:	getpwent.c
getpwnam.o:	getpwnam.c
getpwuid.o:	getpwuid.c
gets.o:		gets.c
getw.o:		getw.c
maxusers.o:	maxusers.c
opendir.o:	opendir.c
printf.o:	printf.c
putchar.o:	putchar.c
puts.o:		puts.c
putw.o:		putw.c
rdwr.o:		rdwr.c
readdir.o:	readdir.c
rew.o:		rew.c
scandir.o:	scandir.c
scanf.o:	scanf.c
seekdir.o:	seekdir.c
setbuf.o:	setbuf.c
sprintf.o:	sprintf.c
strout.o:	strout.c
system.o:	system.c
tabify.o:	tabify.c
telldir.o:	telldir.c
ucount.o:	ucount.c
ungetc.o:	ungetc.c
