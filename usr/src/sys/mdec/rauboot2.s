MAJOR = 2			/ major # from bdevsw[]

/ RA bootstrap
/
/ disk boot program to load and transfer
/ to a unix entry.
/ for use with 1 KB byte blocks, CLSIZE is 2.
/ NDIRIN is the number of direct inode addresses (currently 4)
/ assembled size must be <= 512; if > 494, the 16-byte a.out header
/ must be removed

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

/ entry is made by jsr pc,*$0
/ so return can be rts pc

/ establish sp, copy
/ program up to end of core.

	240			/ These two lines must be present or DEC
	br	start		/ boot ROMs will refuse to run boot block!
start:
	mov	r0,unit		/ Save unit number passed by ROMs(and kernel)
	mov	r1,raip		/ save csr passed by ROMs (and kernel)
	mov	$..,sp
	mov	sp,r1
	clr	r0
1:
	mov	(r0)+,(r1)+
	cmp	r1,$end
	blo	1b
	jmp	*$2f

/ On error, restart from here.
restart:

/ clear core to make things clean
	clr	r0
2:
	clr	(r0)+
	cmp	r0,sp
	blo	2b
/
/ RA initialize controller
/
init:
 movb $'I,tpb
	mov	$RASTEP1,r0
	mov	raip,r1
	clr	(r1)+			/ go through controller init seq.
	mov	$icons,r2
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
 movb $'B,tpb

	jsr	pc,rmblk		/ get next block of directory
		br noboot		/ hang on eof (boot prog not found)
	mov	$buf,r2			/ offset to start of block

nextname:
 movb $'\r,tpb
 movb $'\n,tpb
 movb $'C,tpb
	cmp	r2,$buf+BSIZE		/ check for end of directory block
	bhis	scandir

/ check next directory entry in block
	mov	r2,r4
	add	$16.,r2			/ (inc to next entry)
	mov	(r4)+,r0		/ get inode number
	beq	nextname		/ skip to next if inactive

/ compare file name
	mov	$bootnm, r3		/ name of boot program
9:
 movb (r4),tpb
	cmpb	(r3)+,(r4)+		/ compare byte
	bne	nextname		/ skip to next if not same
	cmp	r4,r2			/ repeat until done
	blo	9b

/ name match!
/ read file into core until
/ a mapping error, (no disk address)

 movb $'L,tpb
	jsr	pc,iget			/ fetch boot's inode
	clr	bno			/ start at block 0 of inode in 'inod'
	clr	r1			/ memory address of boot program

1:
 movb $'.,tpb
	jsr	pc,rmblk		/ get next block into buffer
		br loaded		/ on eof go start boot

	mov	$buf,r2			/ move buf to memory
2:
	mov	(r2)+,(r1)+
	cmp	r2,$buf+BSIZE
	blo	2b			/ repeat next word
	br	1b			/ repeat next block


/ relocate core around
/ assembler header
loaded:
 movb $'R,tpb
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
 movb $'S,tpb
	jsr	pc,*$0
/	jmp	restart

noboot:
 movb $'F,tpb
 br .

/ get the inode specified in r0
iget:
	add	$INOFF,r0		/ get abs inode num relative to start of disk
	mov	r0,r5
	ash	$-4,r0			/ block number is in upper 12 bits
	bic	$!7777,r0
	mov	r0,dno			/ read that block
	jsr	pc,rblk
	bic	$!17,r5			/ calc offset of desired inode within block
	mov	$INOSIZ,r0
	mul	r0,r5
	add	$buf,r5			/ src addr
	mov	$inod,r4		/ dest addr
1:
	movb	(r5)+,(r4)+		/ move from block buffer to inode buffer
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
	add	$2,(sp)			/ update return address
	mov	bno,r0			/ get block number to read of file in inode
	cmp	r0,$NDIRIN		/ if indirect
	blt	1f
	mov	$NDIRIN,r0		/ force offset of first indirect block
1:
	mov	r0,-(sp)		/ mult by 3
	asl	r0
	add	(sp)+,r0
	add	$addr+1,r0		/ get block addr from inode
	movb	(r0)+,dno
	movb	(r0)+,dno+1
	movb	-3(r0),r0		/ if zero this is eof
	bne	1f
	tst	dno
	beq	eof
1:
	jsr	pc,rblk			/ read the block
	mov	bno,r0
	inc	bno			/ inc block pointer for next time
	sub	$NDIRIN,r0		/ if not indirect 
	blt	1f			/ return now
	ash	$2,r0
	mov	buf+2(r0),dno		/ get the next level pointer
	mov	buf(r0),r0
	bne	rblk			/ if not EOF
	tst	dno
	bne	rblk			/ go read the block
eof:
	sub	$2,(sp)
1:
	rts	pc

read	= 4
go	= 1

/
/ RA MSCP read block routine.  This is very primative, so don't expect
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
1:
	tst	(r0)
	beq	1b			/ Wait till command read
	clr	(r0)+			/ Tell controller we saw it, ok.
2:
	tst	(r0)
	beq	2b			/ Wait till response written
	clr	(r0)			/ Tell controller we go it
	rts	pc

icons:	RAERR
	ra+RARING
	0
	RAGO

bootnm:	<boot\0\0\0\0\0\0\0\0\0\0>
unit: 0				/ unit number from ROMs
raip: 0				/ csr address from ROMs
end:

inod = ..-512.-BSIZE		/ room for inod, buf, stack
addr = inod+ADDROFF		/ first address in inod
buf = inod+INOSIZ
bno = buf+BSIZE
dno = bno+2
ra = dno + 2			/ ra mscp communications area (BIG!)
