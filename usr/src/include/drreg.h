/*
 *	Registers of the DR11-W parallel DMA interface
 */
struct	drdevice {
	short	wcr;		/* word count register */
	short	bar;		/* bus address register */
	short	csr;		/* csr/eir register */
	short	dar;		/* input/output data register */
};

/* Bits of the csr */

#define	DR_GO	0000001		/* start transfer */
#define	DR_FN1	0000002		/* user defined function bits */
#define	DR_FN2	0000004
#define	DR_FN3	0000010
#define DR_XBA	0000060		/* Unibus extension bits */
#define	DR_IE	0000100		/* interrupt enable */
#define	DR_RDY	0000200		/* ready bit */
#define	DR_CYL	0000400		/* cycle start */
#define	DR_ST1	0001000		/* user defined status */
#define	DR_ST2	0002000
#define	DR_ST3	0004000
#define	DR_MANT	0010000		/* maintenance mode bit */
#define	DR_ATTN	0020000		/* attention bit from interface */
#define	DR_NEX	0040000		/* non-existant memory */
#define	DR_ERR	0100000		/* general error bit */

#define	DRCSR_BITS	\
"\10\20ERR\17NEX\16ATTN\15MAINT\14STA\13STB\12STC\
\11CYCL\10RDY\7IE\6XBA17\5XBA16\4FN3\3FN2\2FN1\1GO"

/* Bits of the EIR */
#define	DR_FLG	0000001		/* register flag 1=EIR, 0=CSR */
#define	DR_NCYL	0000400		/* N-cycle burst selected */
#define	DR_BDL	0001000		/* Burst data late */
#define	DR_PAR	0002000		/* parity error */
#define	DR_ACLO	0004000		/* powerfailure */
#define	DR_MRQ	0010000		/* multicycle request */

/* All remaining bits same as CSR */
#define	DREIR_BITS	\
	"\10\20ERR\17NEX\16ATTN\15MRQ\14ACLO\13PAR\12BDL\11NCYL\1REG"

/*
 *	Definitions for ioctl calls for DR11-W interface
 */
#define	DRGTTY	0		/* get flags and function bits of device */
#define	DRSTTY	1		/* set flags and function bits of device */
#define	DRSFUN	2		/* set function bits of device */
#define	DRSFLAG	3		/* set flags of device */
#define	DRGCSR	4		/* read csr and wcr of device */
#define	DRSSIG	5		/* set signal to use on ATTN interrupt */
#define	DRESET	6		/* reset DR11-W interface */

/*
 *	Definitions for ioctl calls for DR11-W interface
 */
#define	DRGTTY		0	/* get flags and function bits of device */
#define	DRSTTY		1	/* set flags and function bits of device */
#define	DRSFUN		2	/* set function bits of device */
#define	DRSFLAG		3	/* set flags of device */
#define	DRGCSR		4	/* read csr and wcr of device */
#define	DRSSIG		5	/* set signal to use on ATTN interrupt */
#define	DRESET		6	/* reset DR11-W interface */
#define DRSTIME		7
#define DRCTIME		8
#define DRITIME		9
#define DROUTPUT	10
#define DRINPUT		11
