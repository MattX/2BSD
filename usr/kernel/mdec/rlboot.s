/ funky RL01 initial block bootstrap. wf jolitz 12/13/79
/ menlo park unix system
/
/ Fixed up for 1k blocks 2/16/81 - Bob Kridle
/
/ disk boot program to load and transfer
/ to a unix entry.
/ for use with 1 KB byte blocks.
/ NDIRIN is the number of direct inode addresses (currently 4)

NBOO	= 1.		/ number of boot blocks
NSUP	= 1.		/ number of super blocks
CLSIZE	= 2.		/ number of physical disk blocks per logical block
INOPB	= 16.		/ inodes per logical block
INOSIZ	= 64.		/ size of inode in bytes
INOFF	= 31.		/ inode offset = INOPB * (NBOO+NSUP) - 1
WC	= -512.		/ word count 256 * CLSIZE
NDIRIN	= 4.		/ number of direct inode addresses

/ entry is made by jsr pc,*$0
/ so return can be rts pc

core = 28.
.. = [core*2048.]-512.

/ establish sp and check if running below
/ intended origin, if so, copy
/ program up to 'core' K words.
start:
	mov	$..,sp
	mov	sp,r1
	cmp	pc,r1
	bhis	2f
	clr	r0
	cmp	(r0),$407
	bne	1f
	mov	$20,r0
1:
	mov	(r0)+,(r1)+
	cmp	r1,$end
	blo	1b
	jmp	(sp)

/ clear core to make things clean
2:
	clr	(r0)+
	cmp	r0,sp
	blo	2b

/ at origin, read pathname,
/ initialize rl

/	mov	$13,*$rlda	/get status
/	mov	$4,*$rlcs
/
/	jsr pc,rdy
/	mov *$rlda,r5	/superstision
/
/ spread out in array 'names', one
/ component every 14 bytes.
	mov	$names,r1
1:
	mov	r1,r2
2:
	jsr	pc,getc
	cmp	r0,$'\n
	beq	1f
	cmp	r0,$'/
	beq	3f
	movb	r0,(r2)+
	br	2b
3:
	cmp	r1,r2
	beq	2b
	add	$14.,r1
	br	1b

/ now start reading the inodes
/ starting at the root and
/ going through directories
1:
	mov	$names,r1
	mov	$2,r0
1:
	clr	bno
	jsr	pc,iget
	tst	(r1)			/ A complete match!!
	beq	1f
2:
	jsr	pc,rmblk
		br start
	mov	$buf,r2
3:					/ Read a dir entry
	mov	r1,r3
	mov	r2,r4
	add	$16.,r2
	tst	(r4)+			/ Unassigned entry
	beq	5f			/ get another	
4:
	cmpb	(r3)+,(r4)+
	bne	5f
	cmp	r4,r2
	blo	4b
	mov	-16.(r2),r0
	add	$14.,r1			/ We have a segment match!!
	br	1b
5:
	cmp	r2,$buf+1024.		/ change when changing CLSIZE
	blo	3b			/ Not at end of dir block
	br	2b			/ Get another dir block

/ read file into core until
/ a mapping error, (no disk address)
1:
	clr	r1
1:
	jsr	pc,rmblk
		br 1f
	mov	$buf,r2
2:
	mov	(r2)+,(r1)+
	cmp	r2,$buf+1024.		/ change when changing CLSIZE
	blo	2b
	br	1b
/ relocate core around
/ assembler header
1:
	clr	r0
	cmp	(r0),$407
	bne	2f
1:
	mov	20(r0),(r0)+
	cmp	r0,sp
	blo	1b
/ enter program and
/ restart if return
2:
	jsr	pc,*$0
	br	start

/ get the inode specified in r0
iget:
	add	$INOFF,r0
	mov	r0,r5
	ash	$-4.,r0
	bic	$!7777,r0
	mov	r0,dno
	clr	r0
	jsr	pc,rblk
	bic	$!17,r5
	mul	$INOSIZ,r5
	add	$buf,r5
	mov	$inod,r4
