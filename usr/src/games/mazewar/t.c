#include <stdio.h>
#include <signal.h>

main(argc, argv)
    char **argv;
{
    int parent = getpid();

    if (fork() == 0) {			    /* child */
	signal(SIGTSTP, SIG_IGN);
	sleep(15);
	kill(parent, SIGCONT);
	exit(0);
    }
    fprintf(stderr, "stop me now\n");
    sleep(30);
    fprintf(stderr, "parent here again, sleep 60\n");
    sleep(60);
    fprintf(stderr, "parent here final, bye\n");
    exit(0);
}
