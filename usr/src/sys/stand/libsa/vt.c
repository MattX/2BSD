/*
 * Virtual tape driver - copyright 1998/2000 Warren Toomey wkt@cs.adfa.edu.au
 *
 * $Revision: 1.13 $
 * $Date: 2001/03/25 00:18:10 $
 */

#include <sys/param.h>
#include <sys/inode.h>
#include "saio.h"

#if 0 /* struct not actually used */
/* Command sent to tape server */
struct vtcmd {
    char hdr1;			/* Header, 31 followed by 42 (decimal) */
    char hdr2;
    char cmd;			/* Command, one of VTC_XXX below */
				/* Error comes back in top 4 bits */
    char record;		/* Record we're accessing */
    char blklo;			/* Block number, in lo/hi format */
    char blkhi;
    char sum0;			/* 16-bit checksum */
    char sum1;
};
#endif

/* Header bytes */
#define VT_HDR1		31
#define VT_HDR2		42

/* Client commands available */
#define VTC_QUICK	0	/* Quick read, no cksum sent */
#define VTC_OPEN	1
#define VTC_CLOSE	2
#define VTC_READ	3
#define VTC_WRITE	4
#define VTC_ZEROREAD    6

/* Errors returned */
#define VTE_NOREC	1	/* No such record available */
#define VTE_OPEN	2	/* Can't open requested block */
#define VTE_CLOSE	3	/* Can't close requested block */
#define VTE_READ	4	/* Can't read requested block */
#define VTE_WRITE	5	/* Can't write requested block */
#define VTE_NOCMD       6       /* No such command */
#define VTE_EOF         7       /* End of file: no blocks left to read */

#define BLKSIZE         512

/* Static things */
char *vtbuf;			/* Pointer to input buffer */
unsigned int hitim, lotim;	/* Variables for delay loop */
long curblk;			/* Current block */
char sum0,sum1;
int record;

char vtsendcmd();	/* Forward references */
char vtgetc0();
char vtgetc1();

struct  vtdevice 
	{
        int     rcsr,rbuf;
        int     tcsr,tbuf;
	};

#define NVT	2
struct  vtdevice *VTcsr[NVT + 1] =
	{
        (struct vtdevice *)0177560,	/* We use VTcsr[0]: KL unit 0 */
        (struct vtdevice *)0,
        (struct vtdevice *)-1
	};

extern int tapemark;    /* flag to indicate tapemark encountered
                           (see sys.c as to how it's used) */


/*** Standalone-dependent section */

/* vtopen() is used to inform the server which record we'll be using */
int vtopen(io)
	register struct iob *io;
	{
	register i;

	record= io->i_boff;
	curblk=0;

	vtsendcmd(VTC_OPEN);		/* Send the command to the server */
	}

int vtclose(io)
	struct  iob     *io;
	{
	return(-1);		/* Same as nullsys */
	}


/* We send a command to the server, get a reply, and return the
 * amount read (even on EOF, which may be zero) or -1 on error.
 */
int vtstrategy(io, func)
	register struct iob *io;
	{
	register error, i;

	vtbuf= io->i_ma;

	/* Assume record, curblk are ok */
	/* Assume i->i_cc is in multiples of BLKSIZE */
	for (i=0; i<io->i_cc; i+=BLKSIZE, vtbuf+=BLKSIZE, curblk++)
		{
	        if (func==WRITE) error = vtsendcmd(VTC_WRITE);
	        else error = vtsendcmd(VTC_ZEROREAD);

		/* Some programs rely on the buffer being
	 	 * cleared to indicate EOF, e.g cat.
		 */
		if (error == VTE_EOF)
			{
			tapemark=1;
			vtbuf[0]=0;
			return(i);
			}

		if (error != 0)
			{
			printf("tape error %d", error);
			return(-1);
			}
    		}
	return(io->i_cc);
	}

/*** Protocol-specific stuff ***/

