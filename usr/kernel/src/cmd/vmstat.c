#include <stdio.h>
#include <sys/param.h>
#include <sys/vm.h>

struct
{
	char	name[8];
	int	type;
	unsigned	value;
} nl[] = {
#define	X_DKBUSY 0
	"_dk_busy",	0, 0,
#define	X_RATE 1
	"_rate",	0, 0,
#define X_TOTAL 2
	"_total",	0, 0,
#define X_SUM 3
	"_sum",		0, 0,
#define	X_BOOTIME 4
	"_bootime",	0, 0,
	"\0\0\0\0\0\0\0\0", 0, 0
};

#define DKMAX	5	/* Max internal disk number (skipping 3&4) */
#define NDISK	4	/* Number of disks to print out */
double	stat1();
struct
{
	struct {
		int	busy;
		long	etime[2 << DKMAX];
		long	numb[DKMAX+1];
	} db;
	struct	vmmeter Rate;
	struct	vmtotal	Total;
	struct	vmmeter Sum;
} s, s1, z;
#define	rate		s.Rate
#define	total		s.Total
#define	sum		s.Sum

int	zero;
double	etime;
int 	mf;

main(argc, argv)
char **argv;
{
	time_t now;
	int lines;
	extern char *ctime();
	register i,j;
	int iter;
	long nintv;
	time_t bootime;
	double f1, f2;
	long t;
	extern char _sobuf[];

	setbuf(stdout, _sobuf);
	nlist("/unix", nl);
	if(nl[0].type == -1) {
		printf("no /unix namelist\n");
		exit(1);
	}
	mf = open("/dev/kmem", 0);
	if(mf < 0) {
		printf("cannot open /dev/kmem\n");
		exit(1);
	}
	iter = 0;
	argc--, argv++;
	while (argc>0 && argv[0][0]=='-') {
		char *cp = *argv++;
		argc--;
		while (*++cp) switch (*cp) {

		case 'z':
			close(mf);
			mf = open("/dev/kmem", 2);
			lseek(mf, (long)nl[X_SUM].value, 0);
			write(mf, &z.Sum, sizeof z.Sum);
			exit(0);
		
		case 's':
			dosum();
			exit(0);

		default:
			fprintf(stderr, "usage: vmstat [ -sz ] [ interval ] [ count]\n");
			exit(1);
		}
	}
	if(argc > 1)
		iter = atoi(argv[1]);
	lseek(mf, (long)nl[X_BOOTIME].value, 0);
	read(mf, &bootime, sizeof bootime);
	time(&now);
	nintv = now - bootime;
	if (nintv <= 0 || nintv > 60L*60L*24L*365L*10L) {
		printf("Time makes no sense... namelist must be wrong.\n");
		exit(1);
	}
reprint:
	lines = 20;
	/* s1 = z; */

	printf("   Procs  Virtual Real  Swap    Disk         Faults             Cpu\n");
	printf("RQ DW SW   AVM TX  FRE  I O  D0 D1 D2 D3  IN  SY  TR OV  CS US NI SY ID\n");

loop:
	lseek(mf, (long)nl[X_DKBUSY].value, 0);
 	read(mf, &s.db, sizeof s.db);
	if (nintv != 1) {
		lseek(mf, (long)nl[X_SUM].value, 0);
		read(mf, &rate, sizeof rate);
	} else {
		lseek(mf, (long)nl[X_RATE].value, 0);
		read(mf, &rate, sizeof rate);
	}
	{
		/*
		 * This handstand needed since the kernel smooths the
		 * rate structure, but there are so few overlay traps
		 * that it is always smoothed to zero, so we reconstruct
		 * the raw number of overlay traps from the sum.
		 */
		long oldovly = sum.v_ovly;

		lseek(mf, (long)nl[X_SUM].value, 0);
		read(mf, &sum, sizeof sum);
		rate.v_ovly = sum.v_ovly - oldovly;
	}
	lseek(mf, (long)nl[X_TOTAL].value, 0);
	read(mf, &total, sizeof total);
	for (i=0; i <=DKMAX; i++) {
		t = s.db.numb[i];
		s.db.numb[i] -= s1.db.numb[i];
		s1.db.numb[i] = t;
	}
	for (i=0; i < 2<<DKMAX; i++) {
		t = s.db.etime[i];
		s.db.etime[i] -= s1.db.etime[i];
		s1.db.etime[i] = t;
	}
	t = 0;
	for (i=0; i < 2<<DKMAX; i++)
		t += s.db.etime[i];
	etime = t;
	if(etime == 0.)
		etime = 1.;

	/* Procs RQ DW PW SW */
	printf("%2d%3d%3d", total.t_rq, total.t_dw,
	    total.t_sw);

	/* Memory AVM TX FRE */
	printf("%6ld%3d%5d", total.t_avm/16,
	    pct(total.t_avmtxt, total.t_avm), total.t_free/16);

	/* Swap I O */
	printf("%3ld%2ld", rate.v_swpin/nintv, rate.v_swpout/nintv);

	/* Disk D0 D1 D2 D3 */
	etime /= 60.;
	printf(" ");
	for(i=5; i<=DKMAX; i++)
		stats(i);
	for(i=0; i<3 && i<=DKMAX; i++)
		stats(i);

	/* Faults IN SY TR OV */
	printf("%4ld%4ld%4ld%3ld", (rate.v_intr-rate.v_trap)/nintv,
		rate.v_syscall/nintv, rate.v_trap/nintv,
		nintv > 1 ? rate.v_ovly/(nintv/5) : rate.v_ovly);

	/* context switches CS */
	printf("%4ld", rate.v_swtch/nintv);

	/* Cpu  US SY ID */
	for(i=0; i < 32; i += 8) {
		float f = stat1(i);
		printf("%3.0f", f);
	}
	printf("\n");
	fflush(stdout);
contin:
	nintv = 1;
	--iter;
	if(iter)
	if(argc > 0) {
		sleep(atoi(argv[0]));
		if (--lines <= 0)
			goto reprint;
		goto loop;
	}
}

dosum()
{

	lseek(mf, (long)nl[X_SUM].value, 0);
	read(mf, &sum, sizeof sum);
	printf("%9ld swap ins\n", sum.v_swpin);
	printf("%9ld swap outs\n", sum.v_swpout);
	printf("%9ld pages swapped in\n", sum.v_pswpin / CLSIZE);
	printf("%9ld pages swapped out\n", sum.v_pswpout / CLSIZE);
	printf("%9ld cpu context switches\n", sum.v_swtch);
	printf("%9ld device interrupts\n", sum.v_intr);
	printf("%9ld traps\n", sum.v_trap);
	printf("%9ld system calls\n", sum.v_syscall);
	printf("%9ld overlay traps\n", sum.v_ovly);
}

stats(dn)
{

	if (dn > DKMAX) {
		printf("  0");
		return;
	}
	printf("%3.0f", s.db.numb[dn]/etime);
}

double
stat1(row)
{
	register i;
	long t;
	double f1, f2;

	t = 0;
	for(i=0; i < 2<<DKMAX; i++)
		t += s.db.etime[i];
	f1 = t;
	if(f1 == 0.)
		f1 = 1.;

	t = 0;
	for(i=0; i<8; i++) {
		t += s.db.etime[row+i];
		t += s.db.etime[row+i+32];
	}

	f2 = t;
	return(f2*100./f1);
}

pct(top, bot)
long top, bot;
{
	int ret;

	if (bot == 0)
		return (0);
	ret = (top * 100) / bot;
	return ret;
}
