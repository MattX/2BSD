#
# Makefile for the system autoconfiguration program
#

CFILES=		args.c attach.c do_config.c main.c read_dtab.c read_nlist.c \
		uprobe1.c uprobe2.c
OBJS=		args.o attach.o do_config.o main.o read_dtab.o read_nlist.o \
		uprobe1.o uprobe2.o
CFLAGS=		-O -DGOO
LDFLAGS=
INCLUDES=	/usr/include

all:		autoconfig

autoconfig:	$(OBJS)
		cc $(LDFLAGS) $(OBJS) -o autoconfig

cp:		all
		cp autoconfig ${DESTDIR}/etc
		-strip ${DESTDIR}/etc/autoconfig

clean:
		rm -f autoconfig *.o

