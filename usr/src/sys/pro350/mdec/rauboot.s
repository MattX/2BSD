/ RA bootstrap
/
/ disk boot program to load and transfer
/ to a unix entry.
/ for use with 1 KB byte blocks, CLSIZE is 2.
/ NDIRIN is the number of direct inode addresses (currently 4)
/ assembled size must be <= 512; if > 494, the 16-byte a.out header
/ must be removed

/ options:
nohead	= 1		/ 0->normal, 1->this /boot must have a.out
			/   header removed.  Saves xx bytes.
readname= 0		/ 1->normal, if default not found, read name
			/   from console. 0->loop on failure, saves 36 bytes
prompt	= 0		/ 1->prompt ('>') before reading from console
			/   0-> no prompt, saves 8 bytes
autoboot= 1		/ 1->code for autoboot. 0->no autoboot, saves 12 bytes
putname = 0		/ 1->print out name. 0->do not, save xx bytes.
tree = 0		/ 1->accept full pathname. 0->root dir only.
qbus = 1		/ 1->for qbus cntrl. 0->for uda50

/ constants:
CLSIZE	= 2.			/ physical disk blocks per logical block
CLSHFT	= 1.			/ shift to multiply by CLSIZE
BSIZE	= 512.*CLSIZE		/ logical block size
INOSIZ	= 64.			/ size of inode in bytes
NDIRIN	= 4.			/ number of direct inode addresses
ADDROFF	= 12.			/ offset of first address in inode
INOPB	= BSIZE\/INOSIZ		/ inodes per logical block
INOFF	= 31.			/ inode offset = (INOPB * (SUPERB+1)) - 1
WC	= -256.*CLSIZE		/ word count

/  The boot options and device are placed in the last SZFLAGS bytes
/  at the end of core by the kernel if this is an autoboot.
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
start:
	mov	$..,sp
	mov	sp,r1
.if	qbus
	mov	$4,r0
.endif
.if	qbus-1
	clr	r0
.endif
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

/ initialize ra controller
unit = 1		/ drive unit #
raip = 172150
rasa = 172152
ra_step1 = 4000
ra_i1 = 100000
ra_step2 = 10000
ra_step3 = 20000
ra_step4 = 40000
ra_go = 1
op_stcon = 4
op_onlin = 11
op_read = 41
raset = 100000
dscsiz = 60.
init:
	mov	$rasa,r2	/ use r2 as reg. reference (saves text space)
	clr	*$raip		/ start hard init
1:
	bit	$ra_step1,(r2)	/ test for start of step1
	beq	1b
	mov	$ra_i1,(r2)	/ step 1
1:
	bit	$ra_step2,(r2)	/ test for start of step 2
	beq	1b
	mov	$rspdsc,(r2)	/ step 2 - low order addr. of ring
1:
	bit	$ra_step3,(r2)	/ test for start of step 3
	beq	1b
	clr	(r2)		/ step 3 - high order addr. of ring
1:
	bit	$ra_step4,(r2)	/ test for start of step 4
	beq	1b
	mov	$ra_go,(r2)	/ step 4 go
	mov	$rspref,rspdsc	/ set up descriptor addr.
	mov	$cmdref,cmddsc
	clr	cmdflg
	mov	$op_stcon,r0
	jsr	pc,racmd	/ do start connection cmd
	mov	$unit,cmdunit	/ set up drive unit
	mov	$op_onlin,r0	/ do online cmd
	jsr	pc,racmd

/ at origin, read pathname
.if	prompt
	mov	$'>, r0
	jsr	pc, putc
.endif

/ spread out in array 'names', one
/ component every 14 bytes.
	mov	$names,r1
1:
	mov	r1,r2
2:
	jsr	pc,getc
	cmp	r0,$'\r
	beq	1f
.if	tree
	cmp	r0,$'/
	beq	3f
.endif
	movb	r0,(r2)+
	br	2b
.if	tree
3:
	cmp	r1,r2
	beq	2b
	add	$14.,r1
	br	1b
.endif

/ now start reading the inodes
/ starting at the root and
/ going through directories
1:
	mov	$names,r1
	mov	$2,r0
1:
	clr	bno
	jsr	pc,iget
	tst	(r1)
	beq	1f
2:
	jsr	pc,rmblk
		br .
	mov	$buf,r2