char vtsendcmd(cmd)
	int cmd;
	{
	register i;
	char error;
	char tmp;
	int reply;

  sendcmd:
	sum0=0;
	sum1=0;
	vtputw(VT_HDR1,VT_HDR2);
	vtputw(cmd,record);
	if(curblk<0x0000ff00L)
		{
		vtputw((short)curblk,(short)curblk>>8);
		}
	else
		{
		vtputw((short)curblk,255);
		vtputw((short)curblk>>8,(short)(curblk>>16));
		}

	if (cmd==VTC_WRITE)
		for(i=0; i<BLKSIZE/2; i+=2)
			vtputw(vtbuf[i],vtbuf[i+1]);

	vtputw(sum0,sum1);

  			/* Now get a valid reply from the server */
  getreply:
	sum0 = 0;
	sum1 = 0;

	tmp = vtgetc0();
	if (hitim==0) goto sendcmd;
	if (tmp!=VT_HDR1) goto getreply;
	tmp= vtgetc1();
	if (hitim==0) goto sendcmd;
	if (tmp!=VT_HDR2) goto getreply;
	reply = vtgetc0();
	if (hitim==0) goto sendcmd;
	vtgetc1();
	if (hitim==0) goto sendcmd;
	vtgetc0();
	if (hitim==0) goto sendcmd;
	tmp= vtgetc1();
	if (hitim==0) goto sendcmd;
	if(tmp==-1)
		{
		vtgetc0();
		if (hitim==0) goto sendcmd;
		vtgetc1();
		if (hitim==0) goto sendcmd;
		}

			/* Retrieve the block if no errs and a READ reply */
	if (reply==VTC_READ)
		{
		for (i=0; i<BLKSIZE; i++)
			{
			vtbuf[i]= vtgetc0();
			if (hitim==0) goto sendcmd;
			i++;
			vtbuf[i]= vtgetc1();
			if (hitim==0) goto sendcmd;
      			}
    		}
			/* Get the checksum */
	vtgetc0();
	if (hitim==0) goto sendcmd;
	vtgetc1();
	if (hitim==0) goto sendcmd;

			/* Try again on a bad checksum */
	if (sum0 | sum1)
		{
		putchar('e');
		goto sendcmd;
    		}
			/* Zero the buffer if a successful zero read */
	if (reply==VTC_ZEROREAD)
		for (i=0; i<BLKSIZE; i++)
			vtbuf[i]=0;

			/* Extract any error */
	error= reply >> 4;
	return(error);
	}

/*** Harware-specific stuff ***/

/* vtgetc() and vtputc(): A sort-of repeat of the getchar/putchar
 * code in prf.c, but without any console stuff
 */

/* Get a character, or timeout and return with hitim zero */
char vtgetc0()
	{   
        register c;
    
        VTcsr[0]->rcsr = 1;
	hitim=3;
	lotim=65535;
  
        while ((VTcsr[0]->rcsr&0200)==0)
		{
	   	lotim--;
	   	if (lotim==0) hitim--;
	   	if (hitim==0)
			{
			putchar('t');
			return(0);
			}
		}
        c = VTcsr[0]->rbuf; return(c);
	sum0 ^= c;
	}

char vtgetc1()
	{   
        register c;
    
        VTcsr[0]->rcsr = 1;
	hitim=3;
	lotim=65535;
  
        while ((VTcsr[0]->rcsr&0200)==0)
		{
	   	lotim--;
	   	if (lotim==0) hitim--;
	   	if (hitim==0)
			{
			putchar('t');
			return(0);
			}
		}
        c = VTcsr[0]->rbuf; return(c);
	sum1 ^= c;
	}


vtputw(lo,hi)
	int lo,hi;
	{
        register s;

        while((VTcsr[0]->tcsr&0200) == 0);
        s = VTcsr[0]->tcsr;
        VTcsr[0]->tcsr = 0;
	VTcsr[0]->tbuf = lo;
	VTcsr[0]->tcsr = s;

        while((VTcsr[0]->tcsr&0200) == 0);
        VTcsr[0]->tcsr = 0;
	VTcsr[0]->tbuf = hi;
	VTcsr[0]->tcsr = s;

	sum0 ^= lo;
	sum1 ^= hi;
	}
