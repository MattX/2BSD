/*
	A quickie program that snarfs the arp table from the
	kernel and prints it out in a decent format.
*/

#include <a.out.h>
#include <sys/types.h>
#include <netinet/in.h>

#define	ARPTAB_SIZE (19*5)

struct	arptab {
	struct in_addr at_iaddr;
	u_char at_enaddr[6];
	struct mbuf *at_hold;
	u_char at_timer;
	u_char at_flags;
} arptab[ARPTAB_SIZE];

struct nlist nl[] = {
#define	N_ARPTAB	0
	{ "_arptab"},
	0,
};

main()
{
 	register i;
	register kfd;

	nlist("/unix", nl);
	if (nl[0].n_type == 0) {
		printf("no namelist\n");
		exit(1);
	}
	kfd = open("/dev/kmem", 0);
	if (kfd < 0) {
		printf("can not open kmem\n");
		exit(1);
	}
	lseek(kfd, (off_t)nl[N_ARPTAB].n_value, 0);
	read(kfd, arptab, sizeof arptab);
	close(kfd);
	for(i=0; i < ARPTAB_SIZE; i++)
	  if (arptab[i].at_flags)
	    printf("%d: internet %u.%u.%u.%u ethernet %u.%u.%u.%u.%u.%u flags 0%o\n",
	    	i+1,
		(unsigned)((arptab[i].at_iaddr.s_addr>>24)&0377),
		(unsigned)((arptab[i].at_iaddr.s_addr>>16)&0377),
		(unsigned)((arptab[i].at_iaddr.s_addr>>8)&0377),
		(unsigned)(arptab[i].at_iaddr.s_addr&0377),
		(unsigned) arptab[i].at_enaddr[0],
		(unsigned) arptab[i].at_enaddr[1],
		(unsigned) arptab[i].at_enaddr[2],
		(unsigned) arptab[i].at_enaddr[3],
		(unsigned) arptab[i].at_enaddr[4],
		(unsigned) arptab[i].at_enaddr[5],
		(unsigned) arptab[i].at_flags);
}
