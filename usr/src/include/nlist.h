/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)nlist.h	2.2 (2.11BSD GTE) 1/06/94
 */

#ifndef	_NLIST_H_
#define	_NLIST_H_
#include <sys/types.h>

/*
 * Symbol table entry format.  The #ifdef's are so that programs including
 * nlist.h can initialize nlist structures statically.
 */

struct	oldnlist {
	char	n_name[8];	/* symbol name */
	int	n_type;		/* type flag */
unsigned int	n_value;	/* value */
};

struct	nlist {
#ifdef	_AOUT_INCLUDE_
	union {
		char *n_name;	/* In memory address of symbol name */
		off_t n_strx;	/* String table offset (file) */
	} n_un;
#else
	char	*n_name;	/* symbol name (in memory) */
#endif
	u_char	n_type;		/* Type of symbol - see below */
	char	n_ovly;		/* Overlay number */
	u_int	n_value;	/* Symbol value */
};

/*
 * Simple values for n_type.
 */
#define	N_UNDF	0x0		/* undefined */
#define	N_ABS	0x1		/* absolute */
#define	N_TEXT	0x2		/* text symbol */
#define	N_DATA	0x3		/* data symbol */
#define	N_BSS	0x4		/* bss symbol */
#define	N_REG	0x14		/* register name */
#define	N_FN	0x1f		/* file name symbol */

#define	N_EXT	0x20		/* external bit, or'ed in */
#define	N_TYPE	0x1f		/* mask for all the type bits */

/*
 * Format for namelist values.
 */
#define	N_FORMAT	"%06o"
#endif	/* _NLIST_H_ */
