/* $Header: ctl.c 1.4 83/06/09 23:37:01 moore Exp $ */

/* This file handles haggling with the various talk daemons to
   get a socket to talk to. sockt is opened and connected in
   the progress
 */

#include "talk_ctl.h"

struct sockaddr_in daemon_addr = { AF_INET };
struct sockaddr_in ctl_addr = { AF_INET };
struct sockaddr_in my_addr = { AF_INET };

    /* inet addresses of the two machines */
struct in_addr my_m_addr;
struct in_addr his_m_addr;

u_short daemon_port;	/* port number of the talk daemon */

int ctl_sockt;
int sockt;
int invitation_waiting = 0;

CTL_MSG msg;

open_sockt(sockopts)
int sockopts;
{
    int length;

    my_addr.sin_addr = my_m_addr;
    my_addr.sin_port = 0;

#ifdef	pdp11
    sockt = socket(SOCK_STREAM, 0, &my_addr, sockopts);
    if (sockt <= 0) {
	    p_error("Bad socket (1)");
    }
#else
    sockt = socket(AF_INET, SOCK_STREAM, 0, 0);

    if (sockt <= 0) {
	p_error("Bad socket");
    }

    if ( bind(sockt, &my_addr, sizeof(my_addr)) != 0) {
	p_error("Binding local socket");
    }
#endif	pdp11

    length = sizeof(my_addr);

    if (getsockname(sockt, &my_addr, &length) == -1) {
	p_error("Bad address for socket");
    }
}

    /* open the ctl socket */

open_ctl() 
{
    int length;

    ctl_addr.sin_port = 0;
    ctl_addr.sin_addr = my_m_addr;

#ifdef	pdp11
    ctl_sockt = socket(SOCK_DGRAM, 0, &ctl_addr, 0);
    if (ctl_sockt <= 0) {
	    p_error("Bad socket (2)");
    }
#else
    ctl_sockt = socket(AF_INET, SOCK_DGRAM, 0, 0);

    if (ctl_sockt <= 0) {
	p_error("Bad socket");
    }

    if (bind(ctl_sockt, &ctl_addr, sizeof(ctl_addr), 0) != 0) {
	p_error("Couldn't bind to control socket");
    }
#endif	pdp11

    length = sizeof(ctl_addr);
    if (getsockname(ctl_sockt, &ctl_addr, &length) == -1) {
	p_error("Bad address for ctl socket");
    }
}

#ifdef	pdp11
close_ctl()
{
	close(ctl_sockt);
}
#endif	pdp11

/* p_addr is a debug print routine */

p_addr(addr)
struct sockaddr_in *addr;
{
    int i;

    printf("addr = %x, port = %o, family = %o zero = ",
	    addr->sin_addr, htons((u_short)addr->sin_port), 
	    htons((u_short)addr->sin_family));


    for (i = 0; i<8;i++) {
	printf("%o ", (int)addr->sin_zero[i]);
    }
    putchar('\n');
}

/* p_resp is a debug print routine also */

p_resp(resp)
CTL_RESPONSE *resp;
{
	printf("Response:\n");
	printf("	type = %d\n",resp->type);
	printf("	answer = %d\n",resp->answer);
	printf("	rid = %ld\n",resp->rid_num);
}
