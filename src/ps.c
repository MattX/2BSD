char	*sccsid = "@(#)ps.c	2.8";

/*
 *	ps - process status
 *	This is the augmented UCB ps for V7 PDP-11 Unix (9/80).
 *	It is not very portable, using the phys sys call and
 *	knowing the format of an a.out symbol table.
 *	Examine and print certain things about processes
 *	Usage: ps [acgklrt#uvwx] [corefile] [swapfile] [system]
 */

#include <whoami.h>
#include <stdio.h>
#include <pwd.h>
#include <a.out.h>
#include <core.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/tty.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <psout.h>

struct nlist nl[] = {
	{ "_proc" },
	{ "_swapdev" },
	{ "_swplo" },
	{ "" },
};

struct	proc mproc;

struct	user u;

#define INTPPG		(512/sizeof(int))		/* ints per page */
int	chkpid = 0;
int	aflg;	/* -a: all processes, not just mine */
int	cflg;	/* -c: not complete listing of args, just comm. */
int	gflg;	/* -g: complete listing including group headers, etc */
int	kflg;	/* -k: read from core file instead of real memory */
int	lflg;	/* -l: long listing form */
int	rflg;	/* -r: raw output in style <psout.h> */
int	sflg;	/* -s: stack depth */
int	uflg;	/* -u: user name */
#ifdef VFLAG
int     vflg;	/* -v: virtual memory statistics */
#endif
int	wflg;	/* -w[w]: wide terminal */
int	xflg;	/* -x: ALL processes, even those without ttys */
int	login;	/* -: this is a login shell */
char	*tptr;
int	swmem;
int	mem;
int	swap;
int	file;
daddr_t	swplo;
int	ndev;

# define NNAMESIZ (sizeof nl[0].n_name)
struct chanlis {	/*structure for unix wchan table*/
	char cname[NNAMESIZ];
	caddr_t caddr;
	struct chanlis *cptr;
};
struct chanlis *chanhd,*chantl;
struct chanlis chancap { "",0,0 };

char	*gettty();
char	*getptr();
char	*getchan();
int	pscomp();
long	round();
long	lseek();

struct devl {
	char	dname[DIRSIZ];
	dev_t	dev;
} devl[256];

struct map {
	long	b1, e1; long f1;
	long	b2, e2; long f2;
};
struct map datmap;

struct psout outargs[NPROC];	/* info for first npr processes */
int	npr;			/* number of processes found so far */
int	argwidth;		/* number of chars of args to print */

char	*coref;			/* name of kernel memory file to use */
char	*pcoref;		/* name of physical memory file to use */

