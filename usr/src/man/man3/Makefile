#
# Copyright (c) 1987 Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms are permitted
# provided that the above copyright notice and this paragraph are
# duplicated in all such forms and that any documentation,
# advertising materials, and other materials related to such
# distribution and use acknowledge that the software was developed
# by the University of California, Berkeley.  The name of the
# University may not be used to endorse or promote products derived
# from this software without specific prior written permission.
# THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
# WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
#	@(#)Makefile	5.4.18 (2.11BSD) 2025/12/24
#
MDIR=	/usr/man/cat3
SRCS1=	abort.3 abs.3 alarm.3 asinh.3 assert.3 atof.3 bstring.3 byteorder.3 \
	compat-sys5.3 creat.3 crypt.3 ctime.3 ctype.3 curses.3 dbm.3 daemon.3 \
	devname.3 directory.3 ecvt.3 end.3 erf.3 err.3 execl.3 exit.3 \
	exp.3 fclose.3 fflush.3 ferror.3 floor.3 fgetln.3 funopen.3 \
	fopen.3 fread.3 frexp.3 fseek.3 getc.3 getdisk.3 getenv.3 getfsent.3 \
	getgrent.3 gethostbyname.3 getnetent.3 getopt.3 getpass.3 \
	getgrouplist.3 getloadavg.3 getmntinfo.3 getsubopt.3 \
	getprotoent.3 getpwent.3 fgets.3 getservent.3 getttyent.3 \
	getusershell.3 getwd.3 hypot.3 ieee.3 inet.3 infnan.3 initgroups.3 
SRCS2=	insque.3 intro.3 j0.3 l3tol.3 ldfps.3 lgamma.3 lib2648.3 \
	malloc.3 math.3 mktemp.3 monitor.3 mp.3 ndbm.3 nice.3 nlist.3 ns.3 \
	pause.3 perror.3 plot.3 popen.3 printf.3 psignal.3 putc.3 fputs.3 \
	qsort.3 rand.3 random.3 rcmd.3 regex.3 resolver.3 rexec.3 remove.3 \
	scandir.3 \
	scanf.3 setbuf.3 setjmp.3 siginterrupt.3 signal.3 sin.3 \
	sinh.3 sleep.3 sqrt.3 stdarg.3 stdio.3 strftime.3 string.3 \
	strtok.3 stty.3 setmode.3 setruid.3 sigsetops.3 \
	strcspn.3 strpbrk.3 strsep.3 strspn.3 strtol.3 strtoul.3 strstr.3 \
	swab.3 sysctl.3 syslog.3 \
	system.3 termcap.3 time.3 times.3 tmpnam.3 \
	ttyname.3 ualarm.3 uname.3 ungetc.3 \
	syserrlst.3 \
	utime.3 valloc.3 varargs.3
OBJS1=	abort.0 abs.0 alarm.0 asinh.0 assert.0 atof.0 bstring.0 byteorder.0 \
	compat-sys5.0 creat.0 crypt.0 ctime.0 ctype.0 curses.0 dbm.0 daemon.0 \
	devname.0 directory.0 ecvt.0 end.0 erf.0 err.0 execl.0 exit.0 \
	exp.0 fclose.0 fflush.0 ferror.0 floor.0 fgetln.0 funopen.0 \
	fopen.0 fread.0 frexp.0 fseek.0 getc.0 getdisk.0 getenv.0 getfsent.0 \
	getgrent.0 gethostbyname.0 getnetent.0 getopt.0 getpass.0 \
	getprotoent.0 getpwent.0 fgets.0 getservent.0 getttyent.0 \
	getgrouplist.0 getloadavg.0 getmntinfo.0 getsubopt.0 \
	getusershell.0 getwd.0 hypot.0 ieee.0 inet.0 infnan.0 initgroups.0 
