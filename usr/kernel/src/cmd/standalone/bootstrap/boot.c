#include <sys/param.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/filsys.h>
#include <sys/dir.h>
#include "../saio.h"

#define	RO	02		/* access abilities */
#define	KISA6	(int *)0172354

extern int cputype;
char module[] = "Boot";		/* name of this program-- used by trap */
char line[100];

main()
{
int i, j;

	segflag = 2;

	printf("\n%d%s\n",cputype,module);
	do {
		do {
			printf(": "); gets(line);
			i = open(line,0);
		} while (i < 0);

		j=copyunix(i);
		close(i);

	} while (j < 0);

}


int
copyunix(io)
register io;
{
register int addr,s;
long phys, ovaddr;
unsigned	txtsiz,datsiz,bsssiz,ovsize,ovsizes[8];
int	magic;
long data;
int	disc;
int i;


	lseek(io, (off_t)0, 0);
	magic = getw(io);
	txtsiz = getw(io);
	datsiz = getw(io);
	bsssiz = getw(io);
	disc = 0;

	if( iob[io-3].i_flgs&F_FILE)
		disc++;

	switch (magic) {
	case 0411:
		if(cputype == 40)
		{
			printf("Can't load sep i/d prog's\n");
			return ( -1);
		}
		if( (data = (long) datsiz+bsssiz) > 56*1024L)
			if ( data > 64*1024L)
				printf("Warning: too big to run on pdp11\n");
			else
				printf("Warning: unix too big by %d bytes \n",
					data - 56*1024L);
		else
			printf("\n");
		setseg(0);
		setsep();	/* re-enable sep i/d */
		lseek(io, (long)(020+txtsiz), 0);
		if (disc) printf("%d+",txtsiz);
			/* actually the data is loading now */
		for(addr=0; addr!=datsiz; addr+=2)  {
			mtpi(getw(io),addr);
		}

		if (disc) printf("%d+",datsiz);
		clrseg(addr,bsssiz);

		phys = (long)datsiz + (long)bsssiz + 63L;
		phys =/ 64;
		setseg((int)phys);

		lseek(io, 020L, 0);

		if (disc) printf("%d\n",bsssiz);
		for(addr=0; addr!=txtsiz; addr+=2) {
			mtpi(getw(io),addr);
		}
		return (0);
	case 0407:
		if( (data = (long) datsiz+bsssiz) > 48*1024L)
			if ( data > 56*1024L)
				printf("Warning: too big to run on pdp11\n");
			else
				printf("Warning: unix too big by %d bytes \n",
					data - 48*1024L);
		else
			printf("\n");
		if(cputype != 40)
		{
			setseg(0);
			setnosep();
		}
		/*
		 * space over the header. We do this instead of seeking
		 * because the input might be a tape which doesn't know 
		 * how to seek.
		 */
		getw(io); getw(io); getw(io); getw(io);
		phys = (long)txtsiz+(long)datsiz;
		if (disc) printf("%d+%d+",txtsiz,datsiz);
		for (addr = 0; addr !=(int) phys; addr += 2)
			mtpi(getw(io),addr);
		if (disc) printf("%d\n",bsssiz);
		clrseg(addr, bsssiz);
		return(0);

	case 0430:	/* overlaid text */
		if (cputype != 40) {
			setnosep();
		}
		/*
		 * check for consistency
		 */
		if (txtsiz < 020000) {
			printf("text segment too small (%d)\n", txtsiz);
			exit(1);
		}
		if (txtsiz > 040000) {
			printf("text segment too big (%d)\n", txtsiz);
			exit(1);
		}
		if (datsiz + bsssiz > 060000) {
			printf("data segment too big (%d)\n", datsiz + bsssiz);
			exit(1);
		}

		if (disc) printf("%d+(",txtsiz);
		lseek(io, (long) 020, 0);	/* skip to overlay header */
		for (i = 0, ovsize = 0; i < 8; i++) {
			ovsizes[i] = getw(io);
			ovsize += ovsizes[i];
		}
		ovsize -= ovsizes[0];
		if (ovsizes[0] > 020000) {
			if (disc) printf("max overlay too big (0%o)\n", ovsizes[0]);
			exit(1);
		}
		ovsizes[0] = 0;
		/*
		 * load text segment at zero
		 */
		setseg(0);
		for (addr = 0; addr < txtsiz; addr +=2)
			mtpi(getw(io), addr);
		/*
		 * load data segment at 24Kb
		 */
		phys = 060000L;
		setseg((int)(phys/64));
		lseek(io, 040L + (long)txtsiz + (long)ovsize, 0);
		for (addr = 0; addr < datsiz; addr += 2)
			mtpi(getw(io), addr);
		/*
		 * clear bss
		 */
		clrseg(addr, bsssiz);
		phys += (long)datsiz + (long)bsssiz;
		phys = (phys + 077L) & ~077L;
		/*
		 * load the overlays after the bss
		 * except for overlay 1, which is loaded at 16Kb
		 */
		lseek(io, (long)(040+txtsiz), 0);
		ovaddr = phys;
		for (i = 1; i < 8; i++) {
			if (i == 1)
				setseg(040000/64);
			else {
				setseg((int)(phys/64));
				phys += ovsizes[i];
			}
			if ( i == 7)
			{
				if (disc) printf("%d)+",ovsizes[i]);
			}
			else
			{
				if (disc) printf("%d+",ovsizes[i]);
			}
			for (addr = 0; addr < ovsizes[i]; addr += 2)
				mtpi(getw(io), addr);
		}

		/*
		 * build the overlay address and descriptor
		 * tables for the kernel.
		 */
		setseg(0);
		addr = 0402;	/* overlay tables start here */
		for (i = 1; i < 8; i++) {
			if (i == 1)
				mtpi(040000/64, addr);
			else {
				mtpi((int)(ovaddr/64), addr);
				ovaddr += ovsizes[i];
			}
			addr += 2;
		}
		for (i = 0; i < 8; i++) {
			if (ovsizes[i] != 0)
				mtpi((((ovsizes[i]/64)-1)<<8) | RO, addr);
			else
				mtpi(0, addr);
			addr += 2;
		}
		mtpi((int)(phys/64), addr);	/* start of free memory */
		if (disc) printf("%d+%d\n",datsiz,bsssiz);
		/*
		 * Set KISA6 now, or systrap to kernel mode will
		 * clobber overlay text at 0140000.
		 */
		*KISA6 = (int)(phys/64);
		return(0);

	default:
		printf("File type %o is unrecognizable\n", magic);
		printf("cannot load.\n");
		return(-1);
	}
}
