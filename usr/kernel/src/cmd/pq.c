/*
 * PrintQuota - print the quota file affecting you most immediately
 *
 * Kurt Shoens UCB July, 1977
 */
# include	<whoami.h>
# include	<stdio.h>
# include	<sys/types.h>
# include	<sys/inode.h>
# include	<sys/stat.h>
# include	<sys/lstat.h>

# define	reg	register

# define	isquot(i)	((i.ls_mode & S_IFMT) == S_IFQUOT)

struct lstat	Sbuf;

main(ct,av)
reg int		ct;
char		**av;
{
	reg int 	i;
	reg char	*sp;
	int		lastnode;
	char		buf[80];

	if (ct==1)
	{
		lastnode=0;
		lstat(".",&Sbuf);
		while (lastnode != Sbuf.ls_number)
		{
			lastnode = Sbuf.ls_number;
			lstat(".q",&Sbuf);
			if (isquot(Sbuf))
			{
				PrintQuota(&Sbuf);
				exit(0);
			}
			chdir("..");
			stat(".",&Sbuf);
		}
		exit(9);
	}
	for (i = 1; i < ct; ++i) {
		strcpy(buf, av[i]);
		if ((sp = rindex(buf, '.')) == NULL || strcmp(sp, ".q") != 0)
			strcat(buf, "/.q");
		lstat(buf,&Sbuf);
		if (isquot(Sbuf))
		{
			PrintQuota(&Sbuf);
			exit(0);
		}
		else
			printf("no quota file: %s\n", av[i]);
	}
}

PrintQuota()
{
	float	cut, a, b;

	a = Sbuf.ls_un.ls_qused;
	b = Sbuf.ls_un.ls_qmax;
	cut = 100.0 * a / b;
	printf("%ld/%ld (%1.0f%%)\n", Sbuf.ls_un.ls_qused, Sbuf.ls_un.ls_qmax, cut);
	return;
}