OBJS2=	insque.0 intro.0 j0.0 l3tol.0 ldfps.0 lgamma.0 lib2648.0 \
	malloc.0 math.0 mktemp.0 monitor.0 mp.0 ndbm.0 nice.0 nlist.0 ns.0 \
	pause.0 perror.0 plot.0 popen.0 printf.0 psignal.0 putc.0 fputs.0 \
	qsort.0 rand.0 random.0 rcmd.0 regex.0 resolver.0 rexec.0 remove.0 \
	scandir.0 \
	scanf.0 setbuf.0 setjmp.0 siginterrupt.0 signal.0 sin.0 \
	sinh.0 sleep.0 sqrt.0 stdarg.0 stdio.0 strftime.0 string.0 \
	strtok.0 stty.0 setmode.0 setruid.0 sigsetops.0 \
	strcspn.0 strpbrk.0 strsep.0 strspn.0 strtol.0 strtoul.0 strstr.0 \
	swab.0 sysctl.0 syslog.0 \
	system.0 termcap.0 time.0 times.0 tmpnam.0 \
	ttyname.0 ualarm.0 uname.0 ungetc.0 \
	syserrlst.0 \
	utime.0 valloc.0 varargs.0
REMO1=	edata.0 etext.0 j1.0 jn.0 ns_addr.0 ns_ntoa.0 y0.0 y1.0 yn.0 acos.0 \
	asin.0 atan.0 atan2.0 atoi.0 atol.0 cos.0 dbm_clearerr.0 dbm_close.0 \
	dbm_delete.0 dbm_error.0 dbm_fetch.0 dbm_firstkey.0 dbm_nextkey.0 \
	dbm_open.0 dbm_store.0 dbminit.0 delete.0 erfc.0 expm1.0 fcvt.0 \
	fetch.0 firstkey.0 gcvt.0 log.0 log10.0 log1p.0 nextkey.0 pow.0 \
	rresvport.0 ruserok.0 store.0 tan.0 addr.0 arc.0 asctime.0 cbrt.0 \
	circle.0 comp.0 cont.0 copysign.0 cosh.0 drem.0 encrypt.0 environ.0 \
	erase.0 execle.0 execlp.0 exect.0 execv.0 execve.0 execvp.0 fgetc.0 \
	gets.0 finite.0 fputc.0 puts.0 ftime.0 getchar.0 getw.0 gmtime.0 \
	gtty.0 inet_addr.0 inet_lnaof.0 inet_makeaddr.0 inet_netof.0 \
	inet_network.0 inet_ntoa.0 isalnum.0 isalpha.0 isascii.0 iscntrl.0 \
	isdigit.0 islower.0 isprint.0 ispunct.0 isspace.0 isupper.0 \
	isxdigit.0 label.0 ldexp.0 line.0 linemod.0 localtime.0 logb.0 modf.0 \
	move.0 network.0 ntoa.0 openpl.0 pclose.0 point.0 putchar.0 putw.0
REMO2=	re_comp.0 re_exec.0 scalb.0 setkey.0 space.0 srand.0 tanh.0 \
	timezone.0 toascii.0 tolower.0 toupper.0 ltol3.0 _longjmp.0 \
	_setjmp.0 acosh.0 alloca.0 atanh.0 cabs.0 calloc.0 ceil.0 \
	closelog.0 fpurge.0 fabs.0 fdopen.0 free.0 freopen.0 fscanf.0 \
	ftell.0 fwrite.0 index.0 initstate.0 longjmp.0 mkstemp.0 openlog.0  \
	realloc.0 remque.0 rewind.0 rindex.0 rint.0 \
	setlogmask.0 setstate.0 setrgid.0 srandom.0 \
	sscanf.0 strcat.0 strcmp.0 strcpy.0 strlen.0 strncat.0 strncmp.0 \
	strncpy.0 strcasecmp.0 strncasecmp.0 fropen.0 fwopen.0 \
	strerror.0 snprintf.0 vsnprintf.0 vfscanf.0 vscanf.0 vsscanf.0 \
	sprintf.0 vfprintf.0 vprintf.0 vsprintf.0 alphasort.0 bcmp.0 bcopy.0 \
	bzero.0 clearerr.0 feof.0 ffs.0 fileno.0 fprintf.0 gamma.0  \
	getdiskbyname.0 isatty.0 moncontrol.0 monstartup.0 setbuffer.0 \
	setlinebuf.0 setvbuf.0 sys_siglist.0 ttyslot.0 endfsent.0 endgrent.0 \
	endpwent.0 getfsfile.0 getfsspec.0 getfstype.0 getgrgid.0 

