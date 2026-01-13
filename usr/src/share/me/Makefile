#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	6.1.2 (2.11BSD) 2025/11/25
#
#  Makefile for -me macros
#
#	This makefile strips comments from the source files and
#	installs them in the expected directories.
#
LIB =	/usr/share/me

SRCS =	tmac.e acm.me chars.me deltext.me eqn.me \
	float.me footnote.me index.me local.me null.me \
	refer.me sh.me tbl.me thesis.me

MISC = 	summary.extern test.accent test.umlaut

all: revisions $(SRCS)

install: revisions $(SRCS)
	-if [ ! -d ${LIB} ]; then \
		rm -f ${LIB}; \
		mkdir ${LIB}; \
	fi
	-for i in ${SRCS}; do \
		sed -e '/%beginstrip%/{' \
		    -e 'h' \
		    -e 's/.*/.\\" This version has had comments stripped; an unstripped version is available./p' \
		    -e 'g' \
		    -e '}' \
		    -e '/%beginstrip%/,$$s/[. 	][ 	]*\\".*//' \
		    -e '/^$$/d' \
		    -e '/\\n@/d' $$i > _mac_tmp_; \
		install -m 0444 _mac_tmp_ ${LIB}/$$i; \
	done
	-mkdir -p /usr/share/tmac
	mv ${LIB}/tmac.e /usr/share/tmac/tmac.e
	install -c -m 0444 revisions ${LIB}/revisions
	install -c -m 444 ${MISC} ${LIB}

clean:
