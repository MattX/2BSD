INCL =
CFLAGS = -O -i
LIBS =

traceroute: traceroute.c
	cc $(CFLAGS) $(INCL) -o traceroute traceroute.c $(LIBS)

install: traceroute traceroute.0
	install -s -o root -g bin -m 4551 traceroute ${DESTDIR}/etc/traceroute 
	install -m 444 -o bin -g bin traceroute.0 ${DESTDIR}/usr/new/man/cat8

traceroute.0: traceroute.8
	/usr/man/manroff traceroute.8 > traceroute.0

clean:
	rm -f *.o traceroute traceroute.0 core

lint:
	lint -b -h -x $(INCL) traceroute.c | \
	  grep -v 'possible pointer alignment problem'
