/*
*	TS11 - 1600 bpi tape driver
*
*	*****  make sure that the address 'TSADDR' is set correctly *****
*/

#include <sys/param.h>
#include <sys/buf.h>
#include <sys/dir.h>
#include <sys/conf.h>
#include <sys/file.h>
#include <sys/user.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)ts.c	3.1";
#endif

int cmdpkt[5];		/* command packet. extra location is mod 4 alignment */

struct	device
{
	int	tsdb;		/* TS data buffer and bus address reg */
	int	tssr;		/* TS status register */
};

struct	mespkt
{
	int	mshdr;		/* message packet header word */
	int	mssiz;		/* size of message returned */
	int	msresid;	/* remaining count register */
	int	mstsx0;		/* extended status reg 0 */
	int	mstsx1;		/* extended status reg 1 */
	int	mstsx2;		/* extneded status reg 2 */
	int	mstsx3;		/* extended status reg 3 */
};

struct compkt
{
	int	tscom;		/* command packet command word */
	int	tsba;		/* memory address */
	int	tsbae;		/* extended address */
	int	tswc;		/* byte count, record count, etc. */
};

struct	chrdat
{
	int	msbptr;		/* pointer to message buffer */
	int	msbae;		/* hi address (bits 16 & 17) */
	int	msbsiz;		/* size of message buffer */
	int	mschar;		/* characteristics word */
};

struct	chrdat	chrbuf;		/* characteristics buffer */
struct	mespkt	mesbuf;		/* message buffer */
struct	buf	tstab;
struct	buf	ctsbuf;
struct	buf	rtsbuf;


char	ts_flags[8];
int	ts_openf[8];
daddr_t	ts_blkno[8];
daddr_t	ts_nxrec[8];

#define	TSADDR	((struct device *)0172520)
/*	The following 3 lines are necessary to align com buffer on mod 4 */
#define	t_temp	(&cmdpkt[1])	/* get address of cmdpkt +2 */
#define	t_temp1	(t_temp & 0177776)
#define	combuf	((struct compkt *)t_temp1)


	/* bit definitions for command word in ts command packet */

#define	ACK	0100000		/* acknowledge bit */
#define	CVC	040000		/* clear volume check */
#define	OPP	020000		/* opposite. reverse recovery */
#define	SWB	010000		/* swap bytes. for data xfer */
	/* bit definitions for Command mode field during read command */
#define	RNEXT	0		/* read next (forward) */
#define	RPREV	0400		/* read previous (reverse) */
#define	RRPRV	01000		/* reread previous (space rev, read two) */
#define	RRNXT	01400		/* reread next (space fwd, read rev) */
	/* bit definitions for Command mode field during write command */
#define	WNEXT	0		/* Write data next */
#define	WDRTY	01000		/* write data retry , space rev, erase, write data) */
	/* bit definitions for command mode field during position command */
#define	SPCFWD	0		/* space records forward */
#define	SPCREV	0400		/* space records reverse */
#define	SKTPF	01000		/* skip tape marks forward */
#define	SKTPR	01400		/* skip tape marks reverse */
#define	RWIND	02000		/* rewind */

	/* bit definitions for command mode field during format command */
#define	WEOF	0		/* write tape mark */
#define	ERAS	0400		/* erase */
#define	WEOFE	01000		/* write tape mark entry */
	/* bit definitions for command mode field during control command */
#define	MBREAL	0		/* message buffer release */
#define	REWUNL	0400		/* Rewind and unload */
#define	CLEAN	01000		/* clean */
	/* additional definitions */
#define	IEI	0200		/* interrupt enable bit */

	/* command code definitions */
#define	NOP	0
#define	RCOM	01		/* read command */
#define	WCHAR	04		/* write characteristics */
#define	WCOM	05		/* write */
#define	WSUSM	06		/* write subsystem memory */
#define	POSIT	010		/* position command */
#define	FORMT	011		/* Format command */
#define	CONTRL	012		/* Control command */
#define	INIT	013		/* initialize */
#define	GSTAT	017		/* get status immediate */


	/* definition of tssr bits */
#define	SC	0100000		/* special condition */
#define	UPE	040000		/* unibus parity error */
#define	SPE	020000		/* Serial bus parity error */
#define	RMR	010000		/* register modify refused */
#define	NXM	04000		/* non-existent memory */
#define	NBA	02000		/* Need Buffer address */
#define	SSR	0200		/* Sub-System ready */
#define	OFL	0100		/* off-line */

	/* fatal termination class codes */
#define	FTC	030		/* use this as a mask to get codes */
	/* code = 00	see error code byte in tsx3 */
	/* code = 01	I/O seq Crom or main Crom parity error */
	/* code = 10	u-processor Crom parity error,I/O silo parity */
	/*		serial bus parity, or other fatal */
	/* code = 11	A/C low. drive ac low */

	/* termination class codes */
