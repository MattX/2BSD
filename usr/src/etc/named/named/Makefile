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
#	@(#)Makefile	4.14 (Berkeley) 2/28/88
#
SEPFLAG= -i
RES=
DEFINES=
#DEFINES= -DDEBUG -DSTATS
### -DALLOW_T_UNSPEC -Dmalloc=rt_malloc -Dfree=rt_free
### ALLOC=storage.o
CFLAGS=	-O ${DEFINES}
LDFLAGS= ${RES} ${SEPFLAG}
LIBC=	/lib/libc.a
SRCS=	db_dump.c db_load.c db_lookup.c db_reload.c db_save.c db_update.c \
	ns_forw.c ns_init.c ns_main.c ns_maint.c ns_req.c ns_resp.c \
	ns_sort.c ns_stats.c
HDRS=	db.h ns.h
BASE=	db_lookup.o db_reload.o db_save.o db_update.o \
	ns_forw.o ns_resp.o ns_main.o ns_req.o ns_sort.o ns_maint.o
OBJS1=	db_dump.o ns_init.o
OBJS2=	db_load.o ns_stats.o
OBJS=	${BASE} ${OBJS1} ${OBJS2} ${ALLOC}
SUBDIR=	tools

all: named

named:	${OBJS} ${LIBC}
	sh newvers.sh
# You need overlays if you define DEBUG and STATS.
#	${CC} ${LDFLAGS} -o $@ version.c ${BASE} -Z ${OBJS1} -Z ${OBJS2} -Y
	${CC} ${LDFLAGS} -o $@ version.c ${OBJS}

clean: FRC
	rm -f ${OBJS} core named version.o version.c
	for i in ${SUBDIR}; do \
		(cd $$i; make ${MFLAGS} clean); \
	done

depend: FRC
	mkdep ${CFLAGS} ${SRCS}

install: FRC
	install -s -o root -g staff  -m 751 named ${DESTDIR}/etc/named

lint: FRC
	lint -Dlint ${CFLAGS} ${SRCS}

tags: FRC
	ctags -t ${SRCS} ${HDRS}

FRC:

# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

