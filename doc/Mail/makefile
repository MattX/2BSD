TBL=tbl
VTROFF=/bin/csh /usr/ucb/vtroff

vpr:
	$(TBL) mail?.nr | $(VTROFF) -me

lpr:
	$(TBL) mail?.nr | nroff -me -Tlpr | lpr
