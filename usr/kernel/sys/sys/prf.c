#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mount.h>
#include <sys/filsys.h>
#include <sys/seg.h>
#include <sys/buf.h>
#include <sys/conf.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)prf.c	3.2";
#endif

/*
 * In case console is off,
 * panicstr contains argument to last
 * call to panic.
 */

char	*panicstr ={ 0};

/*
 * Scaled down version of C Library printf.
 * Only %s %u %d (==%u) %o %x %D %O are recognized.
 * Used to print diagnostic information
 * directly on console tty.
 * Since it is not interrupt driven,
 * all system activities are pretty much
 * suspended.
 * Printf should not be used for chit-chat.
 */
/* VARARGS 1 */
printf(fmt, x1)
register char *fmt;
unsigned x1;
{
	register c;
	register unsigned int *adx;
	char *s;

	adx = &x1;
loop:
	while((c = *fmt++) != '%') {
		if(c == '\0')
			return;
		putchar(c);
	}
	c = *fmt++;
	if(c == 'd' || c == 'u' || c == 'o' || c == 'x')
		printn((long)*adx, c=='o'? 8: (c=='x'? 16:10));
	else if(c == 's') {
		s = (char *)*adx;
		while(c = *s++)
			putchar(c);
	} else if (c == 'D' || c == 'O') {
		printn(*(long *)adx, c=='D'?10:8);
		adx += (sizeof(long) / sizeof(int)) - 1;
	}
	adx++;
	goto loop;
}

/*
 * Print an unsigned integer in base b.
 */
printn(n, b)
long n;
{
	register long a;

	if (n<0) {	/* shouldn't happen */
		putchar('-');
		n = -n;
	}
	if(a = n/b)
		printn(a, b);
	putchar("0123456789ABCDEF"[(int)(n%b)]);
}

/*
 * Panic is called on unresolvable
 * fatal errors.
 * It syncs, prints "panic: mesg" and
 * then loops.
 */
panic(s)
char *s;
{
	panicstr = s;
	update();
	printf("panic: %s\n", s);
	for(;;)
		idle();
}

/*
 * prdev prints a warning message of the
 * form "mesg on dev x/y".
 * x and y are the major and minor parts of
 * the device argument.
 */
prdev(str, dev)
char *str;
dev_t dev;
{
	struct	mount	*mp;
	struct	filsys	*fp;
	
	for(mp = &mount[0]; mp < &mount[NMOUNT]; mp++)
		if(mp->m_bufp != NULL)
			if(dev == mp->m_dev)
			{
#ifdef	UCB_MOUNT
				fp = mp->m_caddr;
#else
				fp = mp->m_bufp->b_un.b_filsys;
#endif
				printf("%s on %s (%u/%u)\n",
					 str,fp->s_fname,  major(dev),
					 minor(dev));
				return;
			}
	printf("%s on dev %u/%u\n", str, major(dev), minor(dev));
}

/*
 * deverr prints a diagnostic from
 * a device driver.
 * It prints the device, block number,
 * and an octal word (usually some error
 * status register) passed as argument.
 */
deverror(bp, o1, o2)
register struct buf *bp;
{

	prdev("err", bp->b_dev);
	printf("bn=%D er=%o,%o\n", bp->b_blkno, o1, o2);
}
