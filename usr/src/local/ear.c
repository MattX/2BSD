/*
 * all-purpose server daemon
 *	listens on socket argv[1] decimal
 *	forks up file in argv[2]
 *	argv[0] of child is argv[3]
 *	argv[1] of child is long octal host
 *	argv[2] of child is argv[4] of ear, etc.
 */

#include <stdio.h>
#include <sys/param.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct sockaddr_in ctrl_addr = { AF_INET };
struct sockaddr_in his_addr;

char us[60], *mkhost();

main (argc, argv)
char **argv;
{
	register  netfid;
	char them[20];
	char *getuc ();

	if (argc < 4) {
		fprintf(stderr, "usage: %s socket file args\n", argv[0]);
		exit(1);
	}
	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	dup2(1, 2);
	ctrl_addr.sin_port = atoi(argv[1]);
#if vax || pdp11
	ctrl_addr.sin_port = htons(ctrl_addr.sin_port);
#endif
	while (1)
	{
		while ((netfid = socket (SOCK_STREAM, 0, &ctrl_addr,
		    SO_ACCEPTCONN)) < 0 || accept(netfid, &his_addr) < 0) {
			perror("ear: ");
			fprintf(stderr, "\r\n");
			close(netfid);
			sleep (5);
		}
		if (spawn () == 0)
		{
			sprintf(them, "0%lo", ntohl(his_addr.sin_addr.s_addr));
			argv[1] = argv[2];
			argv[2] = argv[3];
			argv[3] = them;
			dup2 (netfid, 0);
			dup2 (netfid, 1);
			close (netfid);
			execv (argv[1], &argv[2]);
			perror(argv[1]);
			exit (1);
		}
		close (netfid);
	}
}

/*
 * Uppercase a string in place. Return pointer to
 * null at end.
 */
char *
getuc(s)
    char *s;
{
    register char *p,
		  c;
    for (p = s; c = *p; p++)
    {
	if (c <= 'z' && c >= 'a')
	    *p -= ('a' - 'A');
    }
    return(s);
}

spawn()		/* spawn a grandchild ALK dec 82 (must have been in BBN lib)*/
{	register pid;

	while ((pid = fork()) < 0) sleep(3);
	if (pid) {
		wait(0);	/* wait for child, to clear zombie */
		return(1);	/* parent returns non-zero */
	}
	while ((pid = fork()) < 0) sleep(3);
	if (pid) exit(0);	/* child dies */
	return(0);		/* grandchild returns zero */
}

char *
mkhost(addr)
long addr;
{	static char buf[100];

	sprintf(buf, "%d.%d.%d.%d", (int)(addr>>24), (int)((addr>>16)&0377),
		(int)((addr>>8)&0377), (int)(addr&0377));
	return(buf);
}
