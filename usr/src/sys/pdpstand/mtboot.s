/*
 * Primary tape boot program to load and execute secondary boot.
 *
 * Two copies of the primary boot are stored in the first and second blocks
 * of the first tape file (some boot proms execute the second block when
 * booting a tape).  The secondary boot is also stored in the first tape
 * file, starting at block #3.
 *
 * Note that this boot has been hacked to death in order to squeeze three
 * primary tape boots in for the HT, TM, and TS.  It turns out that this
 * wasn't as hard as one might think since the original primary tape boots
 * were overly ambitious in their generality.  Needless to say, that
 * generality is no longer present ...
 *
 * Note that while we are prepared to compile multiple different primary
 * bootstraps to support different tape densities (BPI), we don't bother
 * ifdef'ing out the various drivers which don't support the defined
 * BPI.  They all fit in 512 bytes now, so who cares?
 *
 * Also note that the struct exec header must be removed from this bootstrap.
 * This needs to be done so taking the addresses of the various tape rewind
 * and read functions will work.
 */
NEWLOC	= [48.*1024.]			/ we relocate ourselves to this address

#ifndef BPI
#	define	BPI	1600
#endif

busvec	= 04				/ bus error trap vector
hipri	= 0340				/ PS slp 7

a_text	= 02				/ a_text (struct exec) field offset
a_data	= 04				/ a_data (struct exec) field offset

trew	= r5				/ pointer at trew routine
tread	= r4				/ pointer at tread routine
blkcnt	= r3				/ number of blocks to read
memaddr	= r2				/ memory location to read into
					/ r1 & r0 are junk registers

.. = NEWLOC				/ so absolute address work ...
start:
	nop				/ DEC boot block standard
	br	1f			/ "   "    "     "
