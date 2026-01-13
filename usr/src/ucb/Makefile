#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.19 (2.11BSD) 2025/11/25
#
CFLAGS=	-O
LDFLAGS= -i

# Programs that live in subdirectories, and have makefiles of their own.
#
SUBDIR=	Mail compress error ex finger ftp indent lock man \
	more msgs netstat pascal rdist rlogin sendbug talk tftp \
	tn3270 top tset vgrind window

# Shell scripts that need only be installed and are never removed.
#
CSHSCRIPT= which

# C programs that live in the current directory and do not need
# explicit make lines.
#
STD=	apply biff checknr colcrt colrm ctags expand fold \
	from grep head last lastcomm leave logger mkstr \
	printenv ruptime rwho sccs script soelim strings strcompact \
	symcompact symdump symorder tail tcopy telnet unexpand unifdef users \
	whois what wc xstr yes

# C programs that live in the current directory and need explicit make lines.
#
NSTD=	clear ul vacation

# Programs that must run setuid to root
#
SETUID=	quota rsh

# Programs that must run set-group-id kmem.
#
KMEM=	gcore vmstat w

all:	${SUBDIR} ${STD} ${NSTD} ${KMEM} ${SETUID}

${SUBDIR}: FRC
	cd $@; make ${MFLAGS}

${STD} ${KMEM} ${SETUID}:
	cc ${CFLAGS} ${LDFLAGS} -o $@ $@.c

install: ${STD} ${NSTD} ${KMEM} ${SETUID}
	-for i in ${SUBDIR}; do \
		(cd $$i; make ${MFLAGS} install); done
	-for i in ${CSHSCRIPT}; do \
		(install -m 755 -c $$i.csh /usr/ucb/$$i); done
	-for i in ${STD} ${NSTD}; do \
		(install -c -s -m 755 $$i /usr/ucb/$$i); done
	-for i in ${KMEM}; do \
		(install -g kmem -m 2755 -c -s $$i /usr/ucb/$$i); done
	-for i in ${SETUID}; do \
		(install -o root -m 4755 -c -s $$i /usr/ucb/$$i); done
	rm -f /usr/ucb/uptime /usr/ucb/f
	ln /usr/ucb/w /usr/ucb/uptime
	ln /usr/ucb/finger /usr/ucb/f
	rm -f /usr/ucb/u
	ln /usr/ucb/users /usr/ucb/u

clean:
	rm -f a.out core *.s *.o
	for i in ${SUBDIR}; do (cd $$i; make ${MFLAGS} clean); done
	rm -f ${STD} ${NSTD} ${SETUID} ${KMEM}

FRC:

depend:
	for i in ${STD} ${NSTD} ${SETUID} ${KMEM}; do \
	    cc -M ${INCPATH} $$i.c | sed 's/\.o//' | \
	    awk ' { if ($$1 != prev) \
		{ if (rec != "") print rec; rec = $$0; prev = $$1; } \
		else { if (length(rec $$2) > 78) { print rec; rec = $$0; } \
		else rec = rec " " $$2 } } \
		END { print rec } ' >> makedep; done
	echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp Makefile Makefile.bak
	ed - Makefile < eddep
	rm eddep makedep
	echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
	echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
	echo '# see make depend above' >> Makefile

# Files listed in ${NSTD} have explicit make lines given below.

clear:
	${CC} -o clear ${LDFLAGS} ${CFLAGS} clear.c -ltermlib

ul:
	${CC} -o ul ${LDFLAGS} ${CFLAGS} ul.c -ltermlib

vacation: vacation.c
	${CC} -o vacation ${LDFLAGS} ${CFLAGS} vacation.c -ldbm

# DO NOT DELETE THIS LINE -- make depend uses it

