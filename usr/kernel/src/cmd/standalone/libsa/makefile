DRIVERS =   prf.o\
	    xp.o\
	    ht.o\
	    rk.o\
	    rl.o\
	    rp.o\
	    hk.o\
	    ts.o\
	    dv.o\
	    tm.o

CFLAGS = -O -I../../../../include

../libsa.a:	SYS.o ${DRIVERS} 
		ar rv ../libsa.a $?