main(argc, argv)
char **argv;
{
	int i;
	char *ap;
	int uid, puid;
	char obuf[BUFSIZ];

	setbuf(stdout, obuf);
	argc--, argv++;
	if (argc>0) {
		ap = argv[0];
		while (*ap) switch (*ap++) {
		case '-':
			break;

		case 'a':
			aflg++;
			break;

		case 'c':
			cflg++;
			break;

		case 'g':
			gflg++;
			break;

		case 'k':
			kflg++;
			break;

		case 'l':
			lflg++;
			break;

		case 'r':
			rflg++;
			break;

		case 's':
			sflg++;
			break;

		case 't':
			if(*ap)
				tptr = ap;
			aflg++;
			gflg++;
			if (*tptr == '?')
				xflg++;
			while (*ap)
				ap++;
			break;

		case 'u':
			uflg++;
			break;

#ifdef VFLAG
		case 'v':
			vflg++;
			break;
#endif

		case 'w':
			wflg++;
			break;

		case 'x':
			xflg++;
			break;

		default:
			chkpid=atoi(--ap);
			*ap = '\0';
			aflg++;
			xflg++;
			break;
		}
	}

	nlist(argc>3 ? argv[3] : "/unix", nl);
	if (nl[0].n_type==0) {
		fprintf(stderr, "No namelist\n");
		done(1);
	}
	if (chdir("/dev") < 0) {
		fprintf(stderr, "Can't chdir to /dev\n");
		done(1);
	}
	coref = "/dev/kmem";
	pcoref = "/dev/mem";
	if(kflg)
		pcoref = coref = "/usr/sys/core";
	if ((mem = open(coref, 0)) < 0) {
		fprintf(stderr, "No mem\n");
		done(1);
	}
	swmem = open(pcoref, 0);
	/*
	 * read mem to find swap dev.
	 */
	lseek(mem, (long)nl[1].n_value, 0);
	read(mem, (char *)&nl[1].n_value, sizeof(nl[1].n_value));
	/*
	 * Find base of swap
	 */
	lseek(mem, (long)nl[2].n_value, 0);
	read(mem, (char *)&swplo, sizeof(swplo));
	/*
	 * Locate proc table
	 */
	lseek(mem, (long)nl[0].n_value, 0);
	if (kflg)
		swplo = 0;
	getdev();
	uid = getuid();
	if (lflg + uflg > 1) {
		printf("Cannot combine l and u.\n");
		done(1);
	}
	/* different psout widths depending on how much printed & w flag */
	if (wflg <= 1) {
		argwidth = 57;
		if (wflg) argwidth += 52;	/* 132 col term */
		if (lflg) argwidth -= 47;	/* extra junk printed */
#ifdef VFLAG
		if (vflg) argwidth -= 48;	/* extra junk for -v */
#endif
		if (uflg) argwidth -= 17;	/* user name */
	} else  argwidth = 127;
	if (rflg)
		;	/* No heading for raw output */
	else if (lflg)
		printf("  F S  UID    PID  PPID CPU PRI NICE  ADDR  SZ WCHAN  TTY       TIME CMD\n");
#ifdef VFLAG
	else if (vflg)
		printf("F    PID TTY  TIME TIM SL MINFLT MAJFLT SIZE RSS SRS TSIZ TRS PF CMD\n");
#endif
	else if (uflg)
		printf("USER       PID NICE  SZ TTY       TIME CMD\n");
	else if (chkpid==0) {
		printf("   PID TTY       TIME CMD\n");
	}
	for (i=0; i<NPROC; i++) {
		read(mem, (char *)&mproc, sizeof mproc);
		/* skip processes that don't exist */
		if (mproc.p_stat==0)
			continue;
		/* skip those without a tty unless -x */
		if (mproc.p_pgrp==0 && xflg==0)
			continue;
		/* skip group leaders on a tty unless -g, -x, or -t.. */
		if (!gflg && !xflg && !tptr && mproc.p_pid == mproc.p_pgrp)
			continue;
		/* -g also skips those where **argv is "-" - see savcom */
		puid = mproc.p_uid;
		/* skip other peoples processes unless -a or a specific pid */
		if ((uid != puid && aflg==0) ||
		    (chkpid!=0 && chkpid!=mproc.p_pid))
			continue;
		if (savcom(puid))
			npr++;
	}
	fixup(npr);
	for (i=0; i<npr; i++)
		if (prcom(&outargs[i])) {
			putchar('\n');
			fflush(stdout);
		}
	done(!npr);
}

getdev()
{
#include <sys/stat.h>
	register FILE *df;
	struct stat sbuf;
	struct direct dbuf;

	if ((df = fopen("/dev", "r")) == NULL) {
		fprintf(stderr, "Can't open /dev\n");
		done(1);
	}
	ndev = 0;
	while (fread((char *) &dbuf, sizeof(dbuf), 1, df) == 1) {
		if(dbuf.d_ino == 0)
			continue;
		if(stat(dbuf.d_name, &sbuf) < 0)
			continue;
		if ((sbuf.st_mode&S_IFMT) != S_IFCHR)
			continue;
		strcpy(devl[ndev].dname, dbuf.d_name);
		devl[ndev].dev = sbuf.st_rdev;
		ndev++;
	}
	fclose(df);
	if ((swap = open("/dev/swap", 0)) < 0) {
		fprintf(stderr, "Can't open /dev/swap\n");
		done(1);
	}
}

savcom(puid)
{
	int abuf[INTPPG];
	long addr;
	register int *ip;
	register struct psout *a;
	register char *cp, *cp1;
	int cc, nbad;
	register char *tp;
	char **ap;
	int c;
	long txtsiz, datsiz, stksiz;
	int septxt;

	if (mproc.p_flag&SLOAD) {
		addr = ctob((long)mproc.p_addr);
		file = swmem;
	} else {
		addr = (mproc.p_addr+swplo)<<9;
		file = swap;
	}
	lseek(file, addr, 0);
	if (pread(file, (char *)&u, sizeof(u), addr) != sizeof(u)) {
		return(0);
	}

	/* set up address maps for user pcs */
	txtsiz = ctob(u.u_tsize);
	datsiz = ctob(u.u_dsize);
	stksiz = ctob(u.u_ssize);
	septxt = u.u_sep;
	datmap.b1 = (septxt ? 0 : round(txtsiz,TXTRNDSIZ));
	datmap.e1 = datmap.b1+datsiz;
	datmap.f1 = ctob(USIZE)+addr;
	datmap.b2 = stackbas(stksiz);
	datmap.e2 = stacktop(stksiz);
	datmap.f2 = ctob(USIZE)+(datmap.e1-datmap.b1)+addr;

	tp = gettty();
	if (tptr && strcmp(tptr, tp))
		return(0);
	a = &outargs[npr];
	/* saving com starts here */
	a->o_uid = puid;
	a->o_pid = mproc.p_pid;
	a->o_flag = mproc.p_flag;
	a->o_ppid = mproc.p_ppid;
	a->o_cpu  = mproc.p_cpu;
	a->o_pri  = mproc.p_pri;
	a->o_nice = mproc.p_nice;
	a->o_addr0 = mproc.p_addr;
	a->o_size = (mproc.p_size+7)>>3;
	a->o_wchan = mproc.p_wchan;
	a->o_pgrp = mproc.p_pgrp;
	/*
	a->o_tty[0] = tp[0];
	a->o_tty[1] = tp[1] ? tp[1] : ' ';
	a->o_tty[2] = tp[2] ? tp[2] : ' ';
	a->o_tty[3] = tp[3] ? tp[3] : ' ';
	*/
	strncpy(a->o_tty, tp, 8);
	a->o_ttyd = tp[0] == '?' ? -1 : u.u_ttyd;
	a->o_stat = mproc.p_stat;
	a->o_flag = mproc.p_flag;
	if (a->o_stat==SZOMB) return(1);
	a->o_utime = u.u_utime;
	a->o_stime = u.u_stime;
	a->o_cutime = u.u_cutime;
	a->o_cstime = u.u_cstime;
	a->o_sigs = u.u_signal[SIGINT] + u.u_signal[SIGQUIT];
#ifdef VFLAG
	a->o_time = mproc.p_time;
#endif
	a->o_uname[0] = 0;

	if (mproc.p_stat==SZOMB) return(1);
#ifdef VFLAG
	if (vflg) {
		register struct text *xp;

		if (mproc.p_textp) {
			xp = &text[mproc.p_textp - (struct text *)nl[5].n_value];
			a->o_xsize = xp->x_size;
			a->o_xrssize = xp->x_rssize;
			a->o_xswrss = xp->x_swrss;
			a->o_xccount = xp->x_ccount;
		} else {
			a->o_xsize = 0;
			a->o_xrssize = 0;
			a->o_xswrss = 0;
			a->o_xccount = 0;
		}
		a->o_aveflt = 0; /* mproc.p_aveflt; */
		a->o_minorflt = 0; /* u.u_minorflt; */
		a->o_majorflt = 0; /* u.u_majorflt; */
	}
#endif
	strcpy(a->o_comm, u.u_comm);
	if (cflg)
		return (1);
	a->o_args[0] = 0;	/* in case of early return */
	addr += ctob((long)mproc.p_size) - 512;

	/* look for sh special */
	lseek(file, addr+512-sizeof(char **), 0);
	if (read(file, (char *)&ap, sizeof(char *)) != sizeof(char *))
		return(1);
	if (ap) {
		char b[82];
		char *bp = b;
		while((cp=getptr(ap++)) && cp && (bp<b+sizeof(a->o_args)) ) {
			nbad = 0;
			while((c=getbyte(cp++)) && (bp<b+sizeof(a->o_args))) {
				if (c<' ' || c>'~') {
					if (nbad++>3)
						break;
					continue;
				}
				*bp++ = c;
			}
			*bp++ = ' ';
		}
		*bp++ = 0;
		strcpy(a->o_args, b);
		return(1);
	}

	lseek(file, addr, 0);
	if (pread(file, (char *) abuf, sizeof(abuf), addr) != sizeof(abuf))
		return(1);
	abuf[INTPPG] = 0;
	for (ip = &abuf[INTPPG-2]; ip > abuf;) {
		if (*--ip == -1 || *ip == 0) {
			cp = (char *)(ip+1);
			if (*cp==0)
				cp++;
			nbad = 0;
			for (cp1 = cp; cp1 < (char *)&abuf[INTPPG]; cp1++) {
				cc = *cp1&0177;
				if (cc==0)
					*cp1 = ' ';
				else if (cc < ' ' || cc > 0176) {
					if (++nbad >= 5) {
						*cp1++ = ' ';
						break;
					}
					*cp1 = '?';
				} else if (cc=='=') {
					*cp1 = 0;
					while (cp1>cp && *--cp1!=' ')
						*cp1 = 0;
					break;
				}
			}
			while (*--cp1==' ')
				*cp1 = 0;
			strcpy(a->o_args, cp);
garbage:
			cp = a->o_args;
			if (cp[0]=='-'&&cp[1]<=' ' || cp[0]=='?' || cp[0]<=' ') {
				strcat(cp, " (");
				strcat(cp, u.u_comm);
				strcat(cp, ")");
			}
			cp[63] = 0;	/* max room in psout is 64 chars */
			if (xflg || gflg || tptr || cp[0]!='-')
				return(1);
			return(0);
		}
	}
	goto garbage;
}

prcom(a)
	register struct psout *a;
{
	long tm;

	if (rflg) {
		write(1, (char *) a, sizeof (*a));
		return(0);
	}
	if (lflg) {
		printf("%3o %c", 0377 & a->o_flag,
			"0SWRIZT"[a->o_stat]);
		printf("%6u", a->o_uid);
#ifdef VFLAG
	} else if (vflg) {
		switch (a->o_stat) {

		case SSLEEP:
		case SSTOP:
			if ((a->o_flag & SLOAD) == 0)
				printf("W");
			else if (a->o_pri >= PZERO)
				printf("S");
			/* else if (a->o_flag & SPAGE)
				printf("P"); */
			else
				printf("D");
			break;

		case SRUN:
		case SIDL:
			if (a->o_flag & SLOAD)
				printf("R");
			else
				printf("W");
			break;
		}
		if (a->o_nice > NZERO)
			printf("N");
		else
			printf(" ");
#endif
	} else if (uflg) {
		printf("%-8.8s", a->o_uname);
	}
	printf("%6u", a->o_pid);
	if (lflg)
		printf("%6u%4d%4d%4d%7o", a->o_ppid, a->o_cpu&0377,
			a->o_pri, a->o_nice, a->o_addr0);
	else if (uflg) {
		/* printf("%5.1f", a->o_pctcpu); */
		printf("%4d ", a->o_nice);
	}
	if (lflg || uflg)
		printf("%4d", a->o_size);
	if (lflg)
		printf(" %-6.6s", getchan(a->o_wchan));
	printf(" %-8.8s", a->o_tty);
	if (a->o_stat==SZOMB) {
		printf("  <defunct>");
		return(1);
	}
	tm = (a->o_utime + a->o_stime + 30)/60;
	printf("%3ld:", tm/60);
	tm %= 60;
	printf(tm<10?"0%ld":"%ld", tm);
#ifdef VFLAG
	if (vflg) {
/*
		tm = (a->o_stime + 30) / 60;
		printf(" %2ld:", tm/60);
		tm %= 60;
		printf(tm<10?"0%ld":"%ld", tm);
*/
		printf("%4d%3d", a->o_time, a->o_slptime);
	}
#endif
#ifdef notdef
	if (0 && lflg==0) {	/* 0 == old tflg (print long times) */
		tm = (a->o_cstime + 30)/60;
		printf(" %2ld:", tm/60);
		tm %= 60;
		printf(tm<10?"0%ld":"%ld", tm);
		tm = (a->o_cutime + 30)/60;
		printf(" %2ld:", tm/60);
		tm %= 60;
		printf(tm<10?"0%ld":"%ld", tm);
	}
#endif
#ifdef VFLAG
	if (vflg) {
		printf("%7d%7d",a->o_minorflt,a->o_majorflt);
		printf("%5d%4d%4d", a->o_dsize+a->o_ssize, a->o_rssize, a->o_swrss);
		printf("%5d%4d", a->o_xsize, a->o_xrssize);
		printf("%3d", a->o_aveflt);
	}
#endif
	if (a->o_pid == 0) {
		printf(" swapper");
		return(1);
	}
	if (cflg) {
		printf(" %s", a->o_comm);
		return(1);
	}
	a -> o_args[argwidth] = 0;	/* force it to quit early */
	printf(" %s", a->o_args);
	return (1);
}

char *
gettty()
{
	register i;
	register char *p;

	if (u.u_ttyp==0)
		return("?");
	for (i=0; i<ndev; i++) {
		if (devl[i].dev == u.u_ttyd) {
			p = devl[i].dname;
			if (p[0]=='t' && p[1]=='t' && p[2]=='y')
				p += 3;
			return(p);
		}
	}
	return("?");
}

done(exitno)
{
	if (login) {
		printf("Press return when done: ");
		getchar();
	}
	exit(exitno);
}

/*
 * fixup figures out everybodys name and sorts into a nice order.
 */
fixup(np) int np; {
	register int i;
	register struct passwd *pw;
	struct passwd *getpwent();

	if (uflg) {
		/*
		 * If we want names, traverse the password file. For each
		 * passwd entry, look for it in the processes.
		 * In case of multiple entries in /etc/passwd, we believe
		 * the first one (same thing ls does).
		 */
		while ((pw=getpwent()) != NULL) {
			for (i=0; i<np; i++)
				if (outargs[i].o_uid == pw -> pw_uid) {
					if (outargs[i].o_uname[0] == 0)
						strcpy(outargs[i].o_uname, pw -> pw_name);
				}
		}
	}

	qsort(outargs, np, sizeof(outargs[0]), pscomp);
}

pscomp(x1, x2) struct psout *x1, *x2; {
	register int c;

#ifdef VFLAG
	if (vflg)
		return (vsize(x2) - vsize(x1));
#endif
	c = (x1)->o_ttyd - (x2)->o_ttyd;
	if (c==0) c = (x1)->o_pid - (x2)->o_pid;
	return(c);
}
char *
getptr(adr)
char **adr;
{
	char *ptr;
	register char *p, *pa;
	register i;

	ptr = 0;
	pa = (char *)adr;
	p = (char *)&ptr;
	for (i=0; i<sizeof(ptr); i++)
		*p++ = getbyte(pa++);
	return(ptr);
}

getbyte(adr)
char *adr;
{
	register struct map *amap = &datmap;
	char b;
	long saddr;

	if(!within(adr, amap->b1, amap->e1)) {
		if(within(adr, amap->b2, amap->e2)) {
			saddr = (unsigned)adr + amap->f2 - amap->b2;
		} else
			return(0);
	} else
		saddr = (unsigned)adr + amap->f1 - amap->b1;
	if(lseek(file, saddr, 0) == (long) -1
		   || read(file, &b, 1)<1) {
		return(0);
	}
	return((unsigned)b);
}


within(adr,lbd,ubd)
char *adr;
long lbd, ubd;
{
	return((unsigned)adr>=lbd && (unsigned)adr<ubd);
}

long
round(a, b)
	long	a, b;
{
	long	w = ((a+b-1)/b)*b;

	return (w);
}

/*
 * pread is like read, but if it's /dev/mem we use the phys
 * system call for speed.  (On systems without phys we have
 * to use regular read.)
 */
pread(fd, ptr, nbytes, loc)
char *ptr;
long loc;
{
	int rc;
	extern int errno;

	if (fd == swmem) {
		rc=phys(6, nbytes/64+1, (short)(loc/64));
		if (rc>=0) {
			memcpy(ptr, 0140000, nbytes);
			return nbytes;
		} else {
			return read(fd, ptr, nbytes);
		}
	} else {
		return read(fd, ptr, nbytes);
	}
}

memcpy(dest, src, nbytes)
register char *dest, *src;
register int nbytes;
{
	while (nbytes--)
		*dest++ = *src++;
}

struct chanlis *addchan(name,caddr)
char *name;
char *caddr;
{
	register struct chanlis *ccc,*lag;

	if( (ccc = malloc(sizeof*ccc))==0 )
		err("Too many symbols");
	strncpy(ccc->cname,name,NNAMESIZ-1);
	ccc->cname[NNAMESIZ-1] = 000;
	ccc->caddr = caddr;

	lag = ccc;
	ccc->cptr = chanhd;
	if( chanhd==0 )
	{
		chantl = ccc;
		chanhd = ccc;
		return;
	}
	if( chantl->caddr<caddr )
	{
		ccc->cptr = 0;
		chantl->cptr = ccc;
		chantl = ccc;
		return;
	}
	while( (ccc->cptr)!=0 && (ccc->cptr)->caddr<caddr )
	{
		lag = (ccc->cptr);
		(ccc->cptr) = lag->cptr;
	}
	if( lag==ccc )
		chanhd = ccc;
	else
		lag->cptr = ccc;
	return ccc;
}

char *
getchan(wchan)
char *wchan;
{
	register char *prevsym;
	register struct chanlis *ccc;

	prevsym = "";
	if( wchan!=0 )
		for( ccc = chanhd; ccc!=0; ccc = ccc->cptr )
		{
			if( ccc->caddr>wchan )
				return prevsym;
			prevsym = ccc->cname;
		}
	return prevsym;
}

nlist( aout,tgt )
char *aout;
struct nlist *tgt;
{
	static struct nlist nbuf;
	static struct exec hbuf;
	register struct nlist *nnn;
	FILE *aaa;
	int iii,len,nsyms;

	len = 0;
	for( nnn = tgt; nnn->n_name[0]!=000; nnn++ )
	{
		nnn->n_type = nnn->n_value = 0;
		len++;
	}
	if( (aaa = fopen(aout,"r"))==0 )
		err("Can't open symtab");
	if( fread(&hbuf,sizeof hbuf, 1, aaa) != 1 )
		err("Invalid symtab");
	nsyms = hbuf.a_syms/sizeof nbuf;
	fseek( aaa,(long)(unsigned) hbuf.a_text+hbuf.a_data,1 );
	while( --nsyms>=0 && fread(&nbuf,sizeof(nbuf),1,aaa)==1 )
	{
		if( nbuf.n_name[0]!='_' )
			continue;
		iii = nbuf.n_type&(N_TYPE|N_EXT);
		if( (nbuf.n_type&N_TYPE)!=N_ABS
		    && iii!= (N_EXT|N_DATA)
		    && iii!= (N_EXT|N_BSS) )
			continue;
		addchan(nbuf.n_name+1,nbuf.n_value);
		if( len!=0 )
			for( nnn = tgt; nnn->n_name[0]!=000; nnn++ )
				if(strncmp(nnn->n_name,nbuf.n_name,NNAMESIZ)==0)
				{
					nnn->n_value = nbuf.n_value;
					nnn->n_type = nbuf.n_type;
					len--;
					break;
				}
	}
	fclose(aaa);
	return len==0 ?0 :-1;
}

err(msg)
char *msg;
{
	fprintf(stderr, "%s\n", msg);
	done(1);
}
