##########################################################################
#   Instructions to Make, for compilation of inet processes
##########################################################################

CC      =	cc
CFLAGS  =	-O   -DUCI -DBSD42
LFLAGS  =	-bhx -DUCI -DBSD42
LDFLAGS =	-ns
LINT    =	lint
LIBES   =
LLIBS   =
ADMDIR	=	/admin/etc
ETCDIR	=	/usr/support/etc
PUBDIR	=	/usr/uci


##################################################################
# Here it is...
##################################################################

all:		inetd udps timeck whois hname
inst-all:	inst-inetd inst-udps inst-timeck inst-whois inst-hname
l-all:		l-inetd l-whois


##################################################################
# inetd
##################################################################

inst-inetd:	$(ADMDIR)/inetd

$(ADMDIR)/inetd:	xinetd
		-cp $@ zinetd
		-rm -f $@
		cp xinetd $@
		-@ls -gls $@
		-@echo ""

inetd:		xinetd

xinetd:		inetd.o dtime.o syslog.o $(LIBES)
		$(CC) $(LDFLAGS) -o $@ inetd.o dtime.o syslog.o $(LIBES)

l-inetd:;	$(LINT) $(LFLAGS) inetd.c $(LLIBS)


##################################################################
# udps
##################################################################

inst-udps:	$(ETCDIR)/udps

$(ETCDIR)/udps:	xudps
		-cp $@ zudps
		cp xudps $@
		-@ls -gls $@
		-@echo ""

udps:		xudps

xudps:		udps.o $(LIBES)
		$(CC) $(LDFLAGS) -o $@ udps.o $(LIBES)


##################################################################
# timeck
##################################################################

inst-timeck:	$(ADMDIR)/timeck

$(ADMDIR)/timeck:	xtimeck
		-cp $@ ztimeck
		-chmod u-s ztimeck
		cp xtimeck $@
		chown root $@
		chmod u+s $@
		-@ls -gls $@
		-@echo ""

timeck:		xtimeck

xtimeck:	timeck.o dtime.o $(LIBES)
		$(CC) $(LDFLAGS) -o $@ timeck.o dtime.o $(LIBES)


##################################################################
# whois
##################################################################

inst-whois:	$(PUBDIR)/whois

$(PUBDIR)/whois:	xwhois
		-cp $@ zwhois
		cp xwhois $@
		-@ls -gls $@
		-@echo ""

whois:		xwhois

xwhois:		whois.o
		$(CC) $(LDFLAGS) -o $@ whois.o

l-whois:;	$(LINT) $(LFLAGS) whois.c


##################################################################
# hname
##################################################################

inst-hname:	$(ETCDIR)/hname

$(ETCDIR)/hname:	xhname
		-cp $@ zhname
		cp xhname $@
		-@ls -gls $@
		-@echo ""

hname:		xhname

xhname:		Makefile
		echo 'exec $(PUBDIR)/whois -p hostnames hname $$*' > hname.sh
		chmod +x hname.sh
		cp hname.sh $@

clean:
	rm -f *.[os] core
