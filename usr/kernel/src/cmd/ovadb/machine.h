#
/*
 *	UNIX/INTERDATA debugger
 */

/* unix parameters */
#define DBNAME "adb\n"
#define LPRMODE "%Q"
#define OFFMODE "+%o"
#define TXTRNDSIZ 8192L

TYPE	unsigned TXTHDR[8];
TYPE	struct ovhead	OVLVEC;
TYPE	int  SYMV;
TYPE	char  OVTAG;

/* symbol table in a.out file */
struct symtab {
	char	symc[8];
	char	symf;
	OVTAG	ovnumb;
	SYMV	symv;
};
#define SYMTABSIZ (sizeof (struct symtab))

#define SYMCHK 047
#define SYMTYPE(symflag) (( (int)symflag>=041 || ((int)symflag>=02 && (int)symflag<=04))\
				?  (((int)symflg&07)>=3 ? DSYM : ((int)symflg&07))\
				: NSYM\
			)
struct	ovhead {
	unsigned	max;
	unsigned	ov[7];
};