#define	TCC	016		/* mask for termination class codes */
	/* code = 000	normal termination */
	/* code = 001	Attention condition 
	/* code = 010	Tape status alert 
	/* code = 011	Function reject 
	/* code = 100	Recoverable error - tape pos = 1 record down from
			start of function 
	/* code = 101	Recoverable error - tape has not moved
	/* code = 110	Unrecoverable error - tape position lost
	/* code = 111	Fatal controller error - see fatal class bits */

	/* definition of message buffer header word */
#define	MAKC	0100000		/* acknowledge from controller */
#define	MCCF	07400		/* mask for class code field */
	/* class codes are */
	/*	0 = ATTN, on or ofline
		1 = ATTN, microdiagnostic failure
		0 = FAIL, serial bus parity error
		1 = FAIL, WRT LOCK
		2 = FAIL, interlock or non-executable function
		3 = FAIL, microdiagnostic error
	*/

#define	MMSC	037	/* mask for message code field */
	/* message codes are
		020	= end
		021	= FAIL
		022	= ERROR
		023	= Attention

	/* definition of extended status reg 0 bits */
#define	TMK	0100000		/* Tape mark detected */
#define	RLS	040000		/* Record length short */
#define	LET	020000		/* Logical end of Tape */
#define	RLL	010000		/* Record length long */
#define	WLE	04000		/* Write lock error */
#define	NEF	02000		/* Non-executable function */
#define	ILC	01000		/* Illegal command */
#define	ILA	0400		/* Illegal address */
#define	MOT	0200		/* Capistan is moving */
#define	ONL	0100		/* On Line */
#define	IE	040		/* state of interrupt enable bit */
#define	VCK	020		/* Volume Check */
#define	PED	010		/* Phase encoded drive */
#define	WLK	04		/* Write locked */
#define	BOT	02		/* Tape at bot */
#define	EOT	01		/* Tape at eot */

	/* definitions of xstat1 */
#define	DLT	0100000		/* Data late error */
#define	COR	020000		/* Correctable data error */
#define	CRS	010000	/* Crease detected */
#define	TIG	04000		/* Trash in the gap */
#define	DBF	02000		/* Deskew Buffer Fail */
#define	SCK	01000		/* Speed check */
#define	IPR	0200		/* Invalid preamble */
#define	SYN	0100		/* Synch Failure */
#define	IPO	040		/* invalid postamble */
#define	IED	020		/* invalid end data */
#define	POS	010		/* postamble short */
#define	POL	04		/* postamble long */
#define	UNC	02		/* Uncorrectable data error */
#define	MTE	01		/* multi track error */

	/* Definitions of XSTAT2 bits */
#define	OPM	0100000		/* operation in progress (tape moving) */
#define	SIP	040000		/* Silo Parity error */
#define	BPE	020000		/* Serial bus Parity error at drive */
#define	CAF	010000		/* Capstan Acceleration fail */
#define	WCF	02000		/* Write card error */
#define	DTP	0477		/* mask for Dead track bits */

	/*	bit definitions for XSTAT3 */
#define	LMX	0200		/* Limit exceeded (tension arms) */
#define	OPI	0100		/* operation incomplete */
#define	REV	040		/* current operation in reverse */
#define	CRF	020		/* capstan response fail */
#define	DCK	010		/* density check */
#define	NOI	04		/* no tape mark or preamble */
#define	LXS	02		/* limit exceeded manual recovery */
#define	RIB	01		/* Reverse into BOT */


#define	SIO	01
#define SSEEK	02
#define	SINIT	04
#define	SRETRY	010
#define	SCOM	020
#define	SOK	040
#define	SMAST	0100

#define	S_WRITTEN	1

