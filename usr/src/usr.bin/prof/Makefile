#	@(#)Makefile	4.2 (2.11BSD) 2025/11/27

# If you don't want to plot, take out the -Dplot and -lplot
PLOT = -lplot
CFLAGS=-O -Dplot
LDFLAGS=-i
prof:	prof.o
	$(CC) $(LDFLAGS) -o prof prof.o $(PLOT)

prof.o: prof.c
	cc -c $(CFLAGS) prof.c

install: prof
	install -c -s -m 755 prof /usr/bin

clean :
	rm -f *.o prof