REMO3=	getgrnam.0 getpwnam.0 getpwuid.0 setfsent.0 setgrent.0 setpwent.0 \
	setpwfile.0 tgetent.0 tgetflag.0 tgetnum.0 tgetstr.0 tgoto.0 tputs.0 \
	closedir.0 endttyent.0 getttynam.0 opendir.0 readdir.0 rewinddir.0 \
	seekdir.0 setttyent.0 telldir.0 endnetent.0 getnetbyaddr.0 \
	getnetbyname.0 htonl.0 htons.0 ntohl.0 ntohs.0 setnetent.0 \
	endservent.0 getservbyname.0 getservbyport.0 setservent.0 \
	endprotoent.0 endusershell.0 getprotobyname.0 getprotobynumber.0 \
	setprotoent.0 setusershell.0 endhostent.0 gethostbyaddr.0 gethostent.0 \
	sethostent.0 sethostfile.0 memccpy.0 memchr.0 memcmp.0 memcpy.0 \
	memset.0 strchr.0 strrchr.0 tempnam.0 tmpfile.0 fgetpos.0 fsetpos.0 \
	tempnam.0 getmode.0 usleep.0 setenv.0 unsetenv.0

REMO4=	verr.0 errx.0 verrx.0 warn.0 warnx.0 vwarnx.0 

REMO5 = sigaddset.0 sigdelset.0 sigemptyset.0 sigfillset.0 sigismember.0

.SUFFIXES: .3 .0

.3.0:
	/usr/man/manroff $*.3 > $*.0

all: _make_01 _make_02

_make_01: ${OBJS1}

_make_02: ${OBJS2}

clean: FRC
	rm -f ${OBJS1}
	rm -f ${OBJS2}

