MAJOR = 2			/ major # from bdevsw[]

/ RA bootstrap
/
/ ported version from 2.9 BSD-MSCP
/
/ disk boot program to load and transfer
/ to a unix entry.
/ for use with 1 KB byte blocks, CLSIZE is 2.
/ NDIRIN is the number of direct inode addresses (currently 4)
/ assembled size must be <= 512; if > 494, the 16-byte a.out header
/ must be removed

/ removed debug output to get a clean unix boot prompt
/ Peter Klapper, 31.12.2020

MSCPSIZE =	64.	/ One MSCP command packet is 64bytes long (need 2)

RASEMAP	=	140000	/ RA controller owner semaphore

RAERR =		100000	/ error bit 
RASTEP1 =	04000	/ step1 has started
RAGO =		01	/ start operation, after init
RASTCON	= 	4	/ Setup controller info 
RAONLIN	=	11	/ Put unit on line
RAREAD =	41	/ Read command code
RAWRITE =	42	/ Write command code
RAEND =		200	/ End command code

RACMDI =	4.	/ Command Interrupt
RARSPI =	6.	/ Response Interrupt
RARING =	8.	/ Ring base
RARSPL =	8.	/ Response Command low
RARSPH = 	10.	/ Response Command high
RACMDL =	12.	/ Command to controller low
RACMDH =	14.	/ Command to controller high
RARSPS =	16.	/ Response packet length (location)
RARSPREF =	20.	/ Response reference number
RACMDS =	80.	/ Command packet length (location)
RACMDREF =	84.	/ Command reference number
RAUNIT = 	88.	/ Command packet unit 
RAOPCODE =	92.	/ Command opcode offset
RABYTECT =	96.	/ Transfer byte count
RABUFL =	100.	/ Buffer location (16 bit addressing only)
RABUFH = 	102.	/ Buffer location high 6 bits
RALBNL =	112.	/ Logical block number low
RALBNH = 	114.	/ Logical block number high

/ options: none.  all options of reading an alternate name or echoing to
/		  the keyboard had to be removed to make room for the 
/		  code which understands the new directory structure on disc
/		  also, this is the single largest boot around to begin with.

/ constants:
/
CLSIZE	= 2.			/ physical disk blocks per logical block
CLSHFT	= 1.			/ shift to multiply by CLSIZE
BSIZE	= 512.*CLSIZE		/ logical block size (1024)
INOSIZ	= 64.			/ size of inode in bytes
NDIRIN	= 4.			/ number of direct inode addresses
ADDROFF	= 12.			/ offset of first address in inode
INOPB	= BSIZE\/INOSIZ		/ inodes per logical block (16)
INOFF	= 31.			/ inode offset = (INOPB * (SUPERB+1)) - 1
PBSHFT	= -4			/ shift to divide by inodes per block

tps = 177564
tpb = 177566

/
/  The boot options and device are placed in the last SZFLAGS bytes
/  at the end of core by the kernel if this is an autoboot.
/
ENDCORE=	160000		/ end of core, mem. management off
SZFLAGS=	6		/ size of boot flags
BOOTOPTS=	2		/ location of options, bytes below ENDCORE
BOOTDEV=	4
CHECKWORD=	6

.. = ENDCORE-512.-SZFLAGS	/ save room for boot flags


unit:	240			/ These two lines must be present or DEC
raip:	br	start		/ boot ROMs will refuse to run boot block!
				/ This is data, not code, even though it
				/ executes as nops. If you restart during
				/ debug: r0=unit, r1=CSR, pc=14

icons:	RAERR
	ra+RARING
	0
	RAGO

/ establish sp, copy
/ program up to end of core.

start:
	mov	$..,sp
	mov	sp,r3
	clr	r2
1:
	mov	(r2)+,(r3)+
	cmp	r3,$end
	blo	1b
	jmp	*$2f

/ clear core to make things clean
2:
	clr	(r2)+
	cmp	r2,sp
	blo	2b

/ save stuff passed in registers from boot ROMs
	mov	r0,(r2)+	/ to unit, save unit number passed by ROMs(and kernel)
	mov	r1,(r2)+	/ to raip, save csr passed by ROMs (and kernel)

/
/ RA initialize controller
/
init:
	mov	$putc,r5
	/ jsr	r0,*r5;'I		/ debug output

	mov	$RASTEP1,r0
	clr	(r1)+			/ go through controller init seq.
1:
	bit	r0,(r1)
	beq	1b
	mov	(r2)+,(r1)
	asl	r0
	bpl	1b

	mov	$ra+RARSPREF,*$ra+RARSPL / set controller characteristics
	mov	$ra+RACMDREF,*$ra+RACMDL
	mov	$RASTCON,r0
	jsr	pc,racmd
	mov	unit,*$ra+RAUNIT	/ bring boot unit online
	mov	$RAONLIN,r0
	jsr	pc,racmd

	mov	$2,r0			/ read inode of root directory
	jsr	pc,iget
	clr	bno			/ pointer to current directory block

/ directory entry:
/ 0: file inode, if 0, inactive
/ 2: file name (14 bytes)

scandir:
	/ jsr	r0,*r5;'B		/ debug output

	jsr	pc,rmblk		/ get next block of directory
		br noboot		/ eof (boot prog not found)
	mov	$buf,r2			/ offset to start of block

nextname:
	/ jsr	r0,*r5;'\r		/ debug output
	/ jsr	r0,*r5;'\n		/ debug output

	cmp	r2,$buf+BSIZE		/ check for end of directory block
	bhis	scandir
	/ jsr	r0,*r5;'-		/ debug output

/ check next directory entry in block
	mov	r2,r4
	add	$16.,r2			/ (inc to next entry)
	mov	(r4)+,r0		/ get inode number
	beq	nextname		/ skip to next if inactive