1:
	mov	$NEWLOC,sp		/ give ourselves a stack to work with
	cmp	pc,sp			/ are we running above the stack?
	bhis	3f			/ [yes, no need to relocate ourselves]
	mov	sp,r1			/ no, we gotta move ourselves up
	clr	r0			/ [assume we're at location 0 now]
	mov	$256.,r2		/ primary boot is at most 256 words
2:
	mov	(r0)+,(r1)+		/ move primary boot to just above
	sob	r2,2b			/   the stack
	jmp	(sp)			/ reexecute ourselves at our new loc
3:
	mov	$hipri,*$busvec+2	/ take bus errors at high priority
httest:
	mov	$1f,*$busvec+0		/ HT htcs1 register present?
	tst	*$htcs1			/   yes, if we got here: set trew and
	mov	$htrew,trew		/   tread routines and continue
	mov	$htread,tread
	br	readit
1:
tmtest:
	mov	$1f,*$busvec+0		/ TM tmmr register present? (the TM
	tst	*$tmmr			/   TS controllers use the same CSR
	mov	$tmread,tread		/   address, but tmmr is above the
	mov	$tmrew,trew		/   last CSR address of the TS ...)
	br	readit
1:
tstest:
	mov	$tsread,tread		/ else it has to be a TS ...
	mov	$tsrew,trew
readit:
	jsr	pc,*trew		/ rewind the tape
	mov	$2,blkcnt		/ skip past the two copies of our
	clr	memaddr			/   self on the tape (throw them
	jsr	pc,*tread		/   at location zero)
	mov	$1,blkcnt		/ and read the first block
	clr	memaddr			/   to location 0 ...
	jsr	pc,*tread

	mov	*$a_text,blkcnt		/ compute remaining amount to read:
	add	*$a_data,blkcnt		/   (a_text + a_data
	add	$15.,blkcnt		/    + sizeof(struct exec) + 511
	ash	$-9.,blkcnt		/    - 512) [already read one block]
	bic	$177600,blkcnt		/   / 512 [[unsigned]]
	beq	done			/ already done if == 0 [not likely]

	jsr	pc,*tread
done:
	clr	r0			/ strip the a.out header from the
	mov	$16.,r1			/   secondary boot by shifting it
1:					/   down by sizeof(struct exec)
	mov	(r1)+,(r0)+
	cmp	r1,sp
	blo	1b
	clr	pc			/ jump to location 0 ...

/*
 *			HT tape driver
 */
htcs1	= 0172440
htba	= 0172444
htfc	= 0172446
htcs2	= 0172450
htds	= 0172452
httc	= 0172472

PIP	= 020000
RESET	= 040
MOL	= 010000
ERR	= 040000
REV	= 033
READ	= 071
REW	= 07

#if BPI == 800
	HTBPI	= 01700			/ 800BPI | PDP-11 mode
#endif
#if BPI == 1600
	HTBPI	= 02300			/ 1600BPI | PDP-11 mode
#endif
#if BPI == 6250
	HTBPI	= 0			/ just so things assemble right
#endif

/*
 * Read blkcnt tape records into location memaddr.
 * Side effects:
 *	blkcnt gets zeroed
 *	memaddr += 512 * blkcnt
 *	tape is advanced blkcnt records
 */
htread:
	jsr	pc,hrrec		/ read tape records until blkcnt
	sob	blkcnt,htread		/   goes to 0 ...
	rts	pc

/*
 * Read one tape record.
 * Side effects:
 *	memaddr += 512
 *	tape is advanced 1 record
 */
hrrec:
	mov	$htds,r0
	tstb	(r0)
	bpl	hrrec
	bit	$PIP,(r0)
	bne	hrrec
	bit	$MOL,(r0)
	beq	hrrec
	mov	$htfc,r0
	mov	$-512.,(r0)
	mov	memaddr,-(r0)
	mov	$-256.,-(r0)
	mov	$READ,-(r0)
1:
	tstb	(r0)
	bpl	1b
	bit	$ERR,(r0)
	bpl	2f
	mov	$RESET,*$htcs2
	mov	$-1,*$htfc
	mov	$REV,(r0)
	br	hrrec
2:
	add	$512.,memaddr
	rts	pc

/*
 * Rewind tape.
 * Side effects:
 *	just what the first line says ...
 */
htrew:
	mov	$RESET,*$htcs2
	mov	$HTBPI,*$httc
	mov	$REW,*$htcs1
	rts	pc


/*
 *			TM tape driver
 */
tmer	= 172520
tmcs	= 172522
tmbc	= 172524
tmba	= 172526
tmdb	= 172530
tmrd	= 172532
tmmr	= 172534

#if BPI == 800
	TMBPI	= 060000
#endif
#if BPI == 1600
	TMBPI	= 000000		/ Third party TMs only
#endif
#if BPI == 6250
	TMBPI	= 020000		/ Third party TMs only
#endif

/*
 * Read blkcnt tape records into location memaddr.
 * Side effects:
 *	blkcnt gets zeroed
 *	memaddr += 512 * blkcnt
 *	tape is advanced blkcnt records
 */
tmread:
	jsr	pc,tmrrec		/ read tape records until blkcnt
	sob	blkcnt,tmread		/   goes to 0 ...
	rts	pc

/*
 * Read one tape record.
 * Side effects:
 *	memaddr += 512
 *	tape is advanced 1 record
 */
tmrrec:
	mov	$tmer,r0
	bit	$2,(r0)+		/ rewind status
	bne	tmrrec
	tstb	(r0)+			/ cu ready
	bpl	tmrrec
	inc 	r0
	mov	$-512.,(r0)+		/ byte count
	mov	memaddr,(r0)		/ bus address
	mov	$tmcs,r0
	mov	$TMBPI|3,(r0)		/ read
1:
	tstb	(r0)
	bpl	1b
	tst	(r0)+
	bpl	2f
	mov	$-1,(r0)
	mov	$TMBPI|13,-(r0)		/ backspace
	br	tmrrec
2:
	add	$512.,memaddr
	rts	pc

/*
 * Rewind tape.
 * Side effects:
 *	just what the first line says ...
 */
tmrew:
	mov	$TMBPI|17,*$tmcs
	rts	pc


/*
 *			TS tape driver
 *
 */
tsdb = 172520
tssr  = 172522

TSINIT = 140013
TSCHAR = 140004
TSREW  = 102010
TSREAD = 100001
TSRETRY = 100401

tsdbuf = NEWLOC + 512.			/ put ts data buffer just after us

/*
 * Read blkcnt tape records into location memaddr.
 * Side effects:
 *	blkcnt gets zeroed
 *	memaddr += 512 * blkcnt
 *	tape is advanced blkcnt records
 */
tsread:
	jsr	pc,tsrrec		/ read tape records until blkcnt
	sob	blkcnt,tsread		/   goes to 0 ...
	rts	pc

/*
 * Read one tape record.
 * Side effects:
 *	memaddr += 512
 *	tape is advanced 1 record
 */
tsrrec:
	tstb	tssr
	bpl	tsrrec
	mov	$tsdbuf+6,r0
	mov	$512.,(r0)
	clr	-(r0)
	mov	memaddr,-(r0)
	mov	$TSREAD,-(r0)
	mov	r0,tsdb
1:
	tstb	tssr
	bpl	1b
	cmp	$1,tssr
	blos	2f
	mov	$TSRETRY,(r0)
	mov	r0,tsdb
	br	1b
2:
	add	$512.,memaddr
	rts	pc

/*
 * Rewind tape.
 * Side effects:
 *	just what the first line says ...
 */
tsrew:
	tstb	tssr			/ initialize ...
	bpl	tsrew
	mov	$tsdbuf,r0
	mov	$TSCHAR,(r0)+
	mov	$tsdbuf+10,(r0)+
	clr	(r0)+
	mov	$10,(r0)+
	mov	r0,(r0)+
	clr	(r0)+
	mov	$16,(r0)+
	mov	$tsdbuf,tsdb
1:
	tstb	tssr
	bpl	1b
/*
 * rewind
 */
	mov	$TSREW,tsdbuf
	mov	$tsdbuf,tsdb
	rts	pc
