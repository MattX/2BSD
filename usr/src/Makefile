#
# Copyright (c) 1986 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	4.17	(2.11BSD)	2025/12/24
#
# This makefile is designed to be run as:
#	make build
#	make installsrc
# The `make build' will compile and install the libraries
# before building the rest of the sources. The `make installsrc'
# will then install the remaining binaries.
# 
# It can also be run in the more conventional way:
#	make
#	make install
# The `make' will compile everything without installing anything.
# The `make install' will then install everything. Note however
# that all the binaries will have been loaded with the old libraries.
#
# NOTE: The method of hostname lookup (hosts file or nameserver) is no
#	longer selected here.  Make sure to edit lib/libc/Makefile to set
#	HOSTLOOKUP

CFLAGS=	-O

# Programs that live in subdirectories and have makefiles of their own.
#
# 'share' has to be towards the front of the list because some programs
# need their data files installed first.

LIBDIR= lib usr.lib
SRCDIR=	share bin sbin etc games libexec local new ucb usr.bin usr.sbin man

all:	${LIBDIR} ${SRCDIR}

lib:	FRC
	cd lib/libc; make
	cd lib; make ccom cpp c2

usr.lib ${SRCDIR}: FRC
	cd $@; make

build: buildlib ${SRCDIR}

# 'cpp' depends on libvmf so build libvmf after libc and before cpp

buildlib: FRC
	@echo compiling libc.a
	cd lib/libc; make
	@echo installing /lib/libc.a
	cd lib/libc; make install
	@echo compiling usr.lib/libvmf.a
	cd usr.lib/libvmf; make
	@echo installing /usr/lib/libvmf.a
	cd usr.lib/libvmf; make install
	cd usr.lib/libvmf; make clean
	@echo
	@echo compiling C compiler
	cd lib; make ccom cpp c2
	@echo installing C compiler
	cd lib/ccom; make install
	cd lib/cpp; make install
	cd lib/c2; make install
	cd lib; make clean
	@echo
	@echo re-compiling libc.a
	cd lib/libc; make
	@echo re-installing /lib/libc.a
	cd lib/libc; make install
	@echo
	@echo re-compiling C compiler
	cd lib; make ccom cpp c2
	@echo re-installing C compiler
	cd lib/ccom; make install
	cd lib/cpp; make install
	cd lib/c2; make install
	@echo installing libkern
	cd lib/libkern; make  install
	@echo
	cd lib; make clean
	@echo compiling usr.lib
	cd usr.lib; make
	@echo installing /usr/lib
	cd usr.lib; make install
	cd usr.lib; make clean

FRC:

install:
	-for i in ${LIBDIR} ${SRCDIR}; do \
		(cd $$i; \
		make install); \
	done

installsrc:
	-for i in ${SRCDIR}; do \
		(cd $$i; \
		make install); \
	done

tags:
	for i in lib usr.lib; do \
		(cd $$i; make TAGSFILE=../tags tags); \
	done
	sort -u +0 -1 -o tags tags

clean:
	rm -f a.out core *.s *.o
	for i in ${LIBDIR} ${SRCDIR}; do (cd $$i; make -k clean); done
