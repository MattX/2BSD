#
# Steven Schultz - put in the public domain 1996/6/25.
#
#	@(#)Makefile	1.7 (2.11BSD) 2025/11/25
#
CFLAGS=	-O
TAGSFILE=tags

SUBDIR=	acctd comsat ctimed fingerd ftpd getNAME getty identd makekey \
	popper rexecd rlogind rshd talkd tcpd telnetd tftpd

all:	${SUBDIR}

${SUBDIR}: FRC
	cd $@; make ${MFLAGS}

FRC:
	
install:
	-for i in ${SUBDIR}; do \
		(cd $$i; make ${MFLAGS} \
			install); done

tags:
	cd libc; make ${MFLAGS} TAGSFILE=../${TAGSFILE} tags

clean:
	rm -f a.out core *.s *.o
	-for i in ${SUBDIR}; do (cd $$i; make ${MFLAGS} clean); done
