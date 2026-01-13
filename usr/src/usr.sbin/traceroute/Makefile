#       @(#)Makefile    1.1 (2.11BSD) 2025/11/28

CFLAGS = -O -i
LDFLAGS = -i

all: traceroute traceroute.0

traceroute: traceroute.o
	cc $(LDFLAGS) -o traceroute traceroute.o

install: traceroute traceroute.0
	install -c -s -o root -g staff -m 4551 traceroute /usr/sbin/traceroute 
	install -c -m 444 -o root -g staff traceroute.0 /usr/man/cat8

traceroute.0: traceroute.8
	/usr/man/manroff traceroute.8 > traceroute.0

clean:
	rm -f *.o traceroute traceroute.0 core

lint:
	lint -b -h -x traceroute.c | \
	  grep -v 'possible pointer alignment problem'
