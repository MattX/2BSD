char	*sccsid = "@(#)mkfs.c	2.7";

/*
 * Make a file system prototype.
 * usage: mkfs filsys size [ m n ]
 */
#include <sys/param.h>
/*
 * Need to do the following to get the larger incore inode structure
 * (the kernel might be built with the inode times external/mapped-out).
 * See /sys/h/localtimes.h and /sys/conf.
*/
#undef	EXTERNALITIMES
#ifndef STANDALONE
#include <stdio.h>
#endif

#include <sys/file.h>
#include <sys/fs.h>
#include <sys/inode.h>
#include <sys/dir.h>
#include <sys/stat.h>

#define	UMASK	0755
#define	NIPB	(DEV_BSIZE/sizeof(struct dinode))
#define	MAXFN	500

time_t	utime;

#ifdef STANDALONE
int	fin;
char	module[] = "Mkfs";
#endif

int	fsi;
int	fso;
char	buf[DEV_BSIZE];

union {
	struct fblk fb;
	char pad1[DEV_BSIZE];
} fbuf;

union {
	struct fs fs;
	char pad2[DEV_BSIZE];
} filsys;

int	f_n	= 100;
int	f_m	= 5;

daddr_t	alloc();

main(argc,argv)
int	argc;
char	**argv;
{
register int f, c;
	long n;
register char *size;

#ifndef STANDALONE
	time(&utime);
	if(argc < 3) {
		printf("usage: mkfs filsys proto/size [ m n ]\n");
		exit(1);
	}
	size = argv[2];
	fso = creat(argv[1], 0666);
	if(fso < 0) {
		printf("%s: cannot create\n", argv[1]);
		exit(1);
	}
	fsi = open(argv[1], 0);
	if(fsi < 0) {
		printf("%s: cannot open\n", argv[1]);
		exit(1);
	}
#else
	printf("%s\n",module);
	do {
		printf("file system: ");
		gets(buf);
		fso = open(buf, 1);
		fsi = open(buf, 0);
	} while (fso < 0 || fsi < 0);

	printf("file sys size: ");
	size = buf+128;
	gets(size);
	printf("interleaving factor (m; %d default): ", f_m);
	gets(buf);
	if (buf[0])
		f_m = atoi(buf);
	printf("interleaving modulus (n; %d default): ", f_n);
	gets(buf);
	if (buf[0])
		f_n = atoi(buf);

	if(f_n <= 0 || f_n >= MAXFN)
		f_n = MAXFN;
	if(f_m <= 0 || f_m > f_n)
		f_m = 3;
	argc = 0;
#endif
	n = 0;
	for(f=0; c=size[f]; f++) {
		if(c<'0' || c>'9') {
			printf("%s: size has nondigit\n", size);
			exit(1);
		}
		n = n*10 + (c-'0');
	}
	filsys.fs.fs_fsize = n;
	/*
	 * Minor hack for standalone root and other
	 * small filesystems: reduce ilist size.
	 */
	if (n <= 5000/CLSIZE)
		n = n/50;
	else
		n = n/25;
	if(n <= 0)
		n = 1;
	if(n > 65500/NIPB)
		n = 65500/NIPB;
	filsys.fs.fs_isize = n + 2;
	printf("isize = %D\n", n*NIPB);

	if(argc >= 5) {
		f_m = atoi(argv[3]);
		f_n = atoi(argv[4]);
		if(f_n <= 0 || f_n >= MAXFN)
			f_n = MAXFN;
		if(f_m <= 0 || f_m > f_n)
			f_m = 3;
	}
	filsys.fs.fs_step = f_m;
	filsys.fs.fs_cyl = f_n;
	printf("m/n = %d %d\n", f_m, f_n);
	if(filsys.fs.fs_isize >= filsys.fs.fs_fsize) {
		printf("%ld/%ld: bad ratio\n", filsys.fs.fs_fsize, filsys.fs.fs_isize-2);
		exit(1);
	}
	filsys.fs.fs_tfree = 0;
	filsys.fs.fs_tinode = 0;
	bzero(buf, DEV_BSIZE);
	for(n=2; n!=filsys.fs.fs_isize; n++) {
		wtfs(n, buf);
		filsys.fs.fs_tinode += NIPB;
	}

	bflist();

	fsinit();

	filsys.fs.fs_time = utime;
	wtfs((long)SBLOCK, (char *)&filsys.fs);
	exit(0);
}

/*
 * initialize the file system
 */
struct inode node;
#define PREDEFDIR 3
struct direct root_dir[] = {
	{ ROOTINO, sizeof(struct direct), 1, "." },
	{ ROOTINO, sizeof(struct direct), 2, ".." },
	{ LOSTFOUNDINO, sizeof(struct direct), 10, "lost+found" },
};
struct direct lost_found_dir[] = {
	{ LOSTFOUNDINO, sizeof(struct direct), 1, "." },
	{ ROOTINO, sizeof(struct direct), 2, ".." },
	{ 0, DIRBLKSIZ, 0, 0 },
};

