#define	NRM	0
/* #define RM_DKN	0		/* drive # for iostat disk monitoring */
/* #define RM_DUMP				/* include dump routine */
/* #define	RM_RM05			/* drive type is RM05, not RM02/3 */
#if NRM > 1
#define	UCB_DBUFS		/* parallel xfers for multiple drives */
#endif
