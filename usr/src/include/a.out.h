#ifndef	NOVL
#define	NOVL	15

#define	A_MAGIC1	0407		/* normal */
#define	A_MAGIC2	0410		/* read-only text */
#define	A_MAGIC3	0411		/* separated I&D */
#define	A_MAGIC4	0405		/* overlay */
#define	A_MAGIC5	0430		/* auto-overlay (nonseparate) */
#define	A_MAGIC6	0431		/* auto-overlay (separate)  */

#ifndef	KERNEL
struct	exec {
	int		a_magic;	/* magic number */
	unsigned	a_text; 	/* size of text segment */
	unsigned	a_data; 	/* size of initialized data */
	unsigned	a_bss;  	/* size of uninitialized data */
	unsigned	a_syms; 	/* size of symbol table */
	unsigned	a_entry; 	/* entry point */
	unsigned	a_unused;	/* not used */
	unsigned	a_flag; 	/* relocation info stripped */
};

struct	ovlhdr {
	int		max_ovl;	/* maximum ovl size */
	unsigned	ov_siz[NOVL];	/* size of i'th overlay */
};

#include <nlist.h>

/*
 * Macros which take exec structures as arguments and tell whether
 * the file has a reasonable magic number or offset to text.
 */
#define	N_BADMAG(x) \
	(((x).a_magic)!=A_MAGIC1 && ((x).a_magic)!=A_MAGIC2 && \
	((x).a_magic)!=A_MAGIC3 && ((x).a_magic)!=A_MAGIC4 && \
	((x).a_magic)!=A_MAGIC5 && ((x).a_magic)!=A_MAGIC6)

#define	N_TXTOFF(x) \
	((x).a_magic==A_MAGIC5 || (x).a_magic==A_MAGIC6 ? \
	sizeof (struct ovlhdr) + sizeof (struct exec) : sizeof (struct exec))

#endif	KERNEL
#endif	NOVL
