OBJS= bsdtcp.o df2.o df12.o dk.o dn.o hys.o hysq.o mic.o nov.o pen.o pnet.o \
	rvmacs.o sy.o unet.o va212.o va811.o va820.o vad.o vent.o vmacs.o

aculib.a: ${OBJS}
	ar cr aculib.a ${OBJS}
	-ranlib aculib.a 2>&1 | sed '/no symbol table/d'

${OBJS}: ../uucp.h ../condevs.h

clean:
	rm -f *.o core errs  aculib.a

.c.o:
	${CC} ${CFLAGS} -c $*.c

bsdtcp.o:	bsdtcp.c
df12.o:		df12.c
df2.o:		df2.c
dk.o:		dk.c
dn.o:		dn.c
hys.o:		hys.c
hysq.o:		hysq.c
mic.o:		mic.c
nov.o:		nov.c
pen.o:		pen.c
pnet.o:		pnet.c
rvmacs.o:	rvmacs.c
sy.o:		sy.c
unet.o:		unet.c
va212.o:	va212.c
va811.o:	va811.c
va820.o:	va820.c
vad.o:		vad.c
vent.o:		vent.c
vmacs.o:	vmacs.c