tsopen(dev, flag)
{
	register unit, ds;

	unit = minor(dev) & 0177;
	if (unit >= 8 || ts_openf[unit])
	{
		u.u_error = ENXIO;
		return;
	}
	mesbuf.mstsx0 = 0;
	if(ds = tsinit(dev, unit))
	{
		u.u_error = ENXIO;
		return;
	}
	ts_blkno[unit] = 0;
	ts_nxrec[unit] = 32760;
	ts_flags[unit] = 0;
	tstab.b_flags |= B_TAPE;
	ds = tscommand(dev, NOP);
	if(TSADDR->tssr & OFL || flag && (ds&WLK))
		u.u_error = ENXIO;
	if(u.u_error == 0)
		ts_openf[unit]++;
}
tsclose(dev, flag)
{
	register int unit;

	unit = minor(dev) & 0177;
	if(flag == FWRITE || ((flag&FWRITE) && (ts_flags[unit]&S_WRITTEN)))
	{
		tscommand(dev, (FORMT|WEOF));
		tscommand(dev, (FORMT|WEOF));
		tscommand(dev, (SPCREV|POSIT));
	}
	mesbuf.msresid = 0;
	if ((minor(dev)&200) ==0 )
	{
		tscommand(dev, (RWIND|POSIT));
	}
	ts_openf[unit] = 0;
}
tscommand(dev, com)
{
	register struct buf *bp;

	bp = &ctsbuf;
	spl5();
	while(bp->b_flags&B_BUSY)
	{
		bp->b_flags |= B_WANTED;
		sleep((caddr_t)bp, PRIBIO);
	}
	spl0();
	bp->b_dev = dev;
	bp->b_resid = com;
	bp->b_blkno = 0;
	bp->b_flags = B_BUSY|B_READ;
	tsstrategy(bp);
	iowait(bp);
	if(bp->b_flags&B_WANTED)
		wakeup((caddr_t)bp);
	bp->b_flags = 0;
	return(bp->b_resid);
}
tsstrategy(bp)
{
	register daddr_t *p;

#ifdef UNIBUS_MAP
	if(bp->b_flags & B_PHYS)  /* if RAW I/O call unibus map allocate */
		mapalloc(bp);
#endif
	if(bp != &ctsbuf)
	{
		p = &ts_nxrec[minor(bp->b_dev) & 0177];
#ifndef UCB_NKB
		if(bp->b_blkno > *p) 
#else
		if(dbtofsb(bp->b_blkno) > *p) 
#endif
		{
			bp->b_flags |= B_ERROR;
			bp->b_error = ENXIO;
			iodone(bp);
			return;
		}
#ifndef UCB_NKB
		if(bp->b_blkno == *p && bp->b_flags&B_READ)
#else
		if(dbtofsb(bp->b_blkno) == *p && bp->b_flags&B_READ) 
#endif
		{
			clrbuf(bp);
			bp->b_resid = bp->b_bcount;
			iodone(bp);
			return;
		}
		if((bp->b_flags&B_READ) == 0)
		{
#ifndef UCB_NKB
			*p = bp->b_blkno + 1;
#else
			*p = dbtofsb(bp->b_blkno) + 1;
#endif
			ts_flags[minor(bp->b_dev)&0177] |= S_WRITTEN;
		}
	}
	bp->av_forw = 0;
	spl5();
	if (tstab.b_actf == NULL)
		tstab.b_actf = bp;
	else
		tstab.b_actl->av_forw = bp;
	tstab.b_actl = bp;
	if (tstab.b_active == 0)
		tsstart();
	spl0();
}
tsstart()
{
	register struct buf *bp;
	register int com, unit;
	register daddr_t blkno;


    loop:
	if ((bp = tstab.b_actf) == 0)
		return;
	unit = minor(bp->b_dev) &0177;
	blkno = ts_blkno[unit];
	if(ts_openf[unit] < 0)
	{
		bp->b_flags |= B_ERROR;
		goto next;
	}
	if (bp == &ctsbuf)
	{
		if(bp->b_resid == NOP)
		{
			bp->b_resid = mesbuf.mstsx0;
			goto next;
		}
		if(bp->b_resid == INIT)
			tstab.b_active = SINIT;
		else
			tstab.b_active = SCOM;
		combuf->tswc = 1;
		combuf->tscom = (ACK|IEI|bp->b_resid);
		TSADDR->tsdb = &combuf->tscom;
		return;
	}
#ifndef	UCB_NKB
	if(blkno != bp->b_blkno)
	{
		tstab.b_active = SSEEK;
		if(blkno < bp->b_blkno)
		{
			combuf->tscom = (ACK|IEI|SPCFWD|POSIT);
			combuf->tsba = bp->b_blkno - blkno;
		}
		else
		{
			if(bp->b_blkno == 0)
				combuf->tscom = (ACK|IEI|RWIND|POSIT);
			else
			{
				combuf->tscom = (ACK|IEI|SPCREV|POSIT);
				combuf->tsba = blkno - bp->b_blkno;
#else
	if(blkno != dbtofsb(bp->b_blkno))
	{
		tstab.b_active = SSEEK;
		if(blkno < dbtofsb(bp->b_blkno))
		{
			combuf->tscom = (ACK|IEI|SPCFWD|POSIT);
			combuf->tsba = dbtofsb(bp->b_blkno) - blkno;
		}
		else
		{
			if(dbtofsb(bp->b_blkno) == 0)
				combuf->tscom = (ACK|IEI|RWIND|POSIT);
			else
			{
				combuf->tscom = (ACK|IEI|SPCREV|POSIT);
				combuf->tsba = blkno - dbtofsb(bp->b_blkno);
#endif
			}
		}
		TSADDR->tsdb = &combuf->tscom;
		return;
	}
	tstab.b_active = SIO;
	combuf->tsba = bp->b_un.b_addr;
	combuf->tsbae = bp->b_xmem;
	combuf->tswc = bp->b_bcount;
	combuf->tscom = (ACK|IEI);
	if(bp->b_flags &B_READ)
		combuf->tscom |= (RNEXT|RCOM);
	else
		combuf->tscom |= (WNEXT|WCOM);
	TSADDR->tsdb = &combuf->tscom;
	return;
next:
	tstab.b_actf = bp->av_forw;
	iodone(bp);
	goto loop;
}
tsintr()
{
	register struct buf *bp;
	register int unit,state;
	int err;

	if((bp = tstab.b_actf) == 0)
		return;
	unit = minor(bp->b_dev)&0177;
	state = tstab.b_active;
	tstab.b_active = 0;
	if(TSADDR->tssr & SC)
	{
		err = mesbuf.mstsx0;
		err &= ~(MOT|ONL|IE|PED|WLK);
		++tstab.b_errcnt;
		switch ((TSADDR->tssr & TCC) >> 1)
		{
			case 07:
				ts_openf[unit] = -1;
				deverror(bp, TSADDR->tssr, mesbuf.mstsx3);
				tsinit(bp->b_dev, unit);
				state = 0;
				break;
			case 06:
				deverror(bp, mesbuf.mstsx0, mesbuf.mstsx3);
				if(tsinit(bp->b_dev, unit) != 0)
				{
					ts_openf[unit] = -1;
					break;
				}
				state = SINIT;
				break;
			case 05:
				state = SINIT;
				break;
			case 04:
				if(tstab.b_errcnt <10)
				{
					combuf->tscom = (ACK|IEI);
					if(bp->b_flags & B_READ)
						combuf->tscom |= (RRPRV|RCOM);
					else
						combuf->tscom |= (WDRTY|WCOM);
					TSADDR->tsdb = &combuf->tscom;
					return;
				}
			case 03:
			case 02:
				if(mesbuf.mstsx0 & TMK)
				{
#ifndef	UCB_NKB
					ts_nxrec[unit] = bp->b_blkno;
#else
					ts_nxrec[unit] = dbtofsb(bp->b_blkno);
#endif
					state = SCOM;
				}
				if(bp == &rtsbuf)
				{
					err &= ~(RLS|RLL);
					state = SIO;
				}
				goto out;
			case 01:
				state = SINIT;
				break;
		}
		if(tstab.b_errcnt > 10 || state == 0)
		{
			ts_openf[unit] = -1;
			deverror(bp, TSADDR->tssr, 0);
		}
		bp->b_flags = B_ERROR;
/*
		if(state != 0 || state != SINIT)
			state = SIO;
*/
	}
   out:
	switch (state)
	{
		case SINIT:
			break;
		case SIO:
		case SOK:
			ts_blkno[unit]++;
		case SCOM:
			tstab.b_errcnt = 0;
			tstab.b_actf = bp->av_forw;
			if(combuf->tscom & POSIT)
				mesbuf.msresid = 0;
			bp->b_resid = mesbuf.msresid;
			iodone(bp);
			break;
		case SSEEK:
#ifndef	UCB_NKB
			ts_blkno[unit] = bp->b_blkno;
#else
			ts_blkno[unit] = dbtofsb(bp->b_blkno);
#endif
			break;
	}
	tsstart();
}
tsinit(dev, unit)
{
	int ds;

	combuf->tscom = (ACK|CVC|INIT);
	TSADDR->tsdb = &combuf->tscom;
	while ((TSADDR->tssr & SSR) == 0);
	chrbuf.msbptr = &mesbuf;
	chrbuf.msbae = 0;
	chrbuf.msbsiz = 016;
	chrbuf.mschar = 0;
	combuf->tscom = (ACK|CVC|WCHAR);
	combuf->tsba = &chrbuf;
	combuf->tsbae = 0;
	combuf->tswc = 010;
	tstab.b_active = 0;
	while((TSADDR->tssr & SSR) == 0);
	TSADDR->tsdb = &combuf->tscom;
	while ((TSADDR->tssr & SSR) == 0);
	if(((TSADDR->tssr & TCC) >>1) > 1)
		return(1);
	else
		return(0);
}
tsread(dev)
{
	tsphys(dev);
	physio(tsstrategy, &rtsbuf, dev, B_READ);
}
tswrite(dev)
{
	tsphys(dev);
	physio(tsstrategy, &rtsbuf, dev, B_WRITE);
}
tsphys(dev)
{
	register unit;
	daddr_t a;

	unit = minor(dev) & 0177;
	if(unit < 8)
	{
		a = u.u_offset >>9;
#ifndef	UCB_NKB
		ts_blkno[unit] = a;
		ts_nxrec[unit] = a+1;
#else
		ts_blkno[unit] = dbtofsb(a);
		ts_nxrec[unit] = dbtofsb(a+1);
#endif
	}
}