fsinit()
{
	int i;

	/*
	 * initialize the node
	 */
	node.i_atime = utime;
	node.i_mtime = utime;
	node.i_ctime = utime;
	/*
	 * create the lost+found directory
	 */
	(void)makedir(lost_found_dir, 2);
	for (i = DIRBLKSIZ; i < DEV_BSIZE; i += DIRBLKSIZ)
		bcopy(&lost_found_dir[2], &buf[i], DIRSIZ(&lost_found_dir[2]));
	node.i_number = LOSTFOUNDINO;
	node.i_mode = IFDIR | UMASK;
	node.i_nlink = 2;
	node.i_size = DEV_BSIZE;
	node.i_db[0] = alloc();
	wtfs(node.i_db[0], buf);
	iput(&node);
	/*
	 * create the root directory
	 */
	node.i_number = ROOTINO;
	node.i_mode = IFDIR | UMASK;
	node.i_nlink = PREDEFDIR;
	node.i_size = makedir(root_dir, PREDEFDIR);
	node.i_db[0] = alloc();
	wtfs(node.i_db[0], buf);
	iput(&node);
}

/*
 * construct a set of directory entries in "buf".
 * return size of directory.
 */
makedir(protodir, entries)
	register struct direct *protodir;
	int entries;
{
	char *cp;
	int i, spcleft;

	spcleft = DIRBLKSIZ;
	for (cp = buf, i = 0; i < entries - 1; i++) {
		protodir[i].d_reclen = DIRSIZ(&protodir[i]);
		bcopy(&protodir[i], cp, protodir[i].d_reclen);
		cp += protodir[i].d_reclen;
		spcleft -= protodir[i].d_reclen;
	}
	protodir[i].d_reclen = spcleft;
	bcopy(&protodir[i], cp, DIRSIZ(&protodir[i]));
	return (DIRBLKSIZ);
}

daddr_t
alloc()
{
	register int i;
	daddr_t bno;

	filsys.fs.fs_tfree--;
	bno = filsys.fs.fs_free[--filsys.fs.fs_nfree];
	if(bno == 0) {
		printf("out of free space\n");
		exit(1);
	}
	if(filsys.fs.fs_nfree <= 0) {
		rdfs(bno, (char *)&fbuf);
		filsys.fs.fs_nfree = fbuf.fb.df_nfree;
		for(i=0; i<NICFREE; i++)
			filsys.fs.fs_free[i] = fbuf.fb.df_free[i];
	}
	return(bno);
}

rdfs(bno, bf)
daddr_t bno;
char *bf;
{
	int n;

	lseek(fsi, bno*DEV_BSIZE, 0);
	n = read(fsi, bf, DEV_BSIZE);
	if(n != DEV_BSIZE) {
		printf("read error: %ld\n", bno);
		exit(1);
	}
}

wtfs(bno, bf)
daddr_t bno;
char *bf;
{
	int n;

	lseek(fso, bno*DEV_BSIZE, 0);
	n = write(fso, bf, DEV_BSIZE);
	if(n != DEV_BSIZE) {
		printf("write error: %D\n", bno);
		exit(1);
	}
}

bfree(bno)
daddr_t bno;
{
	int i;

	if (bno != 0)
		filsys.fs.fs_tfree++;
	if(filsys.fs.fs_nfree >= NICFREE) {
		fbuf.fb.df_nfree = filsys.fs.fs_nfree;
		for(i=0; i<NICFREE; i++)
			fbuf.fb.df_free[i] = filsys.fs.fs_free[i];
		wtfs(bno, (char *)&fbuf);
		filsys.fs.fs_nfree = 0;
	}
	filsys.fs.fs_free[filsys.fs.fs_nfree++] = bno;
}

bflist()
{
	struct inode in;
	char flg[MAXFN];
	int adr[MAXFN];
	register int i, j;
	daddr_t f, d;

	bzero(flg, sizeof flg);
	i = 0;
	for(j=0; j<f_n; j++) {
		while(flg[i])
			i = (i+1)%f_n;
		adr[j] = i+1;
		flg[i]++;
		i = (i+f_m)%f_n;
	}

	bzero(&in, sizeof (in));
	in.i_number = 1;		/* inode 1 is a historical hack */
	in.i_mode = IFREG;
	bfree((daddr_t)0);
	d = filsys.fs.fs_fsize-1;
	while(d%f_n)
		d++;
	for(; d > 0; d -= f_n)
	for(i=0; i<f_n; i++) {
		f = d - adr[i];
		if(f < filsys.fs.fs_fsize && f >= filsys.fs.fs_isize)
			bfree(f);
	}
	iput(&in);
}

iput(ip)
register struct inode *ip;
{
	struct	dinode	buf[NIPB];
	register struct dinode *dp;
	daddr_t d;

	filsys.fs.fs_tinode--;
	d = itod(ip->i_number);
	if(d >= filsys.fs.fs_isize) {
		printf("ilist too small\n");
		return;
	}
	rdfs(d, buf);
	dp = (struct dinode *)buf;
	dp += itoo(ip->i_number);

	dp->di_addr[0] = ip->i_addr[0];
	dp->di_ic1 = ip->i_ic1;
	dp->di_ic2 = ip->i_ic2;
	wtfs(d, buf);
}