db_dump.o: db_dump.c /usr/include/sys/param.h /usr/include/sys/localopts.h
db_dump.o: /usr/include/machine/machparam.h /usr/include/signal.h
db_dump.o: /usr/include/sys/types.h /usr/include/sys/time.h /usr/include/time.h
db_dump.o: /usr/include/sys/stat.h /usr/include/netinet/in.h
db_dump.o: /usr/include/short_names.h /usr/include/netdb.h
db_dump.o: /usr/include/short_names.h /usr/include/stdio.h
db_dump.o: /usr/include/syslog.h /usr/include/arpa/nameser.h ns.h shortnames.h
db_dump.o: /usr/include/strings.h /usr/include/arpa/inet.h
db_dump.o: /usr/include/short_names.h db.h
db_load.o: db_load.c /usr/include/sys/param.h /usr/include/sys/localopts.h
db_load.o: /usr/include/machine/machparam.h /usr/include/signal.h
db_load.o: /usr/include/sys/types.h /usr/include/sys/time.h /usr/include/time.h
db_load.o: /usr/include/sys/stat.h /usr/include/netinet/in.h
db_load.o: /usr/include/short_names.h /usr/include/stdio.h
db_load.o: /usr/include/syslog.h /usr/include/ctype.h /usr/include/netdb.h
db_load.o: /usr/include/short_names.h /usr/include/arpa/nameser.h ns.h
db_load.o: shortnames.h /usr/include/strings.h /usr/include/arpa/inet.h
db_load.o: /usr/include/short_names.h db.h
db_lookup.o: db_lookup.c shortnames.h /usr/include/sys/types.h
db_lookup.o: /usr/include/stdio.h /usr/include/arpa/nameser.h db.h
db_reload.o: db_reload.c /usr/include/sys/types.h /usr/include/sys/time.h
db_reload.o: /usr/include/time.h /usr/include/netinet/in.h
db_reload.o: /usr/include/short_names.h /usr/include/stdio.h
db_reload.o: /usr/include/syslog.h /usr/include/arpa/nameser.h ns.h
db_reload.o: shortnames.h /usr/include/strings.h /usr/include/arpa/inet.h
db_reload.o: /usr/include/short_names.h db.h
db_save.o: db_save.c shortnames.h /usr/include/sys/types.h /usr/include/stdio.h
db_save.o: /usr/include/syslog.h /usr/include/arpa/nameser.h db.h
db_update.o: db_update.c /usr/include/sys/types.h /usr/include/sys/socket.h
db_update.o: /usr/include/short_names.h /usr/include/sys/time.h
db_update.o: /usr/include/time.h /usr/include/netinet/in.h
db_update.o: /usr/include/short_names.h /usr/include/stdio.h
db_update.o: /usr/include/syslog.h /usr/include/arpa/nameser.h ns.h
db_update.o: shortnames.h /usr/include/strings.h /usr/include/arpa/inet.h
db_update.o: /usr/include/short_names.h db.h
ns_forw.o: ns_forw.c /usr/include/stdio.h /usr/include/sys/param.h
ns_forw.o: /usr/include/sys/localopts.h /usr/include/machine/machparam.h
ns_forw.o: /usr/include/signal.h /usr/include/sys/types.h
ns_forw.o: /usr/include/sys/time.h /usr/include/time.h
ns_forw.o: /usr/include/sys/socket.h /usr/include/short_names.h
ns_forw.o: /usr/include/netinet/in.h /usr/include/short_names.h
ns_forw.o: /usr/include/syslog.h /usr/include/arpa/nameser.h ns.h shortnames.h
ns_forw.o: /usr/include/strings.h /usr/include/arpa/inet.h
ns_forw.o: /usr/include/short_names.h db.h
ns_init.o: ns_init.c /usr/include/sys/types.h /usr/include/sys/socket.h
ns_init.o: /usr/include/short_names.h /usr/include/sys/time.h
ns_init.o: /usr/include/time.h /usr/include/netinet/in.h
ns_init.o: /usr/include/short_names.h /usr/include/stdio.h /usr/include/errno.h
ns_init.o: /usr/include/signal.h /usr/include/syslog.h /usr/include/ctype.h
ns_init.o: /usr/include/arpa/nameser.h ns.h shortnames.h /usr/include/strings.h
ns_init.o: /usr/include/arpa/inet.h /usr/include/short_names.h db.h
ns_main.o: ns_main.c /usr/include/sys/param.h /usr/include/sys/localopts.h
ns_main.o: /usr/include/machine/machparam.h /usr/include/signal.h
ns_main.o: /usr/include/sys/types.h /usr/include/sys/file.h
ns_main.o: /usr/include/sys/time.h /usr/include/time.h /usr/include/sys/wait.h
ns_main.o: /usr/include/sys/resource.h /usr/include/sys/ioctl.h
ns_main.o: /usr/include/sys/ttychars.h /usr/include/sys/ttydev.h
ns_main.o: /usr/include/sys/socket.h /usr/include/short_names.h
ns_main.o: /usr/include/netinet/in.h /usr/include/short_names.h
ns_main.o: /usr/include/net/if.h /usr/include/net/if_arp.h /usr/include/stdio.h
ns_main.o: /usr/include/syslog.h /usr/include/errno.h /usr/include/signal.h
ns_main.o: /usr/include/netdb.h /usr/include/short_names.h
ns_main.o: /usr/include/arpa/nameser.h /usr/include/arpa/inet.h
ns_main.o: /usr/include/short_names.h /usr/include/resolv.h ns.h shortnames.h
ns_main.o: /usr/include/strings.h /usr/include/arpa/inet.h
ns_main.o: /usr/include/short_names.h db.h
ns_maint.o: ns_maint.c /usr/include/sys/param.h /usr/include/sys/localopts.h
ns_maint.o: /usr/include/machine/machparam.h /usr/include/signal.h
ns_maint.o: /usr/include/sys/types.h /usr/include/sys/socket.h
ns_maint.o: /usr/include/short_names.h /usr/include/sys/time.h
ns_maint.o: /usr/include/time.h /usr/include/netinet/in.h
ns_maint.o: /usr/include/short_names.h /usr/include/stdio.h
ns_maint.o: /usr/include/syslog.h /usr/include/signal.h /usr/include/errno.h
ns_maint.o: /usr/include/arpa/nameser.h ns.h shortnames.h
ns_maint.o: /usr/include/strings.h /usr/include/arpa/inet.h
ns_maint.o: /usr/include/short_names.h db.h
ns_req.o: ns_req.c /usr/include/stdio.h /usr/include/sys/param.h
ns_req.o: /usr/include/sys/localopts.h /usr/include/machine/machparam.h
ns_req.o: /usr/include/signal.h /usr/include/sys/types.h /usr/include/sys/uio.h
ns_req.o: /usr/include/sys/time.h /usr/include/time.h /usr/include/sys/socket.h
ns_req.o: /usr/include/short_names.h /usr/include/netinet/in.h
ns_req.o: /usr/include/short_names.h /usr/include/syslog.h
ns_req.o: /usr/include/sys/file.h /usr/include/arpa/nameser.h ns.h shortnames.h
ns_req.o: /usr/include/strings.h /usr/include/arpa/inet.h
ns_req.o: /usr/include/short_names.h db.h
ns_resp.o: ns_resp.c /usr/include/stdio.h /usr/include/sys/param.h
ns_resp.o: /usr/include/sys/localopts.h /usr/include/machine/machparam.h
ns_resp.o: /usr/include/signal.h /usr/include/sys/types.h
ns_resp.o: /usr/include/sys/time.h /usr/include/time.h
ns_resp.o: /usr/include/sys/socket.h /usr/include/short_names.h
ns_resp.o: /usr/include/sys/file.h /usr/include/netinet/in.h
ns_resp.o: /usr/include/short_names.h /usr/include/syslog.h
ns_resp.o: /usr/include/arpa/nameser.h ns.h shortnames.h /usr/include/strings.h
ns_resp.o: /usr/include/arpa/inet.h /usr/include/short_names.h db.h
ns_sort.o: ns_sort.c /usr/include/stdio.h /usr/include/sys/types.h
ns_sort.o: /usr/include/sys/time.h /usr/include/time.h
ns_sort.o: /usr/include/sys/socket.h /usr/include/short_names.h
ns_sort.o: /usr/include/sys/file.h /usr/include/netinet/in.h
ns_sort.o: /usr/include/short_names.h /usr/include/syslog.h
ns_sort.o: /usr/include/arpa/nameser.h ns.h shortnames.h /usr/include/strings.h
ns_sort.o: /usr/include/arpa/inet.h /usr/include/short_names.h db.h
ns_stats.o: ns_stats.c

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
