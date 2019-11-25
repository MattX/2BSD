.comm	_itolws,4
.comm	_adbtty,6
.comm	_usrtty,6
.comm	_erradb,24
.comm	_NOFORK,0
.comm	_ENDPCS,0
.comm	_BADWAIT,0
.comm	_lp,2
.comm	_sigint,2
.comm	_sigqit,2
.comm	_bkpthead,2
.comm	_reglist,0
.comm	_lastc,2
.comm	_corhdr,0
.comm	_uar0,2
.comm	_overlay,2
.comm	_curov,2
.comm	_fcor,2
.comm	_fsym,2
.comm	_errflg,2
.comm	_signo,2
.comm	_dot,4
.comm	_symfil,2
.comm	_wtflag,2
.comm	_pid,2
.comm	_expv,4
.comm	_adrflg,2
.comm	_loopcnt,4
.comm	_var,0
.globl	_userpc
.data
_userpc:
1
.globl	_getsig
.text
_getsig:
~~getsig:
jsr	r5,csv
~sig=4
clr	(sp)
jsr	pc,*$_expr
tst	r0
jeq	L10000
mov	2+_expv,r0
L3:jmp	cret
L10000:mov	4(r5),r0
jbr	L3
.globl	_runpcs
_runpcs:
~~runpcs:
jsr	r5,csv
~runmode=4
~execsig=6
tst	-(sp)
~rc=177766
~bkpt=r4
tst	_adrflg
jeq	L7
mov	2+_dot,_userpc
L7:tst	_overlay
jeq	L8
mov	464+_corhdr,(sp)
jsr	pc,*$_choverlay
L8:mov	_symfil,(sp)
mov	$L9,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L10
L20001:jgt	L10002
tst	r1
jeq	L11
L10002:mov	$_usrtty,(sp)
clr	-(sp)
jsr	pc,*$_stty
tst	(sp)+
mov	6(r5),(sp)
mov	_userpc,-(sp)
mov	_pid,-(sp)
mov	4(r5),-(sp)
jsr	pc,*$_ptrace
add	$6,sp
jsr	pc,_bpwait
jsr	pc,_chkerr
jsr	pc,_readregs
tst	_signo
jne	L12
mov	_uar0,r0
mov	2(r0),r0
add	$-2,r0
mov	r0,(sp)
jsr	pc,*$_scanbkpt
mov	r0,r4
jeq	L12
mov	_uar0,r0
mov	(r4),2(r0)
mov	2(r0),r0
mov	r0,_userpc
cmpb	$2,10(r4)
jeq	L10003
movb	$2,10(r4)
mov	$72,(sp)
mov	r4,-(sp)
add	$12,(sp)
jsr	pc,*$_command
mov	r0,(sp)+
jeq	L13
dec	4(r4)
jeq	L13
L10003:mov	r4,(sp)
jsr	pc,*$_execbkpt
clr	6(r5)
add	$1,2+_loopcnt
adc	_loopcnt
jbr	L20000
L13:mov	6(r4),4(r4)
mov	$1,-12(r5)
jbr	L10
L12:clr	-12(r5)
mov	_signo,6(r5)
L20000:mov	$1,_userpc
L10:mov	2+_loopcnt,r1
mov	_loopcnt,r0
sub	$1,2+_loopcnt
sbc	_loopcnt
tst	r0
jge	L20001
L11:mov	-12(r5),r0
jmp	cret
.globl	_endpcs
_endpcs:
~~endpcs:
jsr	r5,csv
~bkptr=r4
tst	_pid
jeq	L18
clr	(sp)
clr	-(sp)
mov	_pid,-(sp)
mov	$10,-(sp)
jsr	pc,*$_ptrace
add	$6,sp
clr	_pid
mov	$1,_userpc
mov	_bkpthead,r4
jbr	L22
L23:tstb	10(r4)
jeq	L20
movb	$1,10(r4)
L20:mov	112(r4),r4
L22:tst	r4
jne	L23
L18:jmp	cret
.globl	_setup
_setup:
~~setup:
jsr	r5,csv
mov	_fsym,(sp)
jsr	pc,*$_close
mov	$-1,_fsym
jsr	pc,_fork
mov	r0,_pid
jne	L29
clr	(sp)
clr	-(sp)
clr	-(sp)
clr	-(sp)
jsr	pc,*$_ptrace
add	$6,sp
mov	_sigint,(sp)
mov	$2,-(sp)
jsr	pc,*$_signal
tst	(sp)+
mov	_sigqit,(sp)
mov	$3,-(sp)
jsr	pc,*$_signal
tst	(sp)+
jsr	pc,_doexec
clr	(sp)
jsr	pc,*$_exit
jbr	L28
L29:cmp	$-1,_pid
jne	L31
mov	$_NOFORK,(sp)
jbr	L20002
L31:jsr	pc,_bpwait
jsr	pc,_readregs
movb	$12,*_lp
mov	_lp,r0
clrb	1(r0)
mov	_wtflag,(sp)
mov	_symfil,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fsym
tst	_errflg
jeq	L28
mov	_symfil,(sp)
mov	$L34,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_endpcs
clr	(sp)
L20002:jsr	pc,*$_error
L28:jmp	cret
.globl	_execbkpt
_execbkpt:
~~execbkpt:
jsr	r5,csv
~bkptr=4
tst	-(sp)
~bkptloc=177766
mov	*4(r5),-12(r5)
mov	4(r5),r0
mov	2(r0),(sp)
mov	-12(r5),-(sp)
mov	_pid,-(sp)
mov	$4,-(sp)
jsr	pc,*$_ptrace
add	$6,sp
mov	$_usrtty,(sp)
clr	-(sp)
jsr	pc,*$_stty
tst	(sp)+
clr	(sp)
mov	-12(r5),-(sp)
mov	_pid,-(sp)
mov	$11,-(sp)
jsr	pc,*$_ptrace
add	$6,sp
jsr	pc,_bpwait
jsr	pc,_chkerr
mov	$3,(sp)
mov	-12(r5),-(sp)
mov	_pid,-(sp)
mov	$4,-(sp)
jsr	pc,*$_ptrace
add	$6,sp
mov	4(r5),r0
movb	$1,10(r0)
jmp	cret
.globl	_doexec
_doexec:
~~doexec:
jsr	r5,csv
sub	$1106,sp
~argl=177670
~args=176670
~p=176666
~ap=176664
~filnam=176662
mov	r5,r0
add	$-110,r0
mov	r0,-1114(r5)
mov	r5,r0
add	$-1110,r0
mov	r0,-1112(r5)
mov	_symfil,*-1114(r5)
add	$2,-1114(r5)
L43:jsr	pc,_rdc
cmp	$12,r0
jeq	L42
mov	-1112(r5),*-1114(r5)
jbr	L45
L20004:cmpb	$40,_lastc
jeq	L46
cmpb	$11,_lastc
jeq	L46
movb	_lastc,*-1112(r5)
inc	-1112(r5)
jsr	pc,_readchar
L45:cmpb	$12,_lastc
jne	L20004
L46:clrb	*-1112(r5)
inc	-1112(r5)
mov	*-1114(r5),r0
inc	r0
mov	r0,-1116(r5)
mov	*-1114(r5),r0
cmpb	$74,(r0)
jne	L47
clr	(sp)
jsr	pc,*$_close
clr	(sp)
mov	-1116(r5),-(sp)
jsr	pc,*$_open
mov	r0,(sp)+
jpl	L48
mov	-1116(r5),(sp)
mov	$L49,-(sp)
L20008:jsr	pc,*$_printf
tst	(sp)+
clr	(sp)
jsr	pc,*$_exit
L48:mov	*-1114(r5),-1112(r5)
jbr	L41
L20010:mov	$1,(sp)
jsr	pc,*$_close
mov	$666,(sp)
mov	$1001,-(sp)
mov	-1116(r5),-(sp)
jsr	pc,*$_open
cmp	(sp)+,(sp)+
tst	r0
jpl	L48
mov	-1116(r5),(sp)
mov	$L53,-(sp)
jbr	L20008
L47:mov	*-1114(r5),r0
cmpb	$76,(r0)
jeq	L20010
add	$2,-1114(r5)
L41:cmpb	$12,_lastc
jne	L43
L42:clr	*-1114(r5)
add	$2,-1114(r5)
mov	r5,(sp)
add	$-110,(sp)
mov	_symfil,-(sp)
jsr	pc,*$_execv
tst	(sp)+
jmp	cret
.globl	_scanbkpt
_scanbkpt:
~~scanbkpt:
jsr	r5,csv
~adr=4
~bkptr=r4
mov	_bkpthead,r4
jbr	L60
L61:tstb	10(r4)
jeq	L58
cmp	4(r5),(r4)
jne	L58
tstb	11(r4)
jeq	L59
cmpb	_curov,11(r4)
jeq	L59
L58:mov	112(r4),r4
L60:tst	r4
jne	L61
L59:mov	r4,r0
jmp	cret
.globl	_delbp
_delbp:
~~delbp:
jsr	r5,csv
~bkptr=r4
mov	_bkpthead,r4
jbr	L69
L70:tstb	10(r4)
jeq	L67
mov	r4,(sp)
jsr	pc,*$_del1bp
L67:mov	112(r4),r4
L69:tst	r4
jne	L70
jmp	cret
.globl	_del1bp
_del1bp:
~~del1bp:
jsr	r5,csv
~bkptr=4
mov	4(r5),r0
tstb	11(r0)
jeq	L76
movb	11(r0),r0
mov	r0,(sp)
jsr	pc,*$_choverlay
L76:mov	4(r5),r0
mov	2(r0),(sp)
mov	*4(r5),-(sp)
mov	_pid,-(sp)
mov	$4,-(sp)
jsr	pc,*$_ptrace
add	$6,sp
jmp	cret
.globl	_choverlay
_choverlay:
~~choverlay:
jsr	r5,csv
~ovno=4
clr	_errno
tst	_overlay
jeq	L80
tst	_pid
jeq	L80
tstb	4(r5)
jle	L80
cmpb	$17,4(r5)
jlt	L80
movb	4(r5),r0
mov	r0,(sp)
mov	$464,-(sp)
mov	_pid,-(sp)
mov	$6,-(sp)
jsr	pc,*$_ptrace
add	$6,sp
L80:tst	_errno
jeq	L79
movb	4(r5),r0
mov	r0,(sp)
mov	$L82,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L79:jmp	cret
.globl	_setbp
_setbp:
~~setbp:
jsr	r5,csv
~bkptr=r4
mov	_bkpthead,r4
jbr	L88
L89:tstb	10(r4)
jeq	L86
mov	r4,(sp)
jsr	pc,*$_set1bp
L86:mov	112(r4),r4
L88:tst	r4
jne	L89
jmp	cret
.globl	_set1bp
_set1bp:
~~set1bp:
jsr	r5,csv
~bkptr=4
~a=r4
mov	*4(r5),r4
mov	4(r5),r0
tstb	11(r0)
jeq	L95
movb	11(r0),r0
mov	r0,(sp)
jsr	pc,*$_choverlay
L95:clr	(sp)
mov	r4,-(sp)
mov	_pid,-(sp)
mov	$1,-(sp)
jsr	pc,*$_ptrace
add	$6,sp
mov	4(r5),r1
mov	r0,2(r1)
mov	$3,(sp)
mov	r4,-(sp)
mov	_pid,-(sp)
mov	$4,-(sp)
jsr	pc,*$_ptrace
add	$6,sp
tst	_errno
jeq	L94
mov	$L97,(sp)
jsr	pc,*$_printf
mov	$L98,(sp)
mov	$2,-(sp)
mov	*4(r5),-(sp)
clr	-(sp)
jsr	pc,*$_psymoff
add	$6,sp
L94:jmp	cret
.globl	_bpwait
_bpwait:
~~bpwait:
jsr	r5,csv
tst	-(sp)
~w=r4
~stat=177766
mov	$1,(sp)
mov	$2,-(sp)
jsr	pc,*$_signal
tst	(sp)+
L102:mov	r5,(sp)
add	$-12,(sp)
jsr	pc,*$_wait
mov	r0,r4
cmp	_pid,r4
jeq	L103
cmp	$-1,r4
jne	L102
L103:mov	_sigint,(sp)
mov	$2,-(sp)
jsr	pc,*$_signal
tst	(sp)+
mov	$_usrtty,(sp)
clr	-(sp)
jsr	pc,*$_gtty
tst	(sp)+
mov	$_adbtty,(sp)
clr	-(sp)
jsr	pc,*$_stty
tst	(sp)+
cmp	$-1,r4
jne	L104
clr	_pid
mov	$_BADWAIT,_errflg
jbr	L101
L104:mov	-12(r5),r0
bic	$-200,r0
cmp	$177,r0
jeq	L106
mov	-12(r5),r0
bic	$-200,r0
mov	r0,_signo
jeq	L107
jsr	pc,_sigprint
L107:bit	$200,-12(r5)
jeq	L108
mov	$L109,(sp)
jsr	pc,*$_printf
mov	_fcor,(sp)
jsr	pc,*$_close
jsr	pc,_setcor
L108:clr	_pid
mov	$_ENDPCS,_errflg
jbr	L101
L106:mov	-12(r5),r0
ash	$-10,r0
mov	r0,_signo
cmp	$5,r0
jeq	L111
jsr	pc,_sigprint
jbr	L112
L111:clr	_signo
L112:jsr	pc,_flushbuf
L101:jmp	cret
.globl	_readregs
_readregs:
~~readregs:
jsr	r5,csv
tst	-(sp)
~i=r4
~ovno=177766
clr	r4
L119:clr	(sp)
mov	r4,r0
ash	$2,r0
mov	2+_reglist(r0),r0
asl	r0
add	_uar0,r0
sub	$_corhdr,r0
mov	r0,-(sp)
mov	_pid,-(sp)
mov	$3,-(sp)
jsr	pc,*$_ptrace
add	$6,sp
mov	r4,r1
ash	$2,r1
mov	2+_reglist(r1),r1
asl	r1
add	_uar0,r1
mov	r0,(r1)
inc	r4
cmp	$11,r4
jgt	L119
tst	_overlay
jeq	L121
clr	(sp)
mov	$464,-(sp)
mov	_pid,-(sp)
mov	$3,-(sp)
jsr	pc,*$_ptrace
add	$6,sp
movb	r0,-12(r5)
movb	-12(r5),r0
mov	r0,62+_var
sxt	60+_var
movb	-12(r5),r0
mov	r0,464+_corhdr
movb	-12(r5),r0
mov	r0,(sp)
jsr	pc,*$_setovmap
L121:mov	$2,r4
L125:clr	(sp)
mov	r4,-(sp)
mov	_pid,-(sp)
mov	$3,-(sp)
jsr	pc,*$_ptrace
add	$6,sp
mov	r4,r1
asl	r1
mov	r0,_corhdr(r1)
inc	r4
cmp	$33,r4
jgt	L125
jmp	cret
.globl
.data
L9:.byte 45,163,72,40,162,165,156,156,151,156,147,12,0
L34:.byte 45,163,72,40,143,141,156,156,157,164,40,145,170,145
.byte 143,165,164,145,12,0
L49:.byte 45,163,72,40,143,141,156,156,157,164,40,157,160,145
.byte 156,12,0
L53:.byte 45,163,72,40,143,141,156,156,157,164,40,143,162,145
.byte 141,164,145,12,0
L82:.byte 143,141,156,156,157,164,40,143,150,141,156,147,145,40
.byte 164,157,40,157,166,145,162,154,141,171,40,45,144,12,0
L97:.byte 143,141,156,156,157,164,40,163,145,164,40,142,162,145
.byte 141,153,160,157,151,156,164,72,40,0
L98:.byte 12,0
L109:.byte 40,55,40,143,157,162,145,40,144,165,155,160,145,144,0
