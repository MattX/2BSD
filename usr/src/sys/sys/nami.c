/*
 *	SCCS id	@(#)nami.c	2.1 (Berkeley)	8/5/83
 */

#include "param.h"
#include <sys/systm.h>
#include <sys/inode.h>
#include <sys/filsys.h>
#include <sys/mount.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/buf.h>
#include <sys/quota.h>

#ifndef	saveseg5
#include <sys/seg.h>
#endif

/*
 * Convert a pathname into a pointer to
 * an inode. Note that the inode is locked.
 *
 * func = function called to get next char of name
 *	&uchar if name is in user space
 *	&schar if name is in system space
 *	flag =	LOOKUP if name is sought
 *		CREATE if name is to be created
 *		DELETE if name is to be deleted
 *	follow = 1 if to follow links at end of name
 */
struct inode *
namei(func,flag,follow)
int	(*func)(),
	flag,
	follow;
{
	register int	c;
	struct inode	*dp;
	struct direct	*dirp;
	struct buf	*bp,
			*temp;
	dev_t	d;
	off_t	eo;
	int	nlink;
	short	i;
	char	*cp;

	u.u_sbuf = nlink = 0;

	/*
	 * If name starts with '/' start from root;
	 * otherwise start from current dir.
	 */
	dp = u.u_cdir;
	if((c = (*func)()) == '/')
		if ((dp = u.u_rdir) == NULL)
			dp = rootdir;
	iget(dp->i_dev, dp->i_number);
	while(c == '/')
		c = (*func)();
	if(c == '\0' && flag != LOOKUP)
		u.u_error = ENOENT;
cloop:
	/*
	 * Here dp contains pointer to last component matched.
	 */
	if(u.u_error)
		goto out;
	if(c == '\0')
		return(dp);
	/*
	 * If there is another component, gather up name into
	 * users' dir buffer.
	 */
	cp = &u.u_dbuf[0];
	while (c != '/' && c != '\0' && u.u_error == 0 ) {
		if(cp < &u.u_dbuf[MAXNAMLEN])
			*cp++ = c;
		c = (*func)();
	}
	while(cp < &u.u_dbuf[MAXNAMLEN])
		*cp++ = '\0';
	while(c == '/')
		c = (*func)();
	/*
	 * dp must be a directory and must have X permission.
	 */
	access(dp, IEXEC);
seloop:
	if((dp->i_mode&IFMT) != IFDIR)
		u.u_error = ENOTDIR;
	if(u.u_error)
		goto out;
	/*
	 * set up to search a directory
	 */
	eo = u.u_offset = 0;
	u.u_segflg = 1;
	bp = NULL;
	if (dp == u.u_rdir && u.u_dbuf[0] == '.' &&
		u.u_dbuf[1] == '.' && u.u_dbuf[2] == 0)
			goto cloop;
eloop:
	/*
	 * If at the end of the directory, the search failed. Report what
	 * is appropriate as per flag.
	 */
	if(u.u_offset >= dp->i_size) {
		if(bp != NULL) {
			mapout(bp);
			brelse(bp);
		}
		if(flag == CREATE && c == '\0') {
			if(access(dp, IWRITE))
				goto out;
			u.u_pdir = dp;
			if(eo)
				u.u_offset = eo-sizeof(struct direct);
			else
				dp->i_flag |= IUPD|ICHG;
			goto out1;
		}
		u.u_error = ENOENT;
		goto out;
	}
	/*
	 * If offset is on a block boundary, read the next directory block.
	 * Release previous if it exists.
	 */
	if((u.u_offset & BMASK) == 0) {
		if(bp != NULL) {
			mapout(bp);
			brelse(bp);
		}
		bp = bread(dp->i_dev, bmap(dp, (daddr_t)(u.u_offset>>BSHIFT), B_READ));
		if (bp->b_flags & B_ERROR) {
			brelse(bp);
			goto out;
		}
		dirp = (struct direct *)mapin(bp);
	}
	/*
	 * Search for the directory entry.
	 * Note first empty directory slot in eo for possible creat.
	 */
	u.u_offset += sizeof(struct direct);
	if(!dirp->d_ino) {
		dirp++;
		if(eo == 0)
			eo = u.u_offset;
		goto eloop;
	}
#ifdef UCB_QUOTAS
	if((dirp->d_name[0] == '.') && (dirp->d_name[1] == 'q') && (dirp->d_name[2] == '\0')) {
		cp = dp->i_quot;
	/* If no quota is associated yet or a new quot is around, then... */
		if (cp == NULL || cp->i_number != dirp->d_ino) {
			u.u_dent.d_ino = dirp->d_ino;
			mapout(bp);
			cp = iget(dp->i_dev, u.u_dent.d_ino);
			if (cp != NULL) {
				prele(cp);
	/* If not really a quota node then just put away */
				if (!isquot(cp)) {
					iput(cp);
					cp = NULL;
				}
			}
	/*
	 * The value of dirp is still valid because the buffer can not
	 * have been released between the mapout() above and here, and
	 * there is a static relationship between buffer headers and the
	 * buffers proper.
	 */
			mapin(bp);
			if (cp != NULL) {
	/*
	 * Set up hierarchical inode chains.  NOTE: this is done wrong since
	 * this may overwrite an inode which has not been put away yet.
	 */
				cp->i_quot = dp->i_quot;
				dp->i_quot = cp;
			}
		}
	/*
	 * Mark the directory as being the original owner of the quota.
	 * This is necessary so that quotas do not get copied up the tree.
	 */
		if (cp != NULL)
			dp->i_flag |= IQUOT;
	}
#endif UCB_QUOTAS
	/*
	 * This loop does a string compare with the directory entry and
	 * the current component.  If they don't match, go to eloop.
	 * The loop is unrolled to make it as fast as possible, notice
	 * also, we're comparing 7 shorts rather than 14 chars.
	 */
	{
		register short	*p1 = u.u_dbuf,
				*p2 = dirp->d_name;
		if (*p1++ != *p2++ || *p1++ != *p2++ || *p1++ != *p2++ ||
		*p1++ != *p2++ || *p1++ != *p2++ || *p1++ != *p2++ ||
		*p1++ != *p2++) {
			++dirp;
			goto eloop;
		}
	}
	u.u_dent = *dirp;
	/*
	 * Here a component matched in a directory.  If there is more
	 * pathname, go back to cloop, otherwise return.
	 */
	if(bp != NULL) {
		mapout(bp);
		brelse(bp);
	}
	if(flag == DELETE && c == '\0') {
		if(access(dp, IWRITE))
			goto out;
		return(dp);
	}
	d = dp->i_dev;
	if ((u.u_dent.d_ino == ROOTINO) && (dp->i_number == ROOTINO)
	    && (u.u_dent.d_name[1] == '.'))
		for(i=1; i<nmount; i++)
			if ((mount[i].m_inodp != NULL) && (mount[i].m_dev == d)) {
				iput(dp);
				dp = mount[i].m_inodp;
				dp->i_count++;
				plock(dp);
				/*
				 * Note: permission for ROOTINO already checked.
				 */
				goto seloop;
			}
	prele(dp);
	temp = cp = iget(d, u.u_dent.d_ino);
	if (cp == NULL) {
		if (dp->i_flag & ILOCK)
			dp->i_count--;
		else
			iput(dp);
		goto out1;
	}
	if ((((struct inode *)temp)->i_mode&IFMT)==IFLNK && (follow || c)) {
		struct inode *pdp;

		pdp = (struct inode *)temp;
		if (pdp->i_size >= BSIZE-2 || ++nlink>8 || u.u_sbuf || !pdp->i_size) {
			u.u_error = ELOOP;
			iput(pdp);
			goto out;
		}
		u.u_sbuf = bread(pdp->i_dev, bmap(pdp, (daddr_t)0, B_READ));
		if (u.u_sbuf->b_flags & B_ERROR) {
			brelse(u.u_sbuf);
			iput(pdp);
			u.u_sbuf = 0;
			goto out;
		}
		/* Save our readahead chars at end of buffer, get first */
		/* symbolic link character */
		{
			segm save5;
			char *cp;

			if (c)		/* space for readahead chars */
				u.u_slength = pdp->i_size+2;
			else	u.u_slength = pdp->i_size+1;
			u.u_soffset = 0;
			saveseg5(save5);
			mapin(u.u_sbuf);
			cp = (char *)SEG5;
			if (c)
				cp[u.u_slength-2] = '/';
			cp[u.u_slength-1] = c;
			c = cp[u.u_soffset++];
			mapout(u.u_sbuf);
			restorseg5(save5);
		}

		/* Grab the top-level inode for the new path */
		iput(pdp);
		if (c == '/') {
			iput(dp);
			if ((dp = u.u_rdir) == NULL)
				dp = rootdir;
			while (c == '/')
				c = (*func)();
			iget(dp->i_dev, dp->i_number);
		}
		else	plock(dp);
		goto cloop;
	}
#ifndef UCB_QUOTAS
	else {
		iput(dp);
		dp = (struct inode *)temp;
	}
#endif
#ifdef	UCB_QUOTAS
	/*
	 * Make sure not to copy the quota node up the tree past
	 * the original height.
	 */
	if ((dp->i_flag & IQUOT) && u.u_dent.d_name[0] == '.'
	   && u.u_dent.d_name[1] == '.' && u.u_dent.d_name[2] == '\0')
		cp = dp->i_quot;
	/*
	 * Copy quota to new inode
	 */
	qcopy(dp, cp);
	if (dp->i_flag & ILOCK)
		dp->i_count--;
	else
		iput(dp);
	dp = temp;
#endif
	goto cloop;

out:
	iput(dp);
out1:
	if (u.u_sbuf) {
		brelse(u.u_sbuf);
		u.u_sbuf = u.u_slength = u.u_soffset = 0;
	}
	return(NULL);
}

