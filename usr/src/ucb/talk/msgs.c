/* $Header: /a/guest/moore/talk/RCS/msgs.c,v 1.4 83/07/06 00:17:22 moore Exp $ */

/* 
 * a package to display what is happening every MSG_INTERVAL seconds
 * if we are slow connecting.
 */

#include "talk.h"
#include <signal.h>
#include <stdio.h>
#include <sys/time.h>

#define MSG_INTERVAL 4
#define LONG_TIME 100000L

char *cur_state;
int cur_line = 0;

#ifdef pdp11
static long undo = LONG_TIME;
#else
static struct itimerval itimer;
static struct timeval wait = { MSG_INTERVAL , 0};
static struct timeval undo = { LONG_TIME, 0};
#endif	pdp11
    

disp_msg()
{
    message(cur_state);
}

start_msgs()
{
    message(cur_state);
    signal(SIGALRM, disp_msg);
#ifdef pdp11
    alarm(MSG_INTERVAL);
#else
    itimer.it_value = itimer.it_interval = wait;
    setitimer(ITIMER_REAL, &itimer, (struct timerval *)0);
#endif pdp11
}

end_msgs()
{
    signal(SIGALRM, SIG_IGN);
#ifndef pdp11
    timerclear(&itimer.it_value);
    timerclear(&itimer.it_interval);
    setitimer(ITIMER_REAL, &itimer, (struct timerval *)0);
#else pdp11
    alarm(0);
#endif pdp11
}
