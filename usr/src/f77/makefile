#	Makefile for the Fortran 77 compiler
#	running on a nonseparate i/d PDP11 using text overlays,
#	generating code for the PDP11,
#	using the Ritchie pass 2
#
#	Modified for non fp machines SRS 2/25/81
#
AS = ovas
CC = cc -V
CFL = -DTARGET=PDP11 -DFAMILY=DMR -DHERE=PDP11 -DOUTPUT=BINARY \
	 -DPOLISH=POSTFIX -DOVERLAID
# note: optimizer called below. don't define -O here, it
# 	causes some problems.
#CFLAGS = -s -f $(CFL)
CFLAGS = -s $(CFL)
OPTIMIZER = /lib/c2
LD = ovld
LDFLAGS = -X -n
LIBR = -lovc
STRINGS = f77_strings

OBJECTS = main.o init.o gram.o lex.o proc.o \
	  equiv.o data.o expr.o exec.o intr.o io.o misc.o error.o\
	  put.o putdmr.o pdp11.o pdp11x.o
PASS1 = f77pass1
DRIVER = f77



all:	${DRIVER} ov${DRIVER} ${PASS1}

cp:	all
	cp ${DRIVER} /bin
	cp ov${DRIVER} /bin
	cp ${PASS1} /lib
	cp ${STRINGS} /lib
	rm ${STRINGS} *.o  ${DRIVER} ${PASS1} ov${DRIVER}

cmp:	all
	cmp ${DRIVER} /bin/${DRIVER}
	cmp ov${DRIVER} /bin/ov${DRIVER}
	cmp ${PASS1} /lib/${PASS1}
	cmp ${STRINGS} /lib
	rm *.o  ${DRIVER} ${PASS1} ov${DRIVER} ${STRINGS}



driver.o :
	cc -c ${CFLAGS} $*.c


# Take certain data tables out of data space and put them in
# text space (warning-- this does'nt work except with special cases)

# edit the yacc data structures out.
gram.o:	gram.c
#	Remove error strings to save space - SRS 5-1-81
	mkstr - f77_strings xx gram.c
	${CC} -S ${CFLAGS} xxgram.c 
	mv xxgram.s gram.s
	ed - gram.s < :gram.fix
#	${OPTIMIZER} gram.s x.s
#	mv x.s gram.s
	${AS} -u -o gram.o gram.s
	rm gram.s



# In general move the switch code from data to instruction space.
.c.o:	$*.s
#	Remove error strings to save space - SRS 5-1-81
	mkstr - f77_strings xx $*.c
	${CC} -S ${CFLAGS} xx$*.c
	mv xx$*.s $*.s
	ed - $*.s < :rofix
	rm xx$*.c
	${OPTIMIZER} $*.s x.s
	mv x.s $*.s
	${AS} -u -o $*.o $*.s
	rm $*.s

.c.s:	
#	Remove error strings to save space - SRS 5-1-81
	mkstr - f77_strings xx $*.c
	${CC} -S ${CFLAGS} xx$*.c
	mv xx$*.s $*.s
	ed - $*.s < :rofix
	rm xx$*.c

.s.o:
	${OPTIMIZER} $*.s x.s
	mv x.s $*.s
	${AS} -u -o $*.o $*.s
	rm $*.s

compiler :: ${DRIVER} ${PASS1} ov${DRIVER}
	touch compiler

compiler :: /lib/fovc1
	@echo "***Warning: /lib/fovc1 has changed"

${DRIVER} : driver.c pdp11x.c 
	cc -n $(CFLAGS) driver.c pdp11x.c -o ${DRIVER}
	@size ${DRIVER}

ov${DRIVER} : driver.c pdp11x.c drivedefs
	cc -n $(CFLAGS) -DOVDRIVER driver.c pdp11x.c -o ov${DRIVER}
	@size ov${DRIVER}

${PASS1} : $(OBJECTS)
	@echo LOAD
	${LD} /lib/crt0.o ${LDFLAGS} -o ${PASS1} \
	-Z main.o init.o equiv.o data.o io.o \
	-Z gram.o \
	-Z lex.o proc.o \
	-Z expr.o \
	-Z exec.o intr.o \
	-Z putdmr.o pdp11.o pdp11x.o \
	-L misc.o error.o put.o ${LIBR}
	@size ${PASS1}

#	This section removed, since we do not have a functional yacc.
#gram.c:	gram.head gram.dcl gram.expr gram.exec gram.io tokdefs
#
#	( sed <tokdefs "s/#define/%token/" ;\
#		cat gram.head gram.dcl gram.expr gram.exec gram.io ) >gram.in
#	$(YACC) $(YFLAGS) gram.in
#	mv y.tab.c gram.c
#	rm gram.in

tokdefs: tokens
	grep -n . <tokens | sed "s/\([^:]*\):\(.*\)/#define \2 \1/" >tokdefs

# Dependencies
lex.o : tokdefs string_defs
driver.o $(OBJECTS)  : defs defines locdefs ftypes
driver.o : drivedefs
locdefs : pdp11defs
	cp pdp11defs locdefs
put.o putdmr.o pdp11.o : dmrdefs string_defs
io.o : fio.h string_defs

lint:
	@echo "LINT DRIVER" >Lintout
	@lint -p $(CFL) driver.c pdp11x.c >>Lintout
	echo "LINT PASS1" >>Lintout
	@nohup lint -p $(CFL) error.c exec.c intr.c expr.c gram.c init.c io.c\
		lex.c main.c proc.c misc.c put.c putdmr.c pdp11.c pdp11x.c\
		equiv.c data.c >>Lintout&

cleanup:
	-rm f *.o
	du

install: /bin/${DRIVER} /lib/${PASS1} /bin/ov${DRIVER} /lib/${STRINGS}

/bin/${DRIVER} : ${DRIVER}
	strip ${DRIVER}
	@size ${DRIVER} /bin/${DRIVER}
	cp ${DRIVER} /bin/${DRIVER}

/bin/ov${DRIVER} : ov${DRIVER}
	strip ov${DRIVER}
	@size ov${DRIVER} /bin/ov${DRIVER}
	cp ov${DRIVER} /bin/ov${DRIVER}

/lib/${PASS1} : ${PASS1}
	strip ${PASS1}
	@size ${PASS1} /lib/${PASS1}
	-chmod u-t /lib/${PASS1}
	@/lib/${PASS1} 2>/dev/null
	cp ${PASS1} /lib/${PASS1}
	echo "***Now do a    chmod u+t /lib/f77pass1     in su mode"

/lib/${STRINGS} : ${STRINGS}
	@ls -l ${STRINGS} /lib/${STRINGS}
	cp ${STRINGS} /lib/${STRINGS}

