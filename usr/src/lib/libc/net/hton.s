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
