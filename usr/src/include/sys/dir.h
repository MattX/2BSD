#ifndef MAXNAMLEN
#define MAXNAMLEN	14
#endif !MAXNAMLEN

#ifdef KERNEL

struct	direct {
	ino_t	d_ino;
	char	d_name[MAXNAMLEN];
};

#else !KERNEL

#define DIRBLKSIZ	512

/*
 * The DIRSIZ macro gives the minimum record length which will hold
 * the directory entry.  This requires the amount of space in struct direct
 * without the d_name field, plus enough space for the name with a terminating
 * null byte (dp->d_namlen+1), rounded up to a 4 byte boundary.
 */
#define DIRSIZ(dp) \
    ((sizeof (struct direct) - (MAXNAMLEN+1)) + (((dp)->d_namlen+1 + 3) &~ 3))

struct	direct {
	u_long	d_ino;			/* inode number of entry */
	u_short	d_reclen;		/* length of this record */
	u_short	d_namlen;		/* length of string in d_name */
	char	d_name[MAXNAMLEN + 1];	/* name must be no longer than this */
};

/*
 * Definitions for library routines operating on directories.
 */

typedef struct _dirdesc {
	int	dd_fd;
	long	dd_loc;
	long	dd_size;
	char	dd_buf[DIRBLKSIZ];
} DIR;

#ifndef NULL
#define NULL	0		/* stdio.h */
#endif !NULL

#define rewinddir(dirp)	seekdir((dirp), (long)0)

extern	DIR *opendir();
extern	struct direct *readdir();
extern	long telldir();
extern	void seekdir();
extern	void closedir();

#endif KERNEL
