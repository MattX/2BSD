/* $Header: talk_ctl.h 1.1 83/03/26 14:36:39 moore Exp $ */

#include "talk.h"
#include "ctl.h"
#include <errno.h>

extern int errno;

#define daemon_addr dmn_addr
extern struct sockaddr_in dmn_addr;
extern struct sockaddr_in ctl_addr;
extern struct sockaddr_in my_addr;
extern struct in_addr my_m_addr;
extern struct in_addr his_m_addr;
#define daemon_port dmn_port
extern u_short dmn_port;
extern int ctl_sockt;
extern CTL_MSG msg;