/ compare file name
	mov	$bootnm, r3		/ name of boot program
9:
	jsr	r0,putc4
	cmpb	(r3)+,(r4)+		/ compare byte
	bne	nextname		/ skip to next if not same
	cmp	r4,r2			/ repeat until done
	blo	9b

/ name match!
/ read file into core until
/ a mapping error, (no disk address)

	jsr	pc,iget			/ fetch boot's inode
	clr	bno			/ start at block 0 of inode in 'inod'
	clr	r3			/ memory address of boot program

1:
	jsr	pc,rmblk		/ get next block into buffer
		br loaded		/ on eof go start boot

	/ jsr	r0,*r5;'.		/ debug output

	mov	$buf,r2			/ move buf to memory
2:
	mov	(r2)+,(r3)+
	cmp	r2,$buf+BSIZE
	blo	2b			/ repeat next word
	br	1b			/ repeat next block


/ relocate core around
/ assembler header
loaded:
	clr	r0			/ check for magic cookie
	cmp	(r0),$407
	bne	2f
1:
	mov	20(r0),(r0)+		/ if found, shuffle entire core by 32 bytes
	cmp	r0,sp
	blo	1b

/ enter program and
/ restart if return
2:
	mov	ENDCORE-BOOTOPTS, r4
	bpl	4f
	clr	r4
4:
	mov	unit, r3
	bis	$MAJOR\<8.,r3
	mov	ENDCORE-CHECKWORD, r2
	mov	raip,r1

	/ jsr	r0,*r5;'S		/ debug output
	/ jsr	r0,*r5;'\r		/ debug output
	/ jsr	r0,*r5;'\n		/ debug output

	jsr	pc,*$0

noboot:
	/ jsr	r0,*r5;'F		/ debug output
	/ br .				/ debug output

/ get the inode specified in r0
iget:
	add	$INOFF,r0		/ get abs inode num relative to start of disk
	mov	r0,r3
	ash	$-4,r0			/ block number is in upper 12 bits
	bic	$!7777,r0
	mov	r0,dno			/ read that block
	jsr	pc,rblk
	bic	$!17,r3			/ calc offset of desired inode within block
	mov	$INOSIZ,r0
	mul	r0,r3
	add	$buf,r3			/ src addr
	mov	$inod,r4		/ dest addr
1:
	movb	(r3)+,(r4)+		/ move from block buffer to inode buffer
	sob	r0,1b
	rts	pc

/ read a mapped block from file in inod
/ block offset in file is in bno.
/ skip if success, no skip if eof
/ the algorithm only handles the first
/ indirect block. that means that
/ files longer than NDIRIN+128 blocks cannot
/ be loaded.
rmblk:
	mov	bno,r1			/ get block number to read of file in inode
	inc	bno			/ inc block pointer for next time
	mov	r1,r2
	cmp	r1,$NDIRIN		/ if indirect
	blt	1f
	mov	$NDIRIN,r1		/ force offset of first indirect block
1:
	mul	$3,r1
	add	$addr,r1		/ get block addr from inode
	movb	(r1)+,r0		/ if zero this is eof
	movb	(r1)+,dno
	movb	(r1)+,dno+1
	bis	dno,r0
	bne	1f
eof:	rts	pc

1:	sub	$NDIRIN,r2		/ if not indirect 
	blt	1f

	jsr	pc,rblk			/ read the block
	ash	$2,r2
	add	$buf,r2
	mov	(r2)+,r0
	mov	(r2),dno		/ get the next level pointer
	bis	(r2),r0
	beq	eof			/ if zero, it's eof

1:	add	$2,(sp)			/ update return address

/
/ RA MSCP read block routine.  This is very primitive, so don't expect
/ too much from it.  Note that MSCP requires large data communications
/ space at end of ADDROFF for command area.
/ N.B.  This MUST preceed racmd - a "jsr/rts" sequence is saved by
/	falling thru!
/
/	dno	->	1k block # to load (low half)
/	buf	->	address of buffer to put block into
/	BSIZE	->	size of block to read
/ 
/ Tim Tucker, Gould Electronics, August 23rd 1985
/
rblk:
	mov	dno,r0
	asl	r0
	mov	r0,*$ra+RALBNL		/ Put in disk block number
	mov	$BSIZE,*$ra+RABYTECT	/ Put in byte to transfer
	mov	$buf,*$ra+RABUFL	/ Put in disk buffer location
	mov	$RAREAD,r0

/
/ perform MSCP command -> response poll version
/
racmd:
	movb	r0,*$ra+RAOPCODE	/ fill in command type
	mov	$MSCPSIZE,*$ra+RARSPS	/ give controller struct sizes
	mov	$MSCPSIZE,*$ra+RACMDS
	mov	$RASEMAP,*$ra+RARSPH	/ set mscp semaphores
	mov	$RASEMAP,*$ra+RACMDH
	mov	*raip,r0		/ tap controllers shoulder
	mov	$ra+RACMDI,r0

	jsr	pc,1f
1:
	tst	(r0)
	beq	1b
	clr	(r0)+
	rts	pc

tps = 177564
tpb = 177566
/ print a teletype character
putc4:
	/ movb (r4),*$tpb
	br 1f
putc:
	/ mov (r0)+,*$tpb
1:	tstb *$tps
	bge 1b
	rts r0


bootnm:	<boot\0\0\0\0\0\0\0\0\0\0>
end:

inod = ..-512.-BSIZE		/ room for inod, buf, stack
addr = inod+ADDROFF		/ first address in inod
buf = inod+INOSIZ
bno = buf+BSIZE
dno = bno+2
ra = dno + 2			/ ra mscp communications area (BIG!)