install: _make_01 _make_02
	install -c -o root -g staff -m 444 ${OBJS1} ${MDIR}
	install -c -o root -g staff -m 444 ${OBJS2} ${MDIR}
	for i in ${REMO1}; do \
		rm -f  ${MDIR}/$$i; \
	done
	for i in ${REMO2}; do \
		rm -f  ${MDIR}/$$i; \
	done
	for i in ${REMO3}; do \
		rm -f  ${MDIR}/$$i; \
	done
	for i in ${REMO4}; do \
		rm -f  ${MDIR}/$$i; \
	done
	for i in ${REMO4}; do \
		ln ${MDIR}/err.0 ${MDIR}/$$i; \
	done
	for i in ${REMO5}; do \
		rm -f ${MDIR}/$$i; \
	done
	for i in ${REMO5}; do \
		ln ${MDIR}/sigsetops.0 ${MDIR}/$$i; \
	done

	ln ${MDIR}/end.0 ${MDIR}/edata.0
	ln ${MDIR}/end.0 ${MDIR}/etext.0
	ln ${MDIR}/j0.0 ${MDIR}/j1.0
	ln ${MDIR}/j0.0 ${MDIR}/jn.0
	ln ${MDIR}/ns.0 ${MDIR}/ns_addr.0
	ln ${MDIR}/ns.0 ${MDIR}/ns_ntoa.0
	ln ${MDIR}/j0.0 ${MDIR}/y0.0
	ln ${MDIR}/j0.0 ${MDIR}/y1.0
	ln ${MDIR}/j0.0 ${MDIR}/yn.0
	ln ${MDIR}/sin.0 ${MDIR}/acos.0
	ln ${MDIR}/sin.0 ${MDIR}/asin.0
	ln ${MDIR}/sin.0 ${MDIR}/atan.0
	ln ${MDIR}/sin.0 ${MDIR}/atan2.0
	ln ${MDIR}/atof.0 ${MDIR}/atoi.0
	ln ${MDIR}/atof.0 ${MDIR}/atol.0
	ln ${MDIR}/sin.0 ${MDIR}/cos.0
	ln ${MDIR}/ndbm.0 ${MDIR}/dbm_clearerr.0
	ln ${MDIR}/ndbm.0 ${MDIR}/dbm_close.0
	ln ${MDIR}/ndbm.0 ${MDIR}/dbm_delete.0
	ln ${MDIR}/ndbm.0 ${MDIR}/dbm_error.0
	ln ${MDIR}/ndbm.0 ${MDIR}/dbm_fetch.0
	ln ${MDIR}/ndbm.0 ${MDIR}/dbm_firstkey.0
	ln ${MDIR}/ndbm.0 ${MDIR}/dbm_nextkey.0
	ln ${MDIR}/ndbm.0 ${MDIR}/dbm_open.0
	ln ${MDIR}/ndbm.0 ${MDIR}/dbm_store.0
	ln ${MDIR}/dbm.0 ${MDIR}/dbminit.0
	ln ${MDIR}/dbm.0 ${MDIR}/delete.0
	ln ${MDIR}/erf.0 ${MDIR}/erfc.0
	ln ${MDIR}/exp.0 ${MDIR}/expm1.0
	ln ${MDIR}/ecvt.0 ${MDIR}/fcvt.0
	ln ${MDIR}/dbm.0 ${MDIR}/fetch.0
	ln ${MDIR}/dbm.0 ${MDIR}/firstkey.0
	ln ${MDIR}/ecvt.0 ${MDIR}/gcvt.0
	ln ${MDIR}/exp.0 ${MDIR}/log.0
	ln ${MDIR}/exp.0 ${MDIR}/log10.0
	ln ${MDIR}/exp.0 ${MDIR}/log1p.0
	ln ${MDIR}/dbm.0 ${MDIR}/nextkey.0
	ln ${MDIR}/exp.0 ${MDIR}/pow.0
	ln ${MDIR}/rcmd.0 ${MDIR}/rresvport.0
	ln ${MDIR}/rcmd.0 ${MDIR}/ruserok.0
	ln ${MDIR}/dbm.0 ${MDIR}/store.0
	ln ${MDIR}/sin.0 ${MDIR}/tan.0
	ln ${MDIR}/inet.0 ${MDIR}/addr.0
	ln ${MDIR}/plot.0 ${MDIR}/arc.0
	ln ${MDIR}/ctime.0 ${MDIR}/asctime.0
	ln ${MDIR}/sqrt.0 ${MDIR}/cbrt.0
	ln ${MDIR}/plot.0 ${MDIR}/circle.0
	ln ${MDIR}/regex.0 ${MDIR}/comp.0
	ln ${MDIR}/plot.0 ${MDIR}/cont.0
	ln ${MDIR}/ieee.0 ${MDIR}/copysign.0
	ln ${MDIR}/sinh.0 ${MDIR}/cosh.0
	ln ${MDIR}/ieee.0 ${MDIR}/drem.0
	ln ${MDIR}/crypt.0 ${MDIR}/encrypt.0
	ln ${MDIR}/execl.0 ${MDIR}/environ.0
	ln ${MDIR}/plot.0 ${MDIR}/erase.0
	ln ${MDIR}/execl.0 ${MDIR}/execle.0
	ln ${MDIR}/execl.0 ${MDIR}/execlp.0
	ln ${MDIR}/execl.0 ${MDIR}/exect.0
	ln ${MDIR}/execl.0 ${MDIR}/execv.0
	ln ${MDIR}/execl.0 ${MDIR}/execve.0
	ln ${MDIR}/execl.0 ${MDIR}/execvp.0
	ln ${MDIR}/getc.0 ${MDIR}/fgetc.0
	ln ${MDIR}/fgets.0 ${MDIR}/gets.0
	ln ${MDIR}/ieee.0 ${MDIR}/finite.0
	ln ${MDIR}/putc.0 ${MDIR}/fputc.0
	ln ${MDIR}/fputs.0 ${MDIR}/puts.0
	ln ${MDIR}/time.0 ${MDIR}/ftime.0
	ln ${MDIR}/getc.0 ${MDIR}/getchar.0
	ln ${MDIR}/getc.0 ${MDIR}/getw.0
	ln ${MDIR}/ctime.0 ${MDIR}/gmtime.0
	ln ${MDIR}/stty.0 ${MDIR}/gtty.0
	ln ${MDIR}/inet.0 ${MDIR}/inet_addr.0
	ln ${MDIR}/inet.0 ${MDIR}/inet_lnaof.0
	ln ${MDIR}/inet.0 ${MDIR}/inet_makeaddr.0
	ln ${MDIR}/inet.0 ${MDIR}/inet_netof.0
	ln ${MDIR}/inet.0 ${MDIR}/inet_network.0
	ln ${MDIR}/inet.0 ${MDIR}/inet_ntoa.0
	ln ${MDIR}/ctype.0 ${MDIR}/isalnum.0
	ln ${MDIR}/ctype.0 ${MDIR}/isalpha.0
	ln ${MDIR}/ctype.0 ${MDIR}/isascii.0
	ln ${MDIR}/ctype.0 ${MDIR}/iscntrl.0
	ln ${MDIR}/ctype.0 ${MDIR}/isdigit.0
	ln ${MDIR}/ctype.0 ${MDIR}/islower.0
	ln ${MDIR}/ctype.0 ${MDIR}/isprint.0
	ln ${MDIR}/ctype.0 ${MDIR}/ispunct.0
	ln ${MDIR}/ctype.0 ${MDIR}/isspace.0
	ln ${MDIR}/ctype.0 ${MDIR}/isupper.0
	ln ${MDIR}/ctype.0 ${MDIR}/isxdigit.0
	ln ${MDIR}/plot.0 ${MDIR}/label.0
	ln ${MDIR}/frexp.0 ${MDIR}/ldexp.0
	ln ${MDIR}/plot.0 ${MDIR}/line.0
	ln ${MDIR}/plot.0 ${MDIR}/linemod.0
	ln ${MDIR}/ctime.0 ${MDIR}/localtime.0
	ln ${MDIR}/ieee.0 ${MDIR}/logb.0
	ln ${MDIR}/frexp.0 ${MDIR}/modf.0
	ln ${MDIR}/plot.0 ${MDIR}/move.0
	ln ${MDIR}/inet.0 ${MDIR}/network.0
	ln ${MDIR}/inet.0 ${MDIR}/ntoa.0
	ln ${MDIR}/plot.0 ${MDIR}/openpl.0
	ln ${MDIR}/popen.0 ${MDIR}/pclose.0
	ln ${MDIR}/plot.0 ${MDIR}/point.0
	ln ${MDIR}/putc.0 ${MDIR}/putchar.0
	ln ${MDIR}/putc.0 ${MDIR}/putw.0
	ln ${MDIR}/regex.0 ${MDIR}/re_comp.0
	ln ${MDIR}/regex.0 ${MDIR}/re_exec.0
	ln ${MDIR}/ieee.0 ${MDIR}/scalb.0
	ln ${MDIR}/crypt.0 ${MDIR}/setkey.0
	ln ${MDIR}/plot.0 ${MDIR}/space.0
	ln ${MDIR}/rand.0 ${MDIR}/srand.0
	ln ${MDIR}/sinh.0 ${MDIR}/tanh.0
	ln ${MDIR}/ctime.0 ${MDIR}/timezone.0
	ln ${MDIR}/ctype.0 ${MDIR}/toascii.0
	ln ${MDIR}/ctype.0 ${MDIR}/tolower.0
	ln ${MDIR}/ctype.0 ${MDIR}/toupper.0
	ln ${MDIR}/l3tol.0 ${MDIR}/ltol3.0
	ln ${MDIR}/setjmp.0 ${MDIR}/_longjmp.0
	ln ${MDIR}/setjmp.0 ${MDIR}/_setjmp.0
	ln ${MDIR}/asinh.0 ${MDIR}/acosh.0
	ln ${MDIR}/malloc.0 ${MDIR}/alloca.0
	ln ${MDIR}/asinh.0 ${MDIR}/atanh.0
	ln ${MDIR}/hypot.0 ${MDIR}/cabs.0
	ln ${MDIR}/malloc.0 ${MDIR}/calloc.0
	ln ${MDIR}/floor.0 ${MDIR}/ceil.0
	ln ${MDIR}/syslog.0 ${MDIR}/closelog.0
	ln ${MDIR}/perror.0 ${MDIR}/strerror.0
	ln ${MDIR}/floor.0 ${MDIR}/fabs.0
	ln ${MDIR}/fopen.0 ${MDIR}/fdopen.0
	ln ${MDIR}/malloc.0 ${MDIR}/free.0
	ln ${MDIR}/fopen.0 ${MDIR}/freopen.0
	ln ${MDIR}/scanf.0 ${MDIR}/fscanf.0
	ln ${MDIR}/fseek.0 ${MDIR}/ftell.0
	ln ${MDIR}/fread.0 ${MDIR}/fwrite.0
	ln ${MDIR}/string.0 ${MDIR}/index.0
	ln ${MDIR}/random.0 ${MDIR}/initstate.0
	ln ${MDIR}/setjmp.0 ${MDIR}/longjmp.0
	ln ${MDIR}/mktemp.0 ${MDIR}/mkstemp.0
	ln ${MDIR}/syslog.0 ${MDIR}/openlog.0
	ln ${MDIR}/malloc.0 ${MDIR}/realloc.0
	ln ${MDIR}/insque.0 ${MDIR}/remque.0
	ln ${MDIR}/fseek.0 ${MDIR}/rewind.0
	ln ${MDIR}/fseek.0 ${MDIR}/fgetpos.0
	ln ${MDIR}/fseek.0 ${MDIR}/fsetpos.0
	ln ${MDIR}/string.0 ${MDIR}/rindex.0
	ln ${MDIR}/floor.0 ${MDIR}/rint.0
	ln ${MDIR}/syslog.0 ${MDIR}/setlogmask.0
	ln ${MDIR}/random.0 ${MDIR}/setstate.0
	ln ${MDIR}/random.0 ${MDIR}/srandom.0
	ln ${MDIR}/scanf.0 ${MDIR}/sscanf.0
	ln ${MDIR}/scanf.0 ${MDIR}/vfscanf.0
	ln ${MDIR}/scanf.0 ${MDIR}/vscanf.0
	ln ${MDIR}/scanf.0 ${MDIR}/vsscanf.0
	ln ${MDIR}/string.0 ${MDIR}/strcat.0
	ln ${MDIR}/string.0 ${MDIR}/strcmp.0
	ln ${MDIR}/string.0 ${MDIR}/strcpy.0
	ln ${MDIR}/string.0 ${MDIR}/strlen.0
	ln ${MDIR}/string.0 ${MDIR}/strncat.0
	ln ${MDIR}/string.0 ${MDIR}/strncmp.0
	ln ${MDIR}/string.0 ${MDIR}/strncpy.0
	ln ${MDIR}/string.0 ${MDIR}/strcasecmp.0
	ln ${MDIR}/string.0 ${MDIR}/strncasecmp.0
	ln ${MDIR}/printf.0 ${MDIR}/sprintf.0
	ln ${MDIR}/printf.0 ${MDIR}/vfprintf.0
	ln ${MDIR}/printf.0 ${MDIR}/vprintf.0
	ln ${MDIR}/printf.0 ${MDIR}/vsprintf.0
	ln ${MDIR}/scandir.0 ${MDIR}/alphasort.0
	ln ${MDIR}/bstring.0 ${MDIR}/bcmp.0
	ln ${MDIR}/bstring.0 ${MDIR}/bcopy.0
	ln ${MDIR}/bstring.0 ${MDIR}/bzero.0
	ln ${MDIR}/ferror.0 ${MDIR}/clearerr.0
	ln ${MDIR}/ferror.0 ${MDIR}/feof.0
	ln ${MDIR}/bstring.0 ${MDIR}/ffs.0
	ln ${MDIR}/ferror.0 ${MDIR}/fileno.0
	ln ${MDIR}/printf.0 ${MDIR}/fprintf.0
	ln ${MDIR}/printf.0 ${MDIR}/vsnprintf.0
	ln ${MDIR}/printf.0 ${MDIR}/snprintf.0
	ln ${MDIR}/lgamma.0 ${MDIR}/gamma.0
	ln ${MDIR}/getdisk.0 ${MDIR}/getdiskbyname.0
	ln ${MDIR}/ttyname.0 ${MDIR}/isatty.0
	ln ${MDIR}/monitor.0 ${MDIR}/moncontrol.0
	ln ${MDIR}/monitor.0 ${MDIR}/monstartup.0
	ln ${MDIR}/setbuf.0 ${MDIR}/setbuffer.0
	ln ${MDIR}/setbuf.0 ${MDIR}/setlinebuf.0
	ln ${MDIR}/setbuf.0 ${MDIR}/setvbuf.0
	ln ${MDIR}/psignal.0 ${MDIR}/sys_siglist.0
	ln ${MDIR}/ttyname.0 ${MDIR}/ttyslot.0
	ln ${MDIR}/getfsent.0 ${MDIR}/endfsent.0
	ln ${MDIR}/getgrent.0 ${MDIR}/endgrent.0
	ln ${MDIR}/getpwent.0 ${MDIR}/endpwent.0
	ln ${MDIR}/getfsent.0 ${MDIR}/getfsfile.0
	ln ${MDIR}/getfsent.0 ${MDIR}/getfsspec.0
	ln ${MDIR}/getfsent.0 ${MDIR}/getfstype.0
	ln ${MDIR}/getgrent.0 ${MDIR}/getgrgid.0
	ln ${MDIR}/getgrent.0 ${MDIR}/getgrnam.0
	ln ${MDIR}/getpwent.0 ${MDIR}/getpwnam.0
	ln ${MDIR}/getpwent.0 ${MDIR}/getpwuid.0
	ln ${MDIR}/getfsent.0 ${MDIR}/setfsent.0
	ln ${MDIR}/getgrent.0 ${MDIR}/setgrent.0
	ln ${MDIR}/getpwent.0 ${MDIR}/setpwent.0
	ln ${MDIR}/getpwent.0 ${MDIR}/setpwfile.0
	ln ${MDIR}/termcap.0 ${MDIR}/tgetent.0
	ln ${MDIR}/termcap.0 ${MDIR}/tgetflag.0
	ln ${MDIR}/termcap.0 ${MDIR}/tgetnum.0
	ln ${MDIR}/termcap.0 ${MDIR}/tgetstr.0
	ln ${MDIR}/termcap.0 ${MDIR}/tgoto.0
	ln ${MDIR}/termcap.0 ${MDIR}/tputs.0
	ln ${MDIR}/directory.0 ${MDIR}/closedir.0
	ln ${MDIR}/getttyent.0 ${MDIR}/endttyent.0
	ln ${MDIR}/getttyent.0 ${MDIR}/getttynam.0
	ln ${MDIR}/directory.0 ${MDIR}/opendir.0
	ln ${MDIR}/directory.0 ${MDIR}/readdir.0
	ln ${MDIR}/directory.0 ${MDIR}/rewinddir.0
	ln ${MDIR}/directory.0 ${MDIR}/seekdir.0
	ln ${MDIR}/getttyent.0 ${MDIR}/setttyent.0
	ln ${MDIR}/directory.0 ${MDIR}/telldir.0
	ln ${MDIR}/getnetent.0 ${MDIR}/endnetent.0
	ln ${MDIR}/getnetent.0 ${MDIR}/getnetbyaddr.0
	ln ${MDIR}/getnetent.0 ${MDIR}/getnetbyname.0
	ln ${MDIR}/byteorder.0 ${MDIR}/htonl.0
	ln ${MDIR}/byteorder.0 ${MDIR}/htons.0
	ln ${MDIR}/byteorder.0 ${MDIR}/ntohl.0
	ln ${MDIR}/byteorder.0 ${MDIR}/ntohs.0
	ln ${MDIR}/getnetent.0 ${MDIR}/setnetent.0
	ln ${MDIR}/getservent.0 ${MDIR}/endservent.0
	ln ${MDIR}/getservent.0 ${MDIR}/getservbyname.0
	ln ${MDIR}/getservent.0 ${MDIR}/getservbyport.0
	ln ${MDIR}/getservent.0 ${MDIR}/setservent.0
	ln ${MDIR}/getprotoent.0 ${MDIR}/endprotoent.0
	ln ${MDIR}/getusershell.0 ${MDIR}/endusershell.0
	ln ${MDIR}/getprotoent.0 ${MDIR}/getprotobyname.0
	ln ${MDIR}/getprotoent.0 ${MDIR}/getprotobynumber.0
	ln ${MDIR}/getprotoent.0 ${MDIR}/setprotoent.0
	ln ${MDIR}/getusershell.0 ${MDIR}/setusershell.0
	ln ${MDIR}/gethostbyname.0 ${MDIR}/endhostent.0
	ln ${MDIR}/gethostbyname.0 ${MDIR}/gethostbyaddr.0
	ln ${MDIR}/gethostbyname.0 ${MDIR}/gethostent.0
	ln ${MDIR}/gethostbyname.0 ${MDIR}/sethostent.0
	ln ${MDIR}/gethostbyname.0 ${MDIR}/sethostfile.0
	ln ${MDIR}/setmode.0 ${MDIR}/getmode.0
	ln ${MDIR}/tmpnam.0 ${MDIR}/tmpfile.0
	ln ${MDIR}/tmpnam.0 ${MDIR}/tempnam.0
	ln ${MDIR}/compat-sys5.0 ${MDIR}/memccpy.0
	ln ${MDIR}/compat-sys5.0 ${MDIR}/memchr.0
	ln ${MDIR}/compat-sys5.0 ${MDIR}/memcmp.0
	ln ${MDIR}/compat-sys5.0 ${MDIR}/memcpy.0
	ln ${MDIR}/compat-sys5.0 ${MDIR}/memset.0
	ln ${MDIR}/compat-sys5.0 ${MDIR}/strchr.0
	ln ${MDIR}/compat-sys5.0 ${MDIR}/strrchr.0
	ln ${MDIR}/sleep.0 ${MDIR}/usleep.0
	ln ${MDIR}/getenv.0 ${MDIR}/setenv.0
	ln ${MDIR}/getenv.0 ${MDIR}/unsetenv.0
	ln ${MDIR}/setruid.0 ${MDIR}/setrgid.0
	ln ${MDIR}/fflush.0 ${MDIR}/fpurge.0
	ln ${MDIR}/funopen.0 ${MDIR}/fropen.0
	ln ${MDIR}/funopen.0 ${MDIR}/fwopen.0

FRC:
