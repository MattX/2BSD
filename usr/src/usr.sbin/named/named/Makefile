#
# Copyright (c) 1987 Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms are permitted
# provided that this notice is preserved and that due credit is given
# to the University of California at Berkeley. The name of the University
# may not be used to endorse or promote products derived from this
# software without specific prior written permission. This software
# is provided ``as is'' without express or implied warranty.
#
#	@(#)Makefile	4.14.2 (2.11BSD) 2025/11/29
#
RES=
DEFINES=
#DEFINES= -DDEBUG -DSTATS
### -DALLOW_T_UNSPEC -Dmalloc=rt_malloc -Dfree=rt_free
### ALLOC=storage.o
CFLAGS=	-O ${DEFINES}
LDFLAGS= -i ${RES}
SRCS=	db_dump.c db_load.c db_lookup.c db_reload.c db_save.c db_update.c \
	ns_forw.c ns_init.c ns_main.c ns_maint.c ns_req.c ns_resp.c \
	ns_sort.c ns_stats.c
HDRS=	db.h ns.h
BASE=	db_lookup.o db_reload.o db_save.o db_update.o \
	ns_forw.o ns_resp.o ns_main.o ns_req.o ns_sort.o ns_maint.o
OBJS1=	db_dump.o ns_init.o
OBJS2=	db_load.o ns_stats.o
OBJS=	${BASE} ${OBJS1} ${OBJS2} ${ALLOC}

all: named

named:	${OBJS}
	sh newvers.sh
# You need overlays if you define DEBUG and STATS.
#	${CC} ${LDFLAGS} -o named ${BASE} -Z ${OBJS1} -Z ${OBJS2} -Y version.c
	${CC} ${LDFLAGS} -o named ${OBJS} version.c

clean: FRC
	rm -f ${OBJS} core named version.o version.c

depend: FRC
	mkdep ${CFLAGS} ${SRCS}

install: named
	install -c -s -o root -g staff -m 755 named /usr/sbin/named

lint: FRC
	lint -Dlint ${CFLAGS} ${SRCS}

tags: FRC
	ctags -t ${SRCS} ${HDRS}

FRC:
