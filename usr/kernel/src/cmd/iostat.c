#
	char	*sccsid = "@(#)iostat.c	2.5";

#
/*
 *	IOSTAT
 *
 *	Modified from Bell V7 distribution to measure 199 Cory
 *	configuration at U.C. Berkeley.
 *
 *	Modification History:
 *
 *	Bob Kridle - 4/2/80
 *		Changed devices to:
 *			HP0 (Ampex 9300, 33sect, 3600rpm)
 *			HP1 (Ampex 9300, 33sect, 3600rpm)
 *			HP Controller
 *		Added HS Controller
 *		Added -o option - Suppress first two lines
 *		Reformatted output for 1 line and removed -t option
 */
#include	<whoami.h>
#include	<stdio.h>
#define DKMAX	5
int	bflg;
int	dflg;
int	iflg;
int	aflg;
int	sflg;
int	oflg;		/* Data only, no summary line, no headers */
struct
{
	char	name[8];
	int	type;
	unsigned	value;
} nl[] = {
	"_dk_busy", 0, 0,
	"_io_info", 0, 0,
	"\0\0\0\0\0\0\0\0", 0, 0
};
struct
{
	int	busy;
	long	etime[2 << DKMAX];
	long	numb[DKMAX+1];
	long	wds[DKMAX+1];
	long	tin;
	long	tout;
} s, s1;
struct iostat {
	int	nbuf;
	long	nread;
	long	nreada;
	long	ncache;
	long	nwrite;
	long	bufcount[50];
} io_info, io_delta;
double	etime;

int	skip=1;
int	mf;

main(argc, argv)
char *argv[];
{
	extern char *ctime();
	register  i;
	int iter;
	double f1, f2;
	long t;

	nlist("/unix", nl);
	if(nl[0].type == -1) {
		printf("dk_busy not found in /unix namelist\n");
		exit(1);
	}
	mf = open("/dev/kmem", 0);
	if(mf < 0) {
		printf("cannot open /dev/kmem\n");
		exit(1);
	}
	iter = 0;
	while (argc>1&&argv[1][0]=='-') {
		if (argv[1][1]=='d')
			dflg++;
		else if (argv[1][1]=='s')
			sflg++;
		else if (argv[1][1]=='a')
			aflg++;
		else if (argv[1][1]=='i')
			iflg++;
		else if (argv[1][1]=='b')
			bflg++;
		else if (argv[1][1]=='o')
			oflg++;
		argc--;
		argv++;
	}
	if(argc > 2)
		iter = atoi(argv[2]);
		if(oflg) iter+=1;
	if (!(sflg|iflg|oflg|bflg)) {
		printf("     TTY");
		printf("      HS ACTIVE");
		printf("  HP0 SEEK  HP1 SEEK        HP XFER         PERCENT\n");
		printf("tin tout");
		printf("  tps msps mspt");
		printf("  tps msps  tps msps  tps msps mspt usr nic sys idl \n");
	}
	if(iflg&&!oflg)
	{
		printf("        RESOURCE USAGE (in percent)\n");
		printf(" user nice  sys idle iblk iact hp0s hp1s hpxf hsby\n");
	}

loop:
	lseek(mf, (long)nl[0].value, 0);
	read(mf, (char *)&s, sizeof s);
	for(i=0; i < ((2<<DKMAX) + (2*(DKMAX+1)) + 2); i++) {
		t = s.etime[i];
		s.etime[i] -= s1.etime[i];
		s1.etime[i] = t;
	}
	t = 0;
	for(i=0; i < (2<<DKMAX); i++)
		t += s.etime[i];
	etime = t;
	if(etime == 0.)
		etime = 1.;
	if(oflg && skip)
	{
		skip =0;
		goto contin;
	}
	if (bflg) {
		biostats();
		goto contin;
	}
	if (dflg) {
		long tm;
		time(&tm);
		printf("%s", ctime(&tm));
	}
	if (aflg)
		printf("%.2f minutes total\n", etime/3600);
	if (sflg) {
		stats2(etime);
		goto contin;
	}
	if (iflg) {
		stats3(etime);
		goto contin;
	}
	etime /= 60.;
	f1 = s.tin;
	f2 = s.tout;
	printf(" %3.0f", f1/etime);
	printf(" %3.0f", f2/etime);
	stats(5);
	for(i=0; i<3; i++)
		stats(i);
	for(i=0; i<4; i++)
		stat1(i*8);
	printf("\n");
contin:
	fflush(stdout);

	--iter;
	if(iter)
	if(argc > 1) {
		sleep(atoi(argv[1]));
		goto loop;
	}
}

/* usec per word for the various disks */
double	xf[] = {
	1.95,	/* Ampex 9300/Diva Comp 5 (33 sect.) */
	1.95,	/* Ampex 9300/Diva Comp 5 (33 sect.) */
	1.95,	/* Ampex 9300/Diva Comp 5 (33 sect.) */
	0.00,	/* Unused */
	0.00,	/* Unused */
	2.05,	/* RS04 */
};

stats(dn)
{
	register i;
	double f1, f2, f3;
	double f4, f5, f6;
	long t;

	t = 0;
	for(i=0; i < (2<<DKMAX); i++)
		if(i & (1<<dn))
			t += s.etime[i];
	f1 = t;
	f1 = f1/60.;
	f2 = s.numb[dn];
	if(f2 == 0.) {
		if(dn>1)
		    printf(" %4.0f %4.1f %4.1f", 0.0, 0.0, 0.0);
		else
		    printf(" %4.0f %4.1f", 0.0, 0.0);
		return;
	}
	f3 = s.wds[dn];
	f3 = f3*32.;
	f4 = xf[dn];
	f4 = f4*1.0e-6;
	f5 = f1 - f4*f3;
	f6 = f1 - f5;
	printf(" %4.0f", f2/etime);
	printf(" %4.1f", f5*1000./f2);
	if(dn >1) printf(" %4.1f", f6*1000./f2); /* no xfer time for seeks */
}

stat1(o)
{
	register i;
	long t;
	double f1, f2;

	t = 0;
	for(i=0; i < (2<<DKMAX); i++)
		t += s.etime[i];
	f1 = t;
	if(f1 == 0.)
		f1 = 1.;
	t = 0;
	for(i=0; i<8; i++)
	{
		t += s.etime[o+i];
		t += s.etime[o+i+32];
	}
	f2 = t;
	printf(" %3.0f", f2*100./f1);
}

stats2(t)
double t;
{
	register i, j;

	for (i=0; i<2; i++) {
		for (j=0; j<8; j++)
		{
			printf("%6.2f	", s.etime[8*i+j]/(t/100));
			printf("%6.2f	", s.etime[8*i+j+16]/(t/100));
			printf("%6.2f   ", s.etime[8*i+j+32]/(t/100));
			printf("%6.2f\n", s.etime[8*i+j+48]/(t/100));
		}
		printf("\n");
	}
	for(i=0; i<=DKMAX; i++)
		printf("%12D       %12D\n",s.numb[i],s.wds[i]);
}

stats3(t)
double t;
{
	register i;
	double sum;

	t /= 100;
	sum = 0;
	for(i=0; i < (2<<DKMAX); i++)
		if((i & (030))==000)
			sum += s.etime[i];
	printf(" %4.0f", sum/t);
	sum = 0;
	for(i=0; i < (2<<DKMAX); i++)
		if((i & (030))==010)
		    sum += s.etime[i];
	printf(" %4.0f", sum/t);
	sum = 0;
	for(i=0; i < (2<<DKMAX); i++)
		if((i & (030))==020)
		    sum += s.etime[i];
	printf(" %4.0f", sum/t);
	sum=0;
	for(i=0; i < (2<<DKMAX); i++)
		if((i & (030))==030)
		    sum += s.etime[i];
	printf(" %4.0f", sum/t);
	sum = 0;
	for(i=0; i < (2<<DKMAX); i++)
		if(((i & 030)==030) && (i & 047))
		    sum += s.etime[i];
	printf(" %4.0f", sum/t);
	sum = 0;
	for(i=0; i < (2<<DKMAX); i++)
		if((i & 047) && (( i & 030) != 030))
		    sum += s.etime[i];
	printf(" %4.0f", sum/t);
	sum = 0;
	for(i=0; i < (2<<DKMAX); i++)
		if(i & (01))
		    sum += s.etime[i];
	printf(" %4.0f", sum/t);
	sum = 0;
	for(i=0; i < (2<<DKMAX); i++)
		if(i & (02))
		    sum += s.etime[i];
	printf(" %4.0f", sum/t);
	sum = 0;
	for(i=0; i < (2<<DKMAX); i++)
		if(i & (04))
		    sum += s.etime[i];
	printf(" %4.0f", sum/t);
	sum = 0;
	for(i=0; i < (2<<DKMAX); i++)
		if(i & (040))
		    sum += s.etime[i];
	printf(" %4.0f\n", sum/t);
}

biostats()
{
register i;

	lseek(mf,(long)nl[1].value, 0);
	read(mf, (char *)&io_info, sizeof(io_info));
	printf("%D\t%D\t%D\t%D\n",
	 io_info.nread-io_delta.nread, io_info.nreada-io_delta.nreada,
	 io_info.ncache-io_delta.ncache, io_info.nwrite-io_delta.nwrite);

	for(i=0; i<30; ) {
		printf("%D\t",(long)io_info.bufcount[i]-io_delta.bufcount[i]);
		i++;
		if (i % 10 == 0)
			printf("\n");
	}
	io_delta = io_info;
}
