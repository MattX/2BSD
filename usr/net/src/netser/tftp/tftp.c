/*	tftp.c	4.2	82/08/17	*/

/*
 * TFTP User Program -- Protocol Machines
 */
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <setjmp.h>
#include "tftp.h"
#ifdef	pdp11
#define	connected	cnctd
#endif

extern	int errno;
extern	struct sockaddr_in sin;
extern	char mode[];
time_t	time();
int	f;
int	trace;
int	verbose;
int	connected;
char	buf2[SEGSIZE+5], buf[SEGSIZE+5];
int	timeout;
jmp_buf	toplevel;
jmp_buf	retryit;

timer()
{
	timeout += TIMEOUT;
	if (timeout >= MAXTIMEOUT) {
		printf("Transfer timed out.\n");
		longjmp(toplevel, -1);
	}
	alarm(TIMEOUT);
}

/*
 * Send the requested file.
 */
sendfile(fd, name)
	int fd;
	char *name;
{
	register struct tftphdr *tp = (struct tftphdr *)buf;
	int block = 0, size, n, amount = 0;
	struct sockaddr_in from;
	time_t start = time(0), delta;
#ifdef	GOO
	struct sockaddr_in svsin;
	FILE *fld;
	register char *ptr1, *ptr2;
	char savechar = '\0';

	svsin.sin_port = sin.sin_port;
	if (strcmp(mode, "netascii") == 0) {
		fld = fdopen(fd, "r");
	}
#endif
	size = makerequest(WRQ, name) - 4;
	timeout = 0;
	sigset(SIGALRM, timer);
	do {
		tp = (struct tftphdr *)buf;
		if (block != 0) {
#ifdef	GOO
			if (strcmp(mode, "netascii") == 0) {
				size = SEGSIZE;
				ptr1 = tp->th_data;
				if (savechar)
					*ptr1++ = savechar;
				while (ptr1 < (tp->th_data+SEGSIZE)) {
					if (fgets(ptr1,(SEGSIZE+1)-(ptr1-
					   tp->th_data),fld)!=NULL)
					   if((ptr2=index(ptr1,'\n'))!=NULL) {
						ptr1 = ptr2;
						*ptr1++ = '\r';
						*ptr1++ = '\n';
						if (ptr1>(tp->th_data+SEGSIZE))
						    savechar = '\n';
						else
						    savechar = '\0';
					   } else {
						ptr1 += SEGSIZE;
					   }
					else {
					   size = ptr1-tp->th_data;
					   ptr1 += SEGSIZE;
					}
				}
			} else
#endif
				size = read(fd, tp->th_data, SEGSIZE);
			if (size < 0) {
				nak(errno + 100);
				break;
			}
			tp->th_opcode = htons((u_short)DATA);
			tp->th_block = htons((u_short)block);
		}
		timeout = 0;
		alarm(TIMEOUT);
rexmt:
		if (trace)
			tpacket("sent", tp, size + 4);
		if (send(f, &sin, buf, size + 4) != size + 4) {
			alarm(0);
			perror("send");
			break;
		}
again:
		n = receive(f, &from, buf2, sizeof (buf2));
		if (n <= 0) {
			if (n == 0)
				goto again;
			if (errno == EINTR)
				goto rexmt;
			alarm(0);
			perror("receive");
			break;
		}
		alarm(0);
#ifdef	GOO
		sin.sin_port = from.sin_port;
		tp = (struct tftphdr *)buf2;
#endif
		if (trace)
			tpacket("received", tp, n);
#if vax || pdp11
		tp->th_opcode = ntohs(tp->th_opcode);
		tp->th_block = ntohs(tp->th_block);
#endif
		if (tp->th_opcode == ERROR) {
			printf("Error code %d: %s\n", tp->th_code,
				tp->th_msg);
			break;
		}
		if (tp->th_opcode != ACK || block != tp->th_block) {
			alarm(TIMEOUT);
			goto again;
		}
		if (block > 0)
			amount += size;
		block++;
	} while (size == SEGSIZE || block == 1);
	alarm(0);
	(void) close(fd);
	if (amount > 0) {
		delta = time(0) - start;
		printf("Sent %d bytes in %D seconds.\n", amount, delta);
	}
#ifdef	GOO
	sin.sin_port = svsin.sin_port;
#endif
}

/*
 * Receive a file.
 */
recvfile(fd, name)
	int fd;
	char *name;
{
	struct tftphdr *tp = (struct tftphdr *)buf;
	int block = 1, n, size, amount = 0;
	struct sockaddr_in from;
	time_t start = time(0), delta;
#ifdef	GOO
	struct sockaddr_in svsin;
	register char *ptr1, *ptr2;
	register int i;
	int sz;

	svsin.sin_port = sin.sin_port;
#endif
	size = makerequest(RRQ, name);
	timeout = 0;
	sigset(SIGALRM, timer);
	alarm(TIMEOUT);
	goto rexmt;
	do {
		timeout = 0;
		alarm(TIMEOUT);
		tp = (struct tftphdr *)buf;
		tp->th_opcode = htons((u_short)ACK);
		tp->th_block = htons((u_short)(block));
		size = 4;
		block++;
rexmt:
		if (trace)
			tpacket("sent", tp, size);
		if (send(f, &sin, buf, size) != size) {
			perror("send");
			break;
		}
again:
		n = receive(f, &from, buf2, sizeof (buf2));
		if (n <= 0) {
			if (n == 0)
				goto again;
			if (errno == EINTR)
				goto rexmt;
			alarm(0);
			perror("receive");
			break;
		}
		alarm(0);
#ifdef	GOO
		sin.sin_port = from.sin_port;
		tp = (struct tftphdr *)buf2;
#endif
		if (trace)
			tpacket("received", tp, n);
#if vax || pdp11
		tp->th_opcode = ntohs(tp->th_opcode);
		tp->th_block = ntohs(tp->th_block);
#endif
		if (tp->th_opcode == ERROR) {
			printf("Error code %d: %s\n", tp->th_code,
				tp->th_msg);
			break;
		}
		if (tp->th_opcode != DATA || block != tp->th_block) {
			alarm(TIMEOUT);
			goto again;
		}
#ifdef	GOO
		if (strcmp(mode, "netascii") == 0) {
			sz = 0;
			ptr1 = ptr2 = tp->th_data;
			for (i = 0; i < (n-4); i++, ptr1++)
				if (*ptr1 != '\0' && *ptr1 != '\r') {
					*ptr2++ = *ptr1;
					sz++;
				}
		} else
#endif
			sz = (n > 4)?(n-4):0;
		sz = write(fd, tp->th_data, sz);
		size = n-4;
		if (sz < 0) {
			nak(errno + 100);
			break;
		}
		amount += sz;
	} while (size == SEGSIZE);
	alarm(0);
	tp = (struct tftphdr *)buf;
	tp->th_opcode = htons((u_short)ACK);
	tp->th_block = htons((u_short)block);
	(void) send(f, &sin, buf, 4);
	(void) close(fd);
	if (amount > 0) {
		delta = time(0) - start;
		printf("Received %d bytes in %D seconds.\n", amount, delta);
	}
#ifdef	GOO
	sin.sin_port = svsin.sin_port;
#endif
}

makerequest(request, name)
	int request;
	char *name;
{
	register struct tftphdr *tp;
	int size;
	register char *cp;

	tp = (struct tftphdr *)buf;
	tp->th_opcode = htons((u_short)request);
	strcpy(tp->th_stuff, name);
	size = strlen(name);
	cp = tp->th_stuff + strlen(name);
	*cp++ = '\0';
	strcpy(cp, mode);
	cp += sizeof ("netascii") - 1;
	*cp++ = '\0';
	return (cp - buf);
}

struct errmsg {
	int	e_code;
	char	*e_msg;
} errmsgs[] = {
	{ EUNDEF,	"Undefined error code" },
	{ ENOTFOUND,	"File not found" },
	{ EACCESS,	"Access violation" },
	{ ENOSPACE,	"Disk full or allocation exceeded" },
	{ EBADOP,	"Illegal TFTP operation" },
	{ EBADID,	"Unknown transfer ID" },
	{ EEXISTS,	"File already exists" },
	{ ENOUSER,	"No such user" },
	{ -1,		0 }
};

/*
 * Send a nak packet (error message).
 * Error code passed in is one of the
 * standard TFTP codes, or a UNIX errno
 * offset by 100.
 */
nak(error)
	int error;
{
	register struct tftphdr *tp;
	int length;
	register struct errmsg *pe;
	extern char *sys_errlist[];

	tp = (struct tftphdr *)buf;
	tp->th_opcode = htons((u_short)ERROR);
	tp->th_code = htons((u_short)error);
	for (pe = errmsgs; pe->e_code >= 0; pe++)
		if (pe->e_code == error)
			break;
	if (pe->e_code < 0)
		pe->e_msg = sys_errlist[error - 100];
	strcpy(tp->th_msg, pe->e_msg);
	length = strlen(pe->e_msg) + 4;
	if (trace)
		tpacket("sent", tp, length);
	if (send(f, &sin, buf, length) != length)
		perror("nak");
}

tpacket(s, tp, n)
	struct tftphdr *tp;
	int n;
{
	static char *opcodes[] =
	   { "#0", "RRQ", "WRQ", "DATA", "ACK", "ERROR" };
	register char *cp, *file;
	u_short op = ntohs(tp->th_opcode);
	char *index();

	if (op < RRQ || op > ERROR)
		printf("%s opcode=%x ", s, op);
	else
		printf("%s %s ", s, opcodes[op]);
	switch (op) {

	case RRQ:
	case WRQ:
		n -= 2;
		file = cp = tp->th_stuff;
		cp = index(cp, '\0');
		printf("<file=%s, mode=%s>\n", file, cp + 1);
		break;

	case DATA:
		printf("<block=%d, %d bytes>\n", ntohs(tp->th_block), n - 4);
		break;

	case ACK:
		printf("<block=%d>\n", ntohs(tp->th_block));
		break;

	case ERROR:
		printf("<code=%d, msg=%s>\n", ntohs(tp->th_code), tp->th_msg);
		break;
	}
}
