/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)ra.c	2.3 (2.11BSD GTE) 1/1/93
 */

/*
 * MSCP disk device driver (rx23, rx33, rx50, rd??, ra??, rz??)
 */
#include "../h/param.h"
#include "../h/inode.h"
#include "../machine/mscp.h"
#include "../pdpuba/rareg.h"
#include "saio.h"

#define	NRA	2
#define	RA_SMASK	(RA_STEP4|RA_STEP3|RA_STEP2|RA_STEP1)

	struct	radevice *RAcsr[NRA + 1] =
		{
		(struct radevice *)0172150,
		(struct	radevice *)0,
		(struct radevice *)-1
		};

/*
 * RA Communications Area
*/
struct  rdca {
	short	ca_xxx1;	/* unused */
	char	ca_xxx2;	/* unused */
	char	ca_bdp;		/* BDP to purge */
	short	ca_cmdint;	/* command queue transition interrupt flag */
	short	ca_rspint;	/* response queue transition interrupt flag */
	short	ca_rspl;	/* response descriptors */
	short	ca_rsph;
	short	ca_cmdl;	/* command descriptors */
	short	ca_cmdh;
};

#define	ca_ringbase	ca_rspl

#define	RA_OWN	0x8000	/* RQ owns this descriptor */
#define	RA_INT	0x4000	/* allow interrupt on ring transition */

static	struct	ra {
	struct	rdca	ra_ca;
	struct	mscp	ra_rsp;
	struct	mscp	ra_cmd;
} rd[NRA];

static	u_char 	rainit[NRA];
static	long	raonline[NRA][8];

raopen(io)
	register struct iob *io;
{
	register struct radevice *raaddr;
	register struct ra *racom;
	int i, ctlr, unit;

	ctlr = CTLRn(io->i_unit);
	unit = UNITn(io->i_unit);
	if (genopen(NRA, io) < 0)
		return(-1);
	raaddr = RAcsr[ctlr];
	racom = &rd[ctlr];

	if (rainit[ctlr] == 0) {
again:		raaddr->raip = 0;
		if 	(ra_step(raaddr, RA_STEP1, 1))
			goto again;
		raaddr->rasa = RA_ERR | (0154/4);
		if	(ra_step(raaddr, RA_STEP2, 2))
			goto again;
		raaddr->rasa = (short)&racom->ra_ca.ca_ringbase;
		if	(ra_step(raaddr, RA_STEP3, 3))
			goto again;
		raaddr->rasa = segflag;
		if	(ra_step(raaddr, RA_STEP4, 4))
			goto again;
		raaddr->rasa = RA_GO;
		racom->ra_ca.ca_rspl = (short)&racom->ra_rsp.m_cmdref;
		racom->ra_ca.ca_rsph = segflag;
		racom->ra_ca.ca_cmdl = (short)&racom->ra_cmd.m_cmdref;
		racom->ra_ca.ca_cmdh = segflag;
		racom->ra_cmd.m_cntflgs = 0;
		if (racmd(M_O_STCON, io->i_unit) < 0) {
			printf("RA%d STCON err\n", ctlr);
			return(-1);
		}
		rainit[ctlr] = 1;
	}

	if (raonline[ctlr][unit] == 0)
		if (ramount(io) == -1)
			return(-1);
	return(0);
}

raclose(io)
	register struct iob *io;
{
	raonline[CTLRn(io->i_unit)][UNITn(io->i_unit)] = 0;
	return(0);
}

ramount(io)
	register struct iob *io;
{
	register int ctlr = CTLRn(io->i_unit);
	register int unit = UNITn(io->i_unit);

	if (racmd(M_O_ONLIN, io->i_unit) < 0) {
		printf("RA%d: online err\n", io->i_unit);
		return(-1);
	}
	raonline[ctlr][unit] = rd[ctlr].ra_rsp.m_uslow +  
		((long)(rd[ctlr].ra_rsp.m_ushigh) << 16);
	return(0);
}

racmd(op, unit)
	int op, unit;
{
	register struct mscp *mp;
	register int ctlr = CTLRn(unit);
	register struct ra *racom = &rd[ctlr];
	struct	radevice *csr = RAcsr[ctlr];
	int i;

	racom->ra_cmd.m_opcode = op;
	racom->ra_cmd.m_unit = UNITn(unit);
	racom->ra_rsp.m_header.ra_msglen = sizeof(struct mscp);
	racom->ra_cmd.m_header.ra_msglen = sizeof(struct mscp);
	racom->ra_ca.ca_rsph = RA_OWN | segflag;
	racom->ra_ca.ca_cmdh = RA_OWN | segflag;
	i = csr->raip;
	mp = &racom->ra_rsp;
	while (1) {
		while	(racom->ra_ca.ca_cmdh & RA_OWN) {
			delay(200);		/* SA access delay */
			if	(csr->rasa & (RA_ERR|RA_SMASK))
				goto fail;
		}
		while	(racom->ra_ca.ca_rsph & RA_OWN) {
			delay(200);		/* SA access delay */
			if	(csr->rasa & (RA_ERR|RA_SMASK))
				goto fail;
		}
		racom->ra_ca.ca_cmdint = 0;
		racom->ra_ca.ca_rspint = 0;
		if (mp->m_opcode == (op | M_O_END))
			break;
		printf("RA%d: rsp %x op %x ignored\n",
			unit,mp->m_header.ra_credits & 0xf0, mp->m_opcode);
		racom->ra_ca.ca_rsph |= RA_OWN;
	}
	if ((mp->m_status & M_S_MASK) != M_S_SUCC) {
		printf("RA%d: err op=%x sts=%x\n",unit,
			mp->m_opcode, mp->m_status);
		return(-1);
	}
	return(0);
fail:
	printf("RA%d: rasa=%o\n", ctlr, csr->rasa);
}

rastrategy(io, func)
	register struct iob *io;
{
	register struct mscp *mp;
	struct ra *racom;
	register int ctlr = CTLRn(io->i_unit);

	if (io->i_bn >= raonline[ctlr][UNITn(io->i_unit)])
		return(0);

	racom = &rd[ctlr];
	mp = &racom->ra_cmd;
	mp->m_lbn_l = loint(io->i_bn);
	mp->m_lbn_h = hiint(io->i_bn);
	mp->m_bytecnt = io->i_cc;
	mp->m_buf_l = (ushort)io->i_ma;
	mp->m_buf_h = segflag;
	if (racmd(func == READ ? M_O_READ : M_O_WRITE, io->i_unit) < 0)
		return(-1);
	return(io->i_cc);
}

ra_step(csr, mask, step)
	register struct radevice *csr;
	int mask, step;
	{
	register int	cnt;

	for	(cnt = 0; (csr->rasa & mask) == 0; )
		{
		delay(2000);
		cnt++;
		if	(cnt < 10000)
			continue;
		printf("RA(%o) failed step %d. retrying\n",csr,step);
		return(1);
		}
	return(0);
	}

delay(l)
	int	l;
	{

	while	(l > 0)
		l--;
	}
