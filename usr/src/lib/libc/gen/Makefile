#
# gen makefile
#

CFLAGS=-O
OV=
FL=

GEN=	abort.o abs.o atoi.o atol.o calloc.o crypt.o ctime.o ctype_.o \
	cuexit.o errlst.o execvp.o fakcu.o fakfp.o frexp11.o getenv.o \
	getlogin.o getpass.o getwd.o index.o isapipe.o isatty.o l3.o \
	ldexp11.o ldfps.o loadav.o malloc.o mktemp.o modf11.o mon.o nlist.o \
	perror.o popen.o psignal.o qsort.o random.o regex.o rindex.o \
	siglist.o sigset.o sleep.o strcat.o strcmp.o strcpy.o strlen.o \
	strncat.o strncmp.o strncpy.o stty.o swab.o tell.o timezone.o \
	ttyname.o ttyslot.o atof.o ecvt.o initgroups.o getopt.o

OVER=	setjmp.o

libc.a: atof.o ecvt.o ${GEN} ${OVER}
	ar q ../$@ ${GEN} ${OVER}

libovc.a: atof.o ecvt.o ${GEN}
	ar q ../$@ ${GEN}

clean:
	rm -f ${GEN} ${OVER}

.c.o:
	${CC} ${CFLAGS} ${FP} ${OV} -c $*.c

.s.o:
	as ${OV} - -o $*.o $*.s

abort.o:	abort.s
abs.o:		abs.c
atof.o:		atof.c
atoi.o:		atoi.c
atol.o:		atol.c
calloc.o:	calloc.c
crypt.o:	crypt.c
ctime.o:	ctime.c
ctype_.o:	ctype_.c
cuexit.o:	cuexit.o
cuexit.o:	cuexit.s
disktab.o:	disktab.c
ecvt.o:		ecvt.o
errlst.o:	errlst.c
execvp.o:	execvp.c
fakcu.o:	fakcu.c
fakfp.o:	fakfp.s
frexp11.o:	frexp11.s
getenv.o:	getenv.c
getlogin.o:	getlogin.c
getpass.o:	getpass.c
getwd.o:	getwd.c
index.o:	index.c
isapipe.o:	isapipe.c
isatty.o:	isatty.c
l3.o:		l3.c
ldexp11.o:	ldexp11.s
ldfps.o:	ldfps.s
loadav.o:	loadav.c
malloc.o:	malloc.c
mktemp.o:	mktemp.c
modf11.o:	modf11.s
mon.o:		mon.c
nlist.o:	nlist.c
perror.o:	perror.c
pkon.o:		pkon.c
popen.o:	popen.c
psignal.o:	psignal.c
qsort.o:	qsort.c
rand.o:		rand.s
random.o:	random.c
regex.o:	regex.c
rindex.o:	rindex.c
setjmp.o:	setjmp.o
setjmp.o:	setjmp.s
siglist.o:	siglist.c
sigset.o:	sigset.c
sleep.o:	sleep.c
strcat.o:	strcat.c
strcmp.o:	strcmp.c
strcpy.o:	strcpy.c
strlen.o:	strlen.c
strncat.o:	strncat.c
strncmp.o:	strncmp.c
strncpy.o:	strncpy.c
stty.o:		stty.c
swab.o:		swab.c
tell.o:		tell.c
timezone.o:	timezone.c
ttyname.o:	ttyname.c
ttyslot.o:	ttyslot.c
initgroups.o:	initgroups.c
