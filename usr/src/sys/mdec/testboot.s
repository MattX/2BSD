.. = 140000

	240			/ These two lines must be present or DEC
	br	start		/ boot ROMs will refuse to run boot block!
start:
	mov	$..,sp
	mov	sp,r1
	clr	r0
1:
	mov	(r0)+,(r1)+
	cmp	r1,$end
	blo	1b
	jmp	*$restart

restart:
	mov $'A, r0
	jsr pc, putc
	mov $'>, r0
	jsr pc, putc
	jsr pc,getc
	mov r0,r1
	jsr pc,getc

	cmp r1,$'d
	bne 9f

dump:	jsr pc,gword
	mov r0,r1
	jsr pc,gword
	mov r0,r2

1:	mov $'\n,r0
	jsr pc,putc
	mov r1,r0
	jsr pc,pword
	mov $':,r0
	jsr pc,putc
	mov $40,r0
	jsr pc,putc
2:
	mov (r1)+,r0
	jsr pc,pword
	mov $40,r0
	jsr pc,putc

	dec r2
	beq 3f

	bit r1,$16
	beq 1b
	br 2b
	
3:	mov $'\n,r0
	jsr pc,putc
	br restart

9:	cmp r1,$'m
	bne 9f

modify:
	jsr pc,gword
	mov r0,r1
1:	jsr pc,gword
	mov r0,(r1)+
	bcc 1b
	br 3b

unknown:
9:	mov $'\n,r0
	jsr pc,putc
	mov $'?,r0
	jsr pc,putc
	mov $'\n,r0
	jsr pc,putc
	br	restart

gword:	mov r1,-(sp)
	clr r1
1:	jsr pc,getc
	cmp r0,$40
	ble 2f
	bic $!7,r0
	ash $3,r1
	bis r0,r1
	br 1b
2:	mov r1,r0
	br ex1	

pword:	mov r1,-(sp)
	mov r0,r1
	clr r0
	ashc $1,r0
	jsr pc,pdig1
	jsr pc,pdig
	jsr pc,pdig
	jsr pc,pdig
	jsr pc,pdig
	jsr pc,pdig
ex1:	mov (sp)+,r1
	rts pc

pdig:	ashc $3,r0
pdig1:	mov r0,-(sp)
	bic $!7,r0
	add $'0,r0
	jsr pc,putc
ex0:	mov (sp)+,r0
	rts pc

tks = 177560
tkb = 177562
/ read and echo a teletype character
getc:
	mov	$tks,r0
	inc	(r0)
1:	tstb	(r0)
	bge	1b
	mov	*$tkb,r0
	bic	$!177,r0

tps = 177564
tpb = 177566
/ print a teletype character
putc:
	tstb *$tps
	bge putc
	mov r0,*$tpb
	cmp $'\n,r0
	bne 1f
	mov $'\r,r0
	jsr pc,putc
	mov $'\n,r0
1:	rts pc

end:
