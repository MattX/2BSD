/*
 * Originally written for V7 Unix, later changed to handle 2.9BSD, later
 * still brought up on 2.10BSD.  Pretty close the 4.XBSD except for
 * handling swapped out processes (it doesn't).
 */

#include <sys/param.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/file.h>
#include <stdio.h>
#include <a.out.h>

#define NLIST	"/vmunix"
#define MEM	"/dev/mem"

struct nlist nl[] = {
	{ "_proc" },
#define	X_PROC	0
	{ "_nproc" },
#define	X_NPROC	1
	{ 0 },
};
int	mem, cor, nproc;
struct proc	*pbuf;

main(argc, argv)
	int	argc;
	char	**argv;
{
	register int	pid;
	off_t lseek();
	char *malloc();

	if (argc < 2) {
		printf("Usage: %s pid ...\n", argv[0]);
		exit(1);
	}
	openfiles();
	getkvars();
	(void)lseek(mem, (long)nl[X_NPROC].n_value, L_SET);
	(void)read(mem, &nproc, sizeof nproc);
	pbuf = (struct proc *)calloc(nproc, sizeof (struct proc));
	if (pbuf == NULL) {
		puts("Not enough core");
		exit(1);
	}
	(void)lseek(mem, (long)nl[X_PROC].n_value, L_SET);
	(void)read(mem, pbuf, nproc * sizeof (struct proc));

	while (--argc) {
		if ((pid = atoi(*++argv)) <= 0)
			continue;
		printf("%d: ", pid);
		prcdmp(pid);
	}
	exit(0);
}

prcdmp(pid)
	register int	pid;
{
	register int i;
	register struct proc *pp;
	uid_t uid, getuid();
	char coref[MAXNAMLEN + 1], ubuf[USIZE*64];
	off_t lseek();

	for (i = 0, pp = pbuf;; pp++) {
		if (pp->p_pid == pid)
			break;
		if (i++ == nproc) {
			printf("Process not found.\n");
			return;
		}
	}
	if (pp->p_uid != (uid = getuid()) && uid != 0) {
		printf("Not owner.\n");
		return;
	}
	if ((pp->p_flag & SLOAD) == 0) {
		printf("Swapped out.\n");
		return;
	}
	if (pp->p_stat == SZOMB) {
		printf("Zombie.\n");
		return;
	}
	if (pp->p_flag & SSYS) {
		printf("System process.\n");
		/* i.e. swapper or pagedaemon */
		return;
	}
	if (lseek(mem, (long)pp->p_addr << 6, L_SET) < 0) {
		puts("Bad mem seek");
		return;
	}
	if (read(mem, &ubuf, sizeof ubuf) != sizeof ubuf) {
		puts("Bad mem read");
		return;
	}
	(void)sprintf(coref, "core.%d", pid);
	if ((cor = creat(coref, 0666)) < 0) {
		perror(coref);
		exit(1);
	}
	(void)write(cor, &ubuf, sizeof ubuf);
	(void)lseek(mem, (long)pp->p_daddr << 6, L_SET);
	dump(pp->p_dsize);
	(void)lseek(mem, (long)pp->p_saddr << 6, L_SET);
	dump(pp->p_ssize);
	(void)close(cor);
	printf("%s dumped\n", coref);
}

dump(size)
	register u_int size;
{
	register int blocks, i;
	int bytes;
	char buffer[BUFSIZ];

	size <<= 6;
	blocks = size / BUFSIZ;
	bytes = size % BUFSIZ;

	for (i = 0; i < blocks; i++) {
		(void)read(mem, buffer, BUFSIZ);
		(void)write(cor, buffer, BUFSIZ);
	}
	if (bytes) {
		(void)read(mem, buffer, bytes);
		(void)write(cor, buffer, bytes);
	}
}

openfiles()
{
	mem = open(MEM, 0);
	if (mem < 0) {
		perror(MEM);
		exit(1);
	}
}

getkvars()
{
	nlist(NLIST, nl);
	if (nl[0].n_type == 0) {
		printf("%s: No namelist\n", NLIST);
		exit(1);
	}
}
