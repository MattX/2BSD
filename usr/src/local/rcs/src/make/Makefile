# $Header: Makefile,v 1.3 83/11/28 09:53:11 tony Exp $

#	@(#)/usr/src/cmd/make/make.mk	3.5

#	@(#)make.mk	3.1

# Description file for the Make command
# To build the Columbus make just type "make -f make.mk"
# For systems without the getu() system call type "make -f make.mk GETU="
# For pwb use
#	make -f make.mk GETU= RULES=pwbrules CFLAGS="-O -DPWB" LIBES=-lpw
# The rules.c file can be modified locally for people who still like
#	things like fortran.
# If the markfile is not present the compilation can be turned of with
#	"make -f make.mk MARKFILE="
# For people interested in finding out how the markfile is generated
# 	contact E. G. Bradford.

#	There is an internal rule (rules.c) which can make
#	and object file "markfile.o" from "markfile" if the
#	"markfile" has a what(1) string in it.
#	Disable the $(MARKFILE) macro if the internal: rule
#	doesn't work on your system.
MARKFILE =

#	Turn off the GETU variable if you system does not have the
#	getu system call as documented in the CB unix manual.
GETU	=

#	RULES is defined to be the Columbus rules. It can be defined to
#	be the pwb rules by typing:
#		make -f make.mk RULES=pwbrules
#
RULES	= rules

#	For PWB systems the PWB define symbol must be in the CFLAGS macro.
#	Thus for pwb systems:
#	    make -f make.mk CFLAGS="-O -DPWB" GETU= RULES=pwbrules LIBES=-lpw
#	will make a version of "make" for PWB systems.

#	For Berkeley 4.1 Systems, BSD should be set to -DBSD.  This is
#	necessary due to a change in the structure definition for a.out
#	headers in /u/include/a.out.h.  With the two include files,
#	./ar.h and ./ar11.h, both formats of archives should be represented.
#	ar.h contains the 4.1BSD ASCII archive format, and ar11.h
#	contains the V7 binary archive format.  These were so named
#	because all the 11's at pur-ee run V7.  To compile on V7, these
#	structures should have unique member names.
#	These changes may or may not hold for Berkeley 4.2 Distribution
#	or beyond.
#
#	Define BSD42 to change the location of the directory include file
#	from /usr/include/ndir.h to /usr/include/sys/dir.h
#
BSD	= -DBSD -DBSD42

#BSD	= -DBSD


OBJECTS =  \
	main.o \
	doname.o \
	misc.o \
	files.o \
	$(RULES).o \
	dosys.o \
	dyndep.o \
	prtmem.o \
	gram.o

CFLAGS = -O $(GETU) $(BSD) 

all:  make
	@echo MAKE is up to date.

make:  $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS)  $(LIBES) -o make


$(OBJECTS): defs.h ar.h ar11.h

install:
	install -c -s make $(DESTDIR)/bin/make 

clean cleanup:
	-rm -f *.o

print:
	list="defs main.c doname.c dosys.c files.c $(RULES).c misc.c dyndep.c prtmem.c gram.y make.mk test.mk test.mk?" \
	pr -o1 -w120 -l66 $list