#ifdef UCB_QUOTAS
/*
 * Copy quota from dp to ip if certain conditions hold.
 */
qcopy(dp, ip)
register struct inode *dp, *ip;
{
	register struct inode *qp;

	qp = dp->i_quot;
	if (qp == NULL || qp == ip)
		return;
	if (ip->i_quot != NULL)
		return;
	ip->i_quot = qp;
	if (++(qp->i_count) == 0)
		panic ("qcopy");
}
#endif UCB_QUOTAS

/*
 * Return the next character from the
 * kernel string pointed at by dirp.
 */
schar()
{
	register c;

	if (u.u_sbuf) {
		c = symchar();
		if (c >= 0)
			return(c);
	}
	return(*u.u_dirp++ & 0377);
}

/*
 * Return the next character from the
 * user string pointed at by dirp.
 */
uchar()
{
	register c;

	if (u.u_sbuf) {
		c = symchar();
		if (c >= 0)
			return(c);
	}
	c = fubyte(u.u_dirp++);
	if(c == -1)
		u.u_error = EFAULT;
	else if (c&0200)
		u.u_error = EINVAL;
	return(c);
}

/*
 *	Get a character from the symbolic name buffer
 */
symchar()
{
	segm save5;
	register char c;
	register char *cp;

	if (!u.u_sbuf)		/* Protect ourselves */
		return(-1);
	if (u.u_soffset > u.u_slength) {
		brelse(u.u_sbuf);
		u.u_soffset = u.u_slength = u.u_sbuf = 0;
		return(-1);
	}

	/* Get next character from symbolic link buffer */
	saveseg5(save5);
	mapin(u.u_sbuf);
	cp = (char *)SEG5;
	c = cp[u.u_soffset++];
	mapout(u.u_sbuf);
	restorseg5(save5);
	if (u.u_soffset >= u.u_slength) {
		brelse(u.u_sbuf);
		u.u_soffset = u.u_slength = u.u_sbuf = 0;
	}
	return(c);
};	/* end of symchar */
