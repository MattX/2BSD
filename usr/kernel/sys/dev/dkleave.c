#include <sys/param.h>
#include <sys/buf.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)dkleave.c	3.1";
#endif


daddr_t
dkblock(bp)
register struct buf *bp;
{
	register int dminor;

	if (((dminor=minor(bp->b_dev))&0100) == 0)
		return(bp->b_blkno);
	dminor >>= 3;
	dminor &= 07;
	dminor++;
	return(bp->b_blkno/dminor);
}

dkunit(bp)
register struct buf *bp;
{
	register int dminor;

	dminor = minor(bp->b_dev) >> 3;
	if ((dminor&010) == 0)
		return(dminor);
	dminor &= 07;
	dminor++;
	return(bp->b_blkno%dminor);
}