1:
	mov	(r5)+,(r4)+
	cmp	r4,$inod+64.
	blo	1b
	rts	pc

/ read a mapped block
/ offset in file is in bno.
/ skip if success, no skip if fail
/ the algorithm only handles a single
/ indirect block. that means that
/ files longer than NDIRIN+128 blocks cannot
/ be loaded.
rmblk:
	add	$2,(sp)
	mov	bno,r0
	cmp	r0,$NDIRIN
	blt	1f
	mov	$NDIRIN,r0
1:
	mov	r0,-(sp)
	asl	r0
	add	(sp)+,r0
	add	$addr+1,r0
	movb	(r0)+,dno
	movb	(r0)+,dno+1
	movb	-3(r0),r0
	bne	1f
	tst	dno
	beq	2f
1:
	jsr	pc,rblk
	mov	bno,r0
	inc	bno
	sub	$NDIRIN,r0
	blt	1f
	ash	$2,r0
	mov	buf+2(r0),dno
	mov	buf(r0),r0
	bne	rblk
	tst	dno
	bne	rblk
2:
	sub	$2,(sp) 	/ Error exit - boot will restart
1:
	rts	pc

read	= 6\<1
SEEK	= 3\<1
RDHDR	= 4\<1
go	= 0

rlcs	= 174400
rlda	= 174404
rlba	= 174402
rlmp	= 174406
/ rl01 disk driver.
/ wfj 12/13/79 @menlo park
/ low order address in dno,
/ high order in r0.
rblk:
	mov	r1,-(sp)
	mov	r2,-(sp)
	mov	dno,r1
	ashc	$1,r0		/ multiply by CLSIZE
	div	$20.,r0
	ash	$6,r0		/align cylinder adddress for based arith.
	asl	r1
	bis	r1,r0
	mov	r0,r2
	mov	$RDHDR,*$rlcs	/find out where we are (cyl)
	jsr	pc,rdy
	mov	*$rlmp,r1
	bic	$!77600,r1
	bic	$!77600,r0
	sub	r1,r0		/ compute how much to move
	ble	2f
	bis	$4,r0		/ move head inward
	br	3f
2:	neg	r0		/ or outward
3:	bit	$100,r2		/ top or bottom surface?
	beq	1f
	bis	$20,r0
1:	bis	$1,r0
	mov	r0,*$rlda
	mov	$SEEK,*$rlcs	/ move it
	jsr	pc,rdy
	mov	r2,r0
/
	mov	$rlmp,r1
	mov	$WC,(r1)	/wc into rlmp
	mov	r0,-(r1)	/da into rlda
	mov	$buf,-(r1)	/ba
	mov	$read+go,-(r1)	/cmd into rlcs
	jsr	pc,rdy
	mov	(sp)+,r2
	mov	(sp)+,r1
	rts	pc


rdy:	tstb	*$rlcs
	bpl	rdy
	rts	pc
tks = 177560
tkb = 177562
csw = 177570
/ read and echo a teletype character
getc:
/
/	I had to remove all this good stuff to over ride the default
/	name in order to make this < 512 - RK 02/16/81
/
/	cmp	$0, csw
/	bne	2f
	movb	*cp, r0
	inc	cp
	br	putc
2:
/	mov	$tks,r0
/	inc	(r0)
1:
/	tstb	(r0)
/	bge	1b
/	mov	tkb,r0
/	bic	$!177,r0
/	cmp	r0,$'A
/	blo	1f
/	cmp	r0,$'Z
/	bhi	1f
/	add	$'a-'A,r0
1:

tps = 177564
tpb = 177566
/ print a teletype character
putc:
	tstb	*$tps
	bge	putc
	mov	r0,*$tpb
	cmp	r0,$'\r
	bne	1f
	mov	$'\n,r0
	br	putc
1:
	rts	pc

cp:	defnm
defnm:	<1kboot\r>
end:
inod = ..-1536.		/ change when changing CLSIZE
addr = inod+12.
buf = inod+64.
bno = buf+1024.		/ change when changing CLSIZE
dno = bno+2
names = dno+2
reset = 5
