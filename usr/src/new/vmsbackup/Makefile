#       Makefile        2.0 (2.11BSD) 2025/11/25

#REMOTE=-DREMOTE			# -DREMOTE  use remote tape
SWAP=				# -DSWAP swap bytes
CFLAGS= $(SWAP) $(REMOTE) -O
LDFLAGS=-i
OWNER=bin			# user for remote tape access
MODE=755
BINDIR=/usr/new
MANDIR=/usr/new/man/cat1
MAN=	vmsbackup.0
MANSRC=	vmsbackup.1

all: vmsbackup vmsbackup.0

vmsbackup: vmsbackup.o match.o
	cc $(LDFLAGS) -o vmsbackup vmsbackup.o match.o $(LIBS)

install: vmsbackup $(MAN)
	install -c -m $(MODE) -o $(OWNER) -s vmsbackup $(BINDIR)	
	install -c -m 0444 -o root -g staff vmsbackup.0 $(MANDIR)

vmsbackup.0: $(MANSRC)
	/usr/man/manroff $(MANSRC) > $(MAN)

clean:
	rm -f vmsbackup *.o core $(MAN)

shar:
	shar -a README vmsbackup.1 Makefile vmsbackup.c match.c \
	    > vmsbackup.shar