3:
	mov	r1,r3
	mov	r2,r4
	add	$16.,r2
	tst	(r4)+
	beq	5f
4:
	cmpb	(r3)+,(r4)+
	bne	5f
	cmp	r4,r2
	blo	4b
	mov	-16.(r2),r0
	add	$14.,r1
	br	1b
5:
	cmp	r2,$buf+BSIZE
	blo	3b
	br	2b

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
	cmp	r2,$buf+BSIZE
	blo	2b
	br	1b
/ relocate core around
/ assembler header
1:
.if	nohead-1
	clr	r0
	cmp	(r0),$407
	bne	2f
1:
	mov	20(r0),(r0)+
	cmp	r0,sp
	blo	1b
.endif
/ enter program and
2:
.if	autoboot
	mov	ENDCORE-BOOTOPTS, r4
	bpl	4f
	clr	r4
	clr	r2
4:
	mov	ENDCORE-BOOTDEV, r3
	mov	ENDCORE-CHECKWORD, r2
.endif
	jsr	pc,*$0

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
	cmp	r4,$inod+INOSIZ
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
	sub	$2,(sp)
1:
	rts	pc

/ ra disk driver
/ low order address in dno
/ high order in r0
rblk:
	mov	r1,-(sp)
	mov	dno,r1
.if	CLSIZE-1
	ashc	$1,r0		/ mult. by CLSIZE
.endif
	mov	r0,cmdlbn+2	/ get high order disk addr.
	mov	r1,cmdlbn	/ get low order disk addr.
	clr	cmdcnt+2
	mov	$BSIZE,cmdcnt	/ set up byte cnt.
	clr	cmdbuf+2		/ and buffer addr.
	mov	$buf,cmdbuf
	mov	$op_read,r0	/ read op
	jsr	pc,racmd	/ do the read op.
	mov	(sp)+,r1	/ restore r1
	rts	pc

/ This function performs the op passed in r0 on the ra
racmd:
	movb	r0,cmdop	/ set up op
	mov	$dscsiz,rsplen	/ and descriptor sizes
	mov	$dscsiz,cmdlen
	bis	$raset,rspdsc+2	/ set port ownership
	bis	$raset,cmddsc+2
	mov	*$raip,r0	/ start polling
1:
	tst	rspdsc+2	/ test for completion
	blt	1b
	rts	pc

tks = 177560
tkb = 177562
/ read and echo a teletype character
/ if *cp is nonzero, it is the next char to simulate typing
/ after the defnm is tried once, read a name from the console
getc:
	movb	*cp, r0
	beq	2f
	inc	cp
.if	readname
	br	putc
2:
	mov	$tks,r0
	inc	(r0)
1:
	tstb	(r0)
	bge	1b
	mov	tkb,r0
	bic	$!177,r0
	cmp	r0,$'A
	blo	2f
	cmp	r0,$'Z
	bhi	2f
	add	$'a-'A,r0
.endif
2:

tps = 177564
tpb = 177566
/ print a teletype character
putc:
.if	putname
	tstb	*$tps
	bge	putc
	mov	r0,*$tpb
	cmp	r0,$'\r
	bne	1f
	mov	$'\n,r0
	br	putc
1:
	cmp	r0,$'\n
	bne	4f
	mov	$'\r,r0
4:
.endif
	rts	pc

cp:	defnm
defnm:	<boot\r\0>
end:

inod = ..-512.-BSIZE		/ room for inod, buf, stack
addr = inod+ADDROFF		/ first address in inod
buf = inod+INOSIZ
bno = buf+BSIZE
dno = bno+2
/ This defines the ra control structure of (com. area+cmd. and rsp. blocks)
raca = dno+2
rspdsc = raca+8.
cmddsc = rspdsc+4
rsplen = cmddsc+4
rspref = rsplen+4
rspunit = rspref+4
rspop = rspunit+4
rspcnt = rspop+4
rspbuf = rspcnt+4
rsplbn = rspbuf+12.
cmdlen = rsplbn+24.
cmdref = cmdlen+4
cmdunit = cmdref+4
cmdop = cmdunit+4
cmdcnt = cmdop+4
cmdflg = cmdcnt+2
cmdbuf = cmdflg+2
cmdlbn = cmdbuf+12.
names = cmdlbn+24.
