.globl	cerror
.globl  _select
_select:
	mov	r5,-(sp)
	mov	sp,r5
	mov     4.(r5),0f
	mov     6.(r5),0f+2
	mov     8.(r5),0f+4
	mov     10.(r5),0f+6
	mov     12.(r5),0f+8.
	clr     r0
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys     select; 0:.. ; .. ; .. ; .. ; ..
.text

.globl  _gethost
_gethost:
	mov	r5,-(sp)
	mov	sp,r5
	mov     4.(r5),0f
	mov     6.(r5),0f+2
	clr     r0
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys     gethost; 0:.. ; ..
.text

.globl  _sethost
_sethost:
	mov	r5,-(sp)
	mov	sp,r5
	mov     4.(r5),0f
	mov     6.(r5),0f+2
	clr     r0
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys     sethost; 0:.. ; ..
.text

.globl  _socket
_socket:
	mov	r5,-(sp)
	mov	sp,r5
	mov     4.(r5),0f
	mov     6.(r5),0f+2
	mov     8.(r5),0f+4
	mov     10.(r5),0f+6
	clr     r0
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys     socket; 0:.. ; .. ; .. ; ..
.text

.globl  _connect
_connect:
	mov	r5,-(sp)
	mov	sp,r5
	mov     4.(r5),0f
	mov     6.(r5),0f+2
	clr     r0
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys     connect; 0:.. ; ..
.text

.globl  _accept
_accept:
	mov	r5,-(sp)
	mov	sp,r5
	mov     4.(r5),0f
	mov     6.(r5),0f+2
	clr     r0
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys     accept; 0:.. ; ..
.text

.globl  _send
_send:
	mov	r5,-(sp)
	mov	sp,r5
	mov     4.(r5),0f
	mov     6.(r5),0f+2
	mov     8.(r5),0f+4
	mov     10.(r5),0f+6
	clr     r0
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys     send; 0:.. ; .. ; .. ; ..
.text

.globl  _receive
_receive:
	mov	r5,-(sp)
	mov	sp,r5
	mov     4.(r5),0f
	mov     6.(r5),0f+2
	mov     8.(r5),0f+4
	mov     10.(r5),0f+6
	clr     r0
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys     receive; 0:.. ; .. ; .. ; ..
.text

.globl  _socketa
_socketa:
	mov	r5,-(sp)
	mov	sp,r5
	mov     4.(r5),0f
	mov     6.(r5),0f+2
	clr     r0
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys     socketa; 0:.. ; ..
.text

.globl  _vhangup
_vhangup:
	mov	r5,-(sp)
	mov	sp,r5
	clr     r0
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys     vhangup
.text

.globl  _htonl,_htons,_ntohl,_ntohs
_htonl:
_ntohl:
	mov     2(sp),r0
	mov     4(sp),r1
	swab    r0
	swab    r1
	rts     pc

_htons:
_ntohs:
	mov     2(sp),r0
	swab    r0
	rts     pc

/ error = gethostid();
/ should be hostid instead of hstid, but ...

.globl  _gethstid
_gethstid:
	mov	r5,-(sp)
	mov	sp,r5
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys	gethstid
.text
/ should be hostid instead of hstid, but ...
.globl  _sethstid
_sethstid:
	mov	r5,-(sp)
	mov	sp,r5
	mov     4.(r5),0f
	mov     6.(r5),0f+2
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	clr	r0
	rts	pc
.data
9:
	sys	sethstid; 0:..; ..
.text
/ error = setrgid (rgid)
/	  int rgid;
/
/ error = setegid (egid)
/	  int egid;
/
/ error = setregid(rgid, egid);
/	  int rgid, egid;

.globl	_setrgid
.globl	_setegid
.globl	_setregid
.globl	cerror

_setrgid:
	mov	r5,-(sp)
	mov	sp,r5
	mov	4(r5),r0
	mov	$-1,r1
	br	0f

_setegid:
	mov	r5,-(sp)
	mov	sp,r5
	mov	$-1,r0
	mov	4(r5),r1
	br	0f

_setregid:
	mov	r5,-(sp)
	mov	sp,r5
	mov	4(r5),r0
	mov	6(r5),r1
0:
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	clr	r0
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys	setregid
.text
/ error = setruid (ruid)
/	  int ruid;
/
/ error = seteuid (euid)
/	  int euid;
/
/ error = setreuid(ruid, euid);
/	  int ruid, euid;

.globl	_setruid
.globl	_seteuid
.globl	_setreuid
.globl	cerror

_setruid:
	mov	r5,-(sp)
	mov	sp,r5
	mov	4(r5),r0
	mov	$-1,r1
	br	0f

_seteuid:
	mov	r5,-(sp)
	mov	sp,r5
	mov	$-1,r0
	mov	4(r5),r1
	br	0f

_setreuid:
	mov	r5,-(sp)
	mov	sp,r5
	mov	4(r5),r0
	mov	6(r5),r1
0:
	sys	local; 9f
	bec	1f
	jmp	cerror
1:
	clr	r0
	mov	(sp)+,r5
	rts	pc
.data
9:
	sys	setreuid
.text
