TROFF = /usr/ucb/vtroff
netintro:
	tbl netintro.n | $(TROFF) -ms
systemmanual:
	$(TROFF) -ms systemmanual.n;
ref:
	tbl ref.n | $(TROFF) -ms
doc:
	man new play >outp
	man new record >>outp
makehard:
	pr -h "Words to know and tell" -8 -w130 hardwords >hardwords.out
mannet:
	cd /usr/man/mann; $(TROFF) -man net*.n
