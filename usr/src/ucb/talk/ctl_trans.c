#include "talk_ctl.h"
#include <sys/time.h>

#define CTL_WAIT 2	/* the amount of time to wait for a 
			   response, in seconds */


    /*
     * SOCKDGRAM is unreliable, so we must repeat messages if we have
     * not recieved an acknowledgement within a reasonable amount
     * of time
     */

ctl_transact(target, msg, type, response)
struct in_addr *target;
CTL_MSG *msg;
int type;
CTL_RESPONSE *response;
{
    struct sockaddr junk;
    long read_mask;
    long ctl_mask;
    int nready;
    int cc;
    int junk_size;
#ifndef	pdp11
    struct timeval wait;
    wait.tv_sec = CTL_WAIT;
    wait.tv_usec = 0;
#endif	pdp11

    msg->type = type;

    daemon_addr.sin_addr = *target;
    daemon_addr.sin_port = daemon_port;

    ctl_mask = (long)(1 << ctl_sockt);

	/*
	 * keep sending the message until a response of the right
	 * type is obtained
	 */
    do {
	    /* keep sending the message until a response is obtained */

	do {
	    cc = sendto(ctl_sockt, (char *)msg, sizeof(CTL_MSG), 0,
			&daemon_addr, sizeof(daemon_addr));

#ifdef	pdp11
	    if (cc < 0) 
#else
	    if (cc != sizeof(CTL_MSG)) 
#endif pdp11
	    {
		if (errno == EINTR) {
			/* we are returning from an interupt */
		    continue;
		} else {
		    p_error("Error on write to talk daemon");
		}
	    }

	    read_mask = ctl_mask;

#ifdef	pdp11
	    while ((nready = select(20, &read_mask, 0, CTL_WAIT*1000L)) < 0)
#else
	    while ((nready = select(32, &read_mask, 0, 0, &wait)) < 0)
#endif	pdp11
	    {
		if (errno == EINTR) {
			/* we are returning from an interupt */
		    continue;
		} else {
		    p_error("Error on waiting for response from daemon");
		}
	    }
	} while (nready == 0);

	    /* keep reading while there are queued messages 
	       (this is not necessary, it just saves extra
	       request/acknowledgements being sent)
	     */

	do {

	    junk_size = sizeof(junk);
	    cc = recvfrom(ctl_sockt, (char *) response,
			   sizeof(CTL_RESPONSE), 0, &junk, &junk_size );
	    if (cc < 0) {
		if (errno == EINTR) {
		    continue;
		}
		p_error("Error on read from talk daemon");
	    }

	    read_mask = ctl_mask;

		/* an immediate poll */

#ifndef	pdp11
	    timerclear(&wait);
	    nready = select(32, &read_mask, 0, 0, &wait);
#else
	    alarm(0);
	    nready = select(32, &read_mask, 0L, CTL_WAIT*1000L);
#endif	pdp11

	} while ( nready > 0 && response->type != type);

    } while (response->type != type);
}
