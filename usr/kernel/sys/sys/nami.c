
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/inode.h>
#include <sys/mount.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/buf.h>
#include <sys/quota.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)nami.c	3.2";
#endif

#ifdef	UCB_QUOTAS
#ifdef	QUOTADEBUG
int	qdebug = 0;
#endif
#endif

/*
 * Convert a pathname into a pointer to
 * an inode. Note that the inode is locked.
 *
 * func = function called to get next char of name
 *	&uchar if name is in user space
 *	&schar if name is in system space
 * flag = 0 if name is sought
 *	1 if name is to be created
 *	2 if name is to be deleted
 */
struct inode *
namei(func, flag)
int (*func)();
{
	register struct direct *dirp;
	struct inode *dp;
	register c;
	register char *cp;
	struct buf *bp;
#ifdef UCB_QUOTAS
	struct buf *temp;
#endif
	int i;
	dev_t d;
	off_t eo;

	/*
	 * If name starts with '/' start from
	 * root; otherwise start from current dir.
	 */

	dp = u.u_cdir;
	if((c=(*func)()) == '/')
		if ((dp = u.u_rdir) == NULL)
			dp = rootdir;
	iget(dp->i_dev, dp->i_number);
	while(c == '/')
		c = (*func)();
	if(c == '\0' && flag != 0)
		u.u_error = ENOENT;

cloop:
	/*
	 * Here dp contains pointer
	 * to last component matched.
	 */

	if(u.u_error)
		goto out;
	if(c == '\0')
		return(dp);

	/*
	 * If there is another component,
	 * Gather up name into
	 * users' dir buffer.
	 */

	cp = &u.u_dbuf[0];
	while (c != '/' && c != '\0' && u.u_error == 0 ) {
		if (mpxip!=NULL && c=='!')
			break;
		if(cp < &u.u_dbuf[DIRSIZ])
			*cp++ = c;
		c = (*func)();
	}
	while(cp < &u.u_dbuf[DIRSIZ])
		*cp++ = '\0';
	while(c == '/')
		c = (*func)();
	if (c == '!' && mpxip != NULL) {
		iput(dp);
		plock(mpxip);
		mpxip->i_count++;
		return(mpxip);
	}

seloop:
	/*
	 * dp must be a directory and
	 * must have X permission.
	 */

	if((dp->i_mode&IFMT) != IFDIR)
		u.u_error = ENOTDIR;
	access(dp, IEXEC);
	if(u.u_error)
		goto out;

	/*
	 * set up to search a directory
	 */
	u.u_offset = 0;
	u.u_segflg = 1;
	eo = 0;
	bp = NULL;

eloop:

	/*
	 * If at the end of the directory,
	 * the search failed. Report what
	 * is appropriate as per flag.
	 */

	if(u.u_offset >= dp->i_size) {
		if(bp != NULL) {
#ifdef	UCB_BUFOUT
			mapout(bp);
#endif
			brelse(bp);
		}
		if(flag==1 && c=='\0') {
			if(access(dp, IWRITE))
				goto out;
			u.u_pdir = dp;
			if(eo)
				u.u_offset = eo-sizeof(struct direct);
			else
				dp->i_flag |= IUPD|ICHG;
			return(NULL);
		}
		u.u_error = ENOENT;
		goto out;
	}

	/*
	 * If offset is on a block boundary,
	 * read the next directory block.
	 * Release previous if it exists.
	 */

	if((u.u_offset&BMASK) == 0) {
		if(bp != NULL) {
#ifdef	UCB_BUFOUT
			mapout(bp);
#endif
			brelse(bp);
		}
		bp = bread(dp->i_dev,
			bmap(dp, (daddr_t)(u.u_offset>>BSHIFT), B_READ));
		if (bp->b_flags & B_ERROR) {
			brelse(bp);
			goto out;
		}
#ifdef	UCB_BUFOUT
		dirp = (struct direct *)mapin(bp);
#else
		dirp = (struct direct *) bp->b_un.b_addr;
#endif
	}

	/*
	 * Note first empty directory slot
	 * in eo for possible creat.
	 * String compare the directory entry
	 * and the current component.
	 * If they do not match, go back to eloop.
	 */

	u.u_offset += sizeof(struct direct);
	if(dirp->d_ino == 0) {
		dirp++;
		if(eo == 0)
			eo = u.u_offset;
		goto eloop;
	}
#ifdef UCB_QUOTAS
	/*
	 * See if this could be a quota node (KCA)
	 */
	if((dirp->d_name[0] == '.') && 
	   (dirp->d_name[1] == 'q') && 
	   (!dirp->d_name[2]))
	{
		cp = dp->i_quot;
		/*
		 * If no quota is associated yet or a new quot is
		 * around, then . . .
		 */
		if (cp == NULL || cp->i_number != dirp->d_ino) {
			u.u_dent.d_ino = dirp->d_ino;
#ifdef	UCB_BUFOUT
			mapout(bp);
#endif
			cp = iget(dp->i_dev, u.u_dent.d_ino);
#ifdef	QUOTADEBUG
qprint(01)("NAMEI: found new quota, u.u_dent.d_ino = %d, cp->i_number = %d\n",
	u.u_dent.d_ino, cp->i_number);
#endif
			if (cp != NULL) {
				prele(cp);
				/*
				 * If not really a quota node then just put away
				 */
				if (!isquot(cp)) {
#ifdef	QUOTADEBUG
qprint(01)("NAMEI: isquot returns FALSE\n");
#endif
					iput(cp);
					cp = NULL;
				}
			}
#ifdef	UCB_BUFOUT
			/*
			 * The value of dirp is still valid because
			 * the buffer can not have been released
			 * between the mapout() above and here,
			 * and there is a static relationship between
			 * buffer headers and the buffers proper.
			 */
			mapin(bp);
#endif
			if (cp != NULL) {
				/*
				 * set up hierarchical inode chains
				 * NOTE: this is done wrong since this may
				 *	 ovewrite an inode which has not
				 *	 been put away yet
				 */
				cp->i_quot = dp->i_quot;
				dp->i_quot = cp;
			}
		}
		if (cp != NULL) {
			/*
			 * Mark the directory as being the orignial
			 * owner of the quota.  This is necessary so
			 * that quotas do not get copied up the tree.
			 */
			dp->i_flag |= IQUOT;
#ifdef	QUOTADEBUG
qprint(01)("NAMEI: marking directory as quota dir\n");
#endif

		}
	}
#endif
	for(i=0; i<DIRSIZ; i++)
		if(u.u_dbuf[i] != dirp->d_name[i])
		{
			dirp++;
			goto eloop;
		}
	bcopy(((caddr_t) dirp), (caddr_t)&u.u_dent,
		sizeof(struct direct));
	/*
	 * Here a component matched in a directory.
	 * If there is more pathname, go back to
	 * cloop, otherwise return.
	 */

	if(bp != NULL) {
#ifdef UCB_BUFOUT
		mapout(bp);
#endif
		brelse(bp);
	}
	if(flag==2 && c=='\0') {
		if(access(dp, IWRITE))
			goto out;
		return(dp);
	}
	d = dp->i_dev;
	if(u.u_dent.d_ino == ROOTINO)
	if(dp->i_number == ROOTINO)
	if(u.u_dent.d_name[1] == '.')
		for(i=1; i<NMOUNT; i++)
			if(mount[i].m_bufp != NULL)
			if(mount[i].m_dev == d) {
				iput(dp);
				dp = mount[i].m_inodp;
				dp->i_count++;
				plock(dp);
				goto seloop;
			}
#ifdef UCB_QUOTAS
	prele(dp);
	temp = cp = iget(d, u.u_dent.d_ino);
	if (cp == NULL) {
		if (dp->i_flag & ILOCK)
			dp->i_count--;
		else
			iput(dp);
#ifdef	QUOTADEBUG
qprint(04)("NAMEI: iget(d, %d) returns NULL\n", u.u_dent.d_ino);
#endif
		return(NULL);
	}
	/*
	 * Make sure not to copy the quota node up the tree past
	 * the original height.
	 */
	if ((dp->i_flag & IQUOT) && isname(".."))
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
#else
	iput(dp);
	dp = iget(d, u.u_dent.d_ino);
	if(dp == NULL)
		return(NULL);
#endif
	goto cloop;

out:
	iput(dp);
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
#ifdef	QUOTADEBUG
qprint(01)("QCOPY: qp->i_count = %d, dp->i_number = %d, ip->i_number = %d\n",
	qp->i_count, dp->i_number, ip->i_number);
#endif
}
#endif
/*
 * Return the next character from the
 * kernel string pointed at by dirp.
 */
schar()
{

	return(*u.u_dirp++ & 0377);
}

/*
 * Return the next character from the
 * user string pointed at by dirp.
 */
uchar()
{
	register c;

	c = fubyte(u.u_dirp++);
	if(c == -1)
		u.u_error = EFAULT;
	return(c);
}
