/*-
 * Public domain, May 1995
 *
 *	@(#)label.c	1.0 (2.11BSD GTE) 1995/06/08
 */

#include "../h/param.h"
#include "saio.h"

/*
 * Called from the general device label routine in conf.c.   A label with
 * a fake geometry suitable for reading the label sector is created and
 * the driver's strategy routine is called.  If the label is corrupted
 * or absent an error is possibly printed.
 *
 * NOTES:  We reuse the iob structure's buffer (i_buf).
 *	   All disks must have at least LABELSECTOR+1 sectors in a track.
 *	   (this is not expected to be a problem since LABELSECTOR is 1).
*/

readlabel(io, strat, name)
	struct	iob	*io;
	int	(*strat)();
	char	*name;
	{
	struct disklabel *lp = &io->i_label;

	io->i_bn = LABELSECTOR;
	io->i_ma = io->i_buf;
	io->i_cc = 512;			/* XXX */
	lp->d_nsectors = LABELSECTOR + 1;	/* # sectors per track */
	lp->d_ntracks = 1;			/* # tracks per cylinder */
	lp->d_secpercyl = LABELSECTOR + 1;	/* # sectors per cylinder */
	if	((*strat)(io, READ) != 512)
		{
		printf("%s%d,%d: error reading labelsector\n", name,
			io->i_ctlr, io->i_unit);
		return(-1);
		}
	bcopy(io->i_buf, lp, sizeof (struct disklabel));
	if	(Nolabelerr)
		return(0);
	if	(lp->d_magic != DISKMAGIC || lp->d_magic2 != DISKMAGIC ||
		 dkcksum(lp) != 0)
		{
		printf("%s%d,%d disklabel missing or corrupt\n", name,
			io->i_ctlr, io->i_unit);
		return(-1);
		}
	return(0);
	}

writelabel(io, strat, name)
	register struct	iob	*io;
	int	(*strat)();
	char	*name;
	{
	register struct disklabel *lp = &io->i_label;

	if	((io->i_flgs & F_WRITE) == 0)
		return(-1);
	io->i_bn = LABELSECTOR;
	io->i_ma = (char *)&io->i_label;
	io->i_cc = 512;			/* XXX */
/*
 * The geometry had better be set up and correct at this point - we can not
 * fake the geometry because that would overwrite the contents of the label
 * itself.
*/
	lp->d_secsize = 512;			/* XXX */
	lp->d_magic = DISKMAGIC;
	lp->d_magic2 = DISKMAGIC;
	lp->d_checksum = 0;
	lp->d_checksum = dkcksum(lp);

	if	((*strat)(io, WRITE) != 512)
		{
		printf("%s: error writing labelsector\n", name);
		return(-1);
		}
	return(0);
	}
/*
 * Compute checksum for disk label.
 */
dkcksum(lp)
	register struct disklabel *lp;
	{
	register u_short *start, *end;
	register u_short sum = 0;

	start = (u_short *)lp;
	end = (u_short *)&lp->d_partitions[lp->d_npartitions];
	while (start < end)
		sum ^= *start++;
	return (sum);
	}

/*
 * Check for partitions which overlap each other.  While it is legal
 * to have partitions which overlap (as long as they are not used at the
 * same time) it is often a mistake or oversite. 
*/

overlapchk(lp)
	register struct disklabel *lp;
	{
	register struct partition *pp;
	int	i, part, openmask;
	daddr_t	start, end;

#define	RAWPART	2
/*
 * 'c' is normally (but not always) the partition which spans the whole
 * drive, thus it is not normally an error for it to overlap all other 
 * partitions.
*/
	openmask = ~0 & ~(1 << RAWPART);
	for	(part = 0; part < lp->d_npartitions; part++)
		{
		pp = &lp->d_partitions[part];
		if	(part == RAWPART || pp->p_size == 0)
			continue;
		start = pp->p_offset;
		end = start + pp->p_size;

		for	(pp = lp->d_partitions, i = 0; 
			 i < lp->d_npartitions;
			 pp++, i++)
			{
/*
 * We make sure to not report zero length partitions or that a 
 * partition overlaps itself.  We've already checked lower partitions
 * so avoid giving something like "a overlaps b" and then "b overlaps a".
*/
			if	(i <= part || pp->p_size == 0)
				continue;
			if	(pp->p_offset + pp->p_size <= start ||
				 pp->p_offset >= end)
				continue;
			if	((openmask & (1 << i)) == 0)
				continue;
			printf("warning: partition '%c' overlaps '%c'\n",
				'a' + part, 'a' + i);
			}
		}
	}
