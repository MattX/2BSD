h52553
s 00044/00000/00000
d D 1.1 81/02/25 18:16:26 dlw 1 0
c date and time created 81/02/25 18:16:26 by dlw
e
u
U
t
T
I 1
# assemble man pages
#	%W%	%G%

MANDIR = /usr/ucb/man/man3
CATDIR = /usr/ucb/man/cat3

MANUAL = \
	access.3f \
	chdir.3f \
	etime.3f \
	fdate.3f \
	flush.3f \
	fork.3f \
	fseek.3f \
	getarg.3f \
	getc.3f \
	getenv.3f \
	getlog.3f \
	getpid.3f \
	getuid.3f \
	idate.3f \
	kill.3f \
	link.3f \
	loc.3f \
	perror.3f \
	putc.3f \
	qsort.3f \
	signal.3f \
	sleep.3f \
	stat.3f \
	system.3f \
	time.3f \
	unlink.3f \
	wait.3f

man.3.f77:	$(MANUAL)
	@rm -f man.3.f77
	@nroff -man $(MANUAL) > man.3.f77
	@echo "" > /dev/tty

install:
	@cp $(MANUAL) $(MANDIR)
	@for i in $(MANUAL); do nroff -man $$i > $(CATDIR)/$$i; done
	@echo "" > /dev/tty
E 1
