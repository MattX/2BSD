OBJS=		fp1.o \
		fp2.o \
		fp3.o \
		fpx.o

LIBC=		/lib/libc.a
FPSIM=		fpsim.o
RM=		-rm

$(FPSIM):	$(OBJS)
		ar rb fakfp.o $(LIBC) $(FPSIM)
		ranlib $(LIBC)

clean:
		$(RM) $(FPSIM) $(OBJS)
