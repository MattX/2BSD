#
	char	*sccsid = "@(#)login.c	2.8";

/*
 * login [ name ]
 */

#include <sys/types.h>
#include <sgtty.h>
#include <utmp.h>
#include <signal.h>
#include <pwd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <lastlog.h>
#include <whoami.h>

#define SCPYN(a, b)	strncpy(a, b, sizeof(a))
#define NMAX	sizeof(utmp.ut_name)
#define LMAX	sizeof(utmp.ut_line)

char	maildir[30] =	"/usr/spool/mail/";
char	lastlog[] =	"/usr/adm/lastlog";
struct	passwd nouser = {"", "nope"};
struct	sgttyb ttyb;
struct	utmp utmp;
char	minusnam[16] = "-";
char	homedir[64] = "HOME=";
char	term[64] = "TERM=";
char	shell[64] = "SHELL=";
char	user[NMAX+9] = "USER=";

char	*envinit[] = {homedir,
#ifdef CORY
		/* Local default path */
		"PATH=:/usr/eecs/bin:/usr/ucb/bin",
#else
		"PATH=:/usr/ucb/bin",
#endif
		term, shell,  user, 0};
struct	passwd *pwd;

struct	passwd *getpwnam();
char	*strcat();
int	setpwent();
char	*ttyname();
char	*crypt();
char	*v6crypt();
char	*getpass();
char	*rindex(), *index();
char	*ttyn;
extern	char **environ;
extern	char _sobuf[];

#define	CTRL(c)	('c' & 037)
struct	tchars tc={
	0177, CTRL(\\), CTRL(q), CTRL(s), CTRL(d), 0377
};

#ifdef UCB_NTTY
struct	ltchars ltc={
	CTRL(z), CTRL(y), CTRL(r), CTRL(o), CTRL(w), CTRL(v),
	CTRL(h), CTRL(u), CTRL(c)
};
#endif

main(argc, argv)
char **argv;
{
	register char *namep;
	int t, f, c, wasslash;
	int ldisc = 0;
	char *cp;

	setbuf(stdout, _sobuf);
	alarm(180);
	signal(SIGQUIT, SIG_IGN);
	nice(-100);
	nice(20);
	nice(0);

	gtty(0, &ttyb);
	ttyb.sg_erase = '#';
	ttyb.sg_kill = '@';
	stty(0, &ttyb);
#ifdef UCB_NTTY
	ioctl(0, TIOCSETD, &ldisc);
	ioctl(0, TIOCLSET, 0);
	ioctl(0, TIOCSLTC, &ltc);
#endif
	ioctl(0, TIOCSETC, &tc);

	for (t=3; t<20; t++)
		close(t);
	ttyn = ttyname(0);
	if (ttyn==0)
		ttyn = "/dev/tty??";

    loop:
	SCPYN(utmp.ut_name, "");
	if (argc>1) {
		SCPYN(utmp.ut_name, argv[1]);
		argc = 0;
	}
	while (utmp.ut_name[0] == '\0') {
		namep = utmp.ut_name;
		printf("login: ");
		fflush(stdout);
		while ((c = getchar()) != '\n') {
			if(c == ' ')
				c = '_';
			if (c == EOF)
				exit(0);
			if (namep < utmp.ut_name+NMAX)
				*namep++ = c;
		}
	}
	setpwent();
	if ((pwd = getpwnam(utmp.ut_name)) == NULL)
		pwd = &nouser;
	endpwent();
	if (*pwd->pw_passwd != '\0') {
		char *pw;
		namep = crypt(pw=getpass("Password:"),pwd->pw_passwd);
		if (strcmp(namep, pwd->pw_passwd)) {
			/*
			 * Hack for conversion from v6 to v7, which have
			 * different encryption algorithms.  We accept a
			 * password which matches with either algorithm,
			 * but warn v6 users to change it.
			 */
			namep = v6crypt(pw);
			if (strcmp(namep, pwd->pw_passwd)) {
				printf("Login incorrect\n");
				goto loop;
			}
			printf("Be sure to change your version 6 password\n");
		}
		signal(SIGINT, SIG_IGN);
		/*
		 * Don't allow root anywhere but the console.  Also, only
		 * staff on the root dialup.  This is a local policy decision.
		 */
#ifdef CORY
		if ((pwd->pw_uid == 0 && strcmp(ttyn, "/dev/console")) ||
		    (!strcmp(ttyn, "/dev/dz15") && pwd->pw_gid != 2)) {
			printf("Login incorrect\n");
			goto loop;
		}
#endif
	}
	alarm(0);
	if((f = open(lastlog, 2)) >= 0) {
		struct lastlog ll;

		lseek(f, (long) ((unsigned) (pwd->pw_uid) * sizeof (struct lastlog)), 0);
		if (read(f, (char *) &ll, sizeof ll) == sizeof ll && ll.ll_time != 0) {
			register char *ep = (char *) ctime(&ll.ll_time);
			printf("Last login: ");
			ep[24 - 5] = 0;
			printf("%s on %.*s\n", ep, LMAX, ll.ll_line);
			fflush(stdout);	/* So user sees the message quickly! */
		}
		lseek(f, (long) ((unsigned) (pwd->pw_uid) * sizeof (struct lastlog)), 0);
		time(&ll.ll_time);
		strncpy(ll.ll_line, ttyn+5, LMAX);
		write(f, (char *) &ll, sizeof ll);
		close(f);
	}
	showfile("/etc/motd");
	time(&utmp.ut_time);
	t = ttyslot();
	if (t>0 && (f = open("/etc/utmp", 1)) >= 0) {
		lseek(f, (long)(t*sizeof(utmp)), 0);
		SCPYN(utmp.ut_line, rindex(ttyn, '/')+1);
		write(f, (char *)&utmp, sizeof(utmp));
		close(f);
	}
	if (t>0 && (f = open("/usr/adm/wtmp", 1)) >= 0) {
		lseek(f, 0L, 2);
		write(f, (char *)&utmp, sizeof(utmp));
		close(f);
	}
	getterm();
	chown(ttyn, pwd->pw_uid, pwd->pw_gid);
	chmod(ttyn, 0622);
	setgid(pwd->pw_gid);
	setuid(pwd->pw_uid);
	namep = pwd->pw_dir;
	for (;;) {
		if (*namep == '\0')
			break;
		cp = namep++;
		for (; *namep != '/' && *namep != '\0'; namep++);
		wasslash = 0;
		if (*namep == '/') {
			*namep = '\0';
			wasslash++;
		}
		if (chdir(cp)<0) {
			printf("No directory\n");
			exit(1);
		}
		showfile(".broadcast");
		if (wasslash)
			*namep++ = '/';
	}
	showfile(".reminder");

	if (*pwd->pw_shell == '\0')
		pwd->pw_shell = "/bin/csh";
	environ = envinit;
	strncat(homedir, pwd->pw_dir, sizeof(homedir)-6);
	strncat(user, pwd->pw_name, sizeof(user)-6);
	if ((namep = rindex(pwd->pw_shell, '/')) == NULL)
		namep = pwd->pw_shell;
	else
		namep++;
	strncat(shell, pwd->pw_shell, sizeof(shell)-7);
	strcat(minusnam, namep);
	/*
	 * The umask is a local decision.  077 is very paranoid (everything
	 * is highly secret).  0 is wide open (everything readable and writable
	 * by anyone.)  022 is moderate.  027 is also a possibility.
	 */
	umask(077);
	strcat(maildir, pwd->pw_name);
	if(access(maildir,4)==0) {
		struct stat statb;
		stat(maildir, &statb);
		if (statb.st_size)
			printf("You have mail.\n");
	}
	signal(SIGQUIT, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	fflush(stdout);
	execlp(pwd->pw_shell, minusnam, 0);
	printf("No shell\n");
	exit(0);
}

int	stopmotd;
catch()
{
	signal(SIGINT, SIG_IGN);
	stopmotd++;
	printf("\n");
	fflush(stdout);		/* Immediate-looking response. */
}

showfile(name)
char *name;
{
	FILE *mf;
	register c;

	stopmotd = 0;
	signal(SIGINT, catch);
	if((mf = fopen(name,"r")) != NULL) {
		while((c = getc(mf)) != EOF && stopmotd == 0)
			putchar(c);
		fclose(mf);
		fflush(stdout);
	}
	signal(SIGINT, SIG_IGN);
}

/*
 * make a reasonable guess as to the kind of terminal the user is on.
 * We look in /etc/ttytype for this info (format: each line has two
 * words, first word is a term type, second is a tty name), and default
 * to "unknown" if we can't find any better.  In the case of dialups we get
 * names like "dialup" which is a lousy guess but tset can
 * take it from there.
 */
getterm()
{

	register char	*sp, *tname;
	register int	i;
	register FILE	*fdes;
	char		*type, *t;
	char		ttline[64];

	if ((fdes = fopen("/etc/ttytype", "r")) == NULL) {
unknown:
		strcat(term, "unknown");
		fclose(fdes);
		return;
	}
	for (tname = ttyn; *tname++; )
		;
	while (*--tname != '/')
		;
	tname++;
	while (fgets(ttline, sizeof(ttline), fdes) != NULL) {
		ttline[strlen(ttline)-1] = 0;	/* zap \n on end */
		type = ttline;
		for (t=ttline; *t && *t!=' '; t++)
			;
		*t++ = 0;
		/* Now have term and type pointing to the right guys */
		if (strcmp(t, tname) == 0) {
			strcat(term, type);
			fclose(fdes);
			return;
		}
	}
	goto unknown;
}

/*
 * An alleged "version 6 crypt" routine, with the names changed to avoid
 * conflicts with the v7 crypt routine, follows.  This may not match the
 * version of crypt you were running at your installation for v6.
 */

/*
 * This program implements the
 * Proposed Federal Information Processing
 *  Data Encryption Standard.
 * See Federal Register, March 17, 1975 (40FR12134)
 */

/*
 * Initial permutation,
 */
char	v6IP[] {
	58,50,42,34,26,18,10, 2,
	60,52,44,36,28,20,12, 4,
	62,54,46,38,30,22,14, 6,
	64,56,48,40,32,24,16, 8,
	57,49,41,33,25,17, 9, 1,
	59,51,43,35,27,19,11, 3,
	61,53,45,37,29,21,13, 5,
	63,55,47,39,31,23,15, 7,
};

/*
 * Final permutation, v6FP = v6IP^(-1)
 */
char	v6FP[] {
	40, 8,48,16,56,24,64,32,
	39, 7,47,15,55,23,63,31,
	38, 6,46,14,54,22,62,30,
	37, 5,45,13,53,21,61,29,
	36, 4,44,12,52,20,60,28,
	35, 3,43,11,51,19,59,27,
	34, 2,42,10,50,18,58,26,
	33, 1,41, 9,49,17,57,25,
};

/*
 * Permuted-choice 1 from the key bits
 * to yield v6C and v6D.
 * Note that bits 8,16... are left out:
 * They are intended for a parity check.
 */
char	v6PC1_C[] {
	57,49,41,33,25,17, 9,
	 1,58,50,42,34,26,18,
	10, 2,59,51,43,35,27,
	19,11, 3,60,52,44,36,
};

char	v6PC1_D[] {
	63,55,47,39,31,23,15,
	 7,62,54,46,38,30,22,
	14, 6,61,53,45,37,29,
	21,13, 5,28,20,12, 4,
};

/*
 * Sequence of v6shifts used for the key schedule.
*/
char	v6shifts[] {
	1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1,
};

/*
 * Permuted-choice 2, to pick out the bits from
 * the CD array that generate the key schedule.
 */
char	v6PC2_C[] {
	14,17,11,24, 1, 5,
	 3,28,15, 6,21,10,
	23,19,12, 4,26, 8,
	16, 7,27,20,13, 2,
};

char	v6PC2_D[] {
	41,52,31,37,47,55,
	30,40,51,45,33,48,
	44,49,39,56,34,53,
	46,42,50,36,29,32,
};

/*
 * The v6C and v6D arrays used to calculate the key schedule.
 */

char	v6C[28];
char	v6D[28];
/*
 * The key schedule.
 * Generated from the key.
 */
char	v6KS[16][48];

/*
 * Set up the key schedule from the key.
 */

v6setkey(key)
char *key;
{
	register i, j, k;
	int t;

	/*
	 * First, generate v6C and v6D by permuting
	 * the key.  The low order bit of each
	 * 8-bit char is not used, so v6C and v6D are only 28
	 * bits apiece.
	 */
	for (i=0; i<28; i++) {
		v6C[i] = key[v6PC1_C[i]-1];
		v6D[i] = key[v6PC1_D[i]-1];
	}
	/*
	 * To generate Ki, rotate v6C and v6D according
	 * to schedule and pick up a permutation
	 * using PC2.
	 */
	for (i=0; i<16; i++) {
		/*
		 * rotate.
		 */
		for (k=0; k<v6shifts[i]; k++) {
			t = v6C[0];
			for (j=0; j<28-1; j++)
				v6C[j] = v6C[j+1];
			v6C[27] = t;
			t = v6D[0];
			for (j=0; j<28-1; j++)
				v6D[j] = v6D[j+1];
			v6D[27] = t;
		}
		/*
		 * get Ki. Note v6C and v6D are concatenated.
		 */
		for (j=0; j<24; j++) {
			v6KS[i][j] = v6C[v6PC2_C[j]-1];
			v6KS[i][j+24] = v6D[v6PC2_D[j]-28-1];
		}
	}
}

/*
 * The v6E bit-selection table.
 */
char	v6E[] {
	32, 1, 2, 3, 4, 5,
	 4, 5, 6, 7, 8, 9,
	 8, 9,10,11,12,13,
	12,13,14,15,16,17,
	16,17,18,19,20,21,
	20,21,22,23,24,25,
	24,25,26,27,28,29,
	28,29,30,31,32, 1,
};

/*
 * The 8 selection functions.
 * For some reason, they give a 0-origin
 * index, unlike everything else.
 */
char	v6S[8][64] {
	14, 4,13, 1, 2,15,11, 8, 3,10, 6,12, 5, 9, 0, 7,
	 0,15, 7, 4,14, 2,13, 1,10, 6,12,11, 9, 5, 3, 8,
	 4, 1,14, 8,13, 6, 2,11,15,12, 9, 7, 3,10, 5, 0,
	15,12, 8, 2, 4, 9, 1, 7, 5,11, 3,14,10, 0, 6,13,

	15, 1, 8,14, 6,11, 3, 4, 9, 7, 2,13,12, 0, 5,10,
	 3,13, 4, 7,15, 2, 8,14,12, 0, 1,10, 6, 9,11, 5,
	 0,14, 7,11,10, 4,13, 1, 5, 8,12, 6, 9, 3, 2,15,
	13, 8,10, 1, 3,15, 4, 2,11, 6, 7,12, 0, 5,14, 9,

	10, 0, 9,14, 6, 3,15, 5, 1,13,12, 7,11, 4, 2, 8,
	13, 7, 0, 9, 3, 4, 6,10, 2, 8, 5,14,12,11,15, 1,
	13, 6, 4, 9, 8,15, 3, 0,11, 1, 2,12, 5,10,14, 7,
	 1,10,13, 0, 6, 9, 8, 7, 4,15,14, 3,11, 5, 2,12,

	 7,13,14, 3, 0, 6, 9,10, 1, 2, 8, 5,11,12, 4,15,
	13, 8,11, 5, 6,15, 0, 3, 4, 7, 2,12, 1,10,14, 9,
	10, 6, 9, 0,12,11, 7,13,15, 1, 3,14, 5, 2, 8, 4,
	 3,15, 0, 6,10, 1,13, 8, 9, 4, 5,11,12, 7, 2,14,

	 2,12, 4, 1, 7,10,11, 6, 8, 5, 3,15,13, 0,14, 9,
	14,11, 2,12, 4, 7,13, 1, 5, 0,15,10, 3, 9, 8, 6,
	 4, 2, 1,11,10,13, 7, 8,15, 9,12, 5, 6, 3, 0,14,
	11, 8,12, 7, 1,14, 2,13, 6,15, 0, 9,10, 4, 5, 3,

	12, 1,10,15, 9, 2, 6, 8, 0,13, 3, 4,14, 7, 5,11,
	10,15, 4, 2, 7,12, 9, 5, 6, 1,13,14, 0,11, 3, 8,
	 9,14,15, 5, 2, 8,12, 3, 7, 0, 4,10, 1,13,11, 6,
	 4, 3, 2,12, 9, 5,15,10,11,14, 1, 7, 6, 0, 8,13,

	 4,11, 2,14,15, 0, 8,13, 3,12, 9, 7, 5,10, 6, 1,
	13, 0,11, 7, 4, 9, 1,10,14, 3, 5,12, 2,15, 8, 6,
	 1, 4,11,13,12, 3, 7,14,10,15, 6, 8, 0, 5, 9, 2,
	 6,11,13, 8, 1, 4,10, 7, 9, 5, 0,15,14, 2, 3,12,

	13, 2, 8, 4, 6,15,11, 1,10, 9, 3,14, 5, 0,12, 7,
	 1,15,13, 8,10, 3, 7, 4,12, 5, 6,11, 0,14, 9, 2,
	 7,11, 4, 1, 9,12,14, 2, 0, 6,10,13,15, 3, 5, 8,
	 2, 1,14, 7, 4,10, 8,13,15,12, 9, 0, 3, 5, 6,11,
};

/*
 * v6P is a permutation on the selected combination
 * of the current v6L and key.
 */
char	v6P[] {
	16, 7,20,21,
	29,12,28,17,
	 1,15,23,26,
	 5,18,31,10,
	 2, 8,24,14,
	32,27, 3, 9,
	19,13,30, 6,
	22,11, 4,25,
};

/*
 * The current block, divided into 2 halves.
 */
char	v6L[32], v6R[32];
char	v6tempL[32];
char	v6f[32];
char	v6tempf[32];

/*
 * The combination of the key and the input, before selection.
 */
char	v6preS[48];

/*
 * The payoff: v6encrypt a block.
 */

v6encrypt(block, edflag)
char *block;
{
	int i, ii;
	register t, j, k;

	/*
	 * First, permute the bits in the input
	 */
	for (j=0; j<64; j++)
		v6L[j] = block[v6IP[j]-1];
	/*
	 * Perform an v6encryption operation 16 times.
	 */
	for (ii=0; ii<16; ii++) {
		/*
		 * Set direction
		 */
		if (edflag)
			i = 15-ii;
		else
			i = ii;
		/*
		 * Save the v6R array,
		 * which will be the new v6L.
		 */
		for (j=0; j<32; j++)
			v6tempL[j] = v6R[j];
		/*
		 * Expand v6R to 48 bits using the v6E selector;
		 * exclusive-or with the current key bits.
		 */
		for (j=0; j<48; j++)
			v6preS[j] = v6R[v6E[j]-1] ^ v6KS[i][j];
		/*
		 * The pre-select bits are now considered
		 * in 8 groups of 6 bits each.
		 * The 8 selection functions map these
		 * 6-bit quantities into 4-bit quantities
		 * and the results permuted
		 * to make an v6f(v6R, K).
		 * The indexing into the selection functions
		 * is peculiar; it could be simplified by
		 * rewriting the tables.
		 */
		for (j=0; j<8; j++) {
			t = 6*j;
			k = v6S[j][(v6preS[t+0]<<5)+
				(v6preS[t+1]<<3)+
				(v6preS[t+2]<<2)+
				(v6preS[t+3]<<1)+
				(v6preS[t+4]<<0)+
				(v6preS[t+5]<<4)];
			t = 4*j;
			v6f[t+0] = (k>>3)&01;
			v6f[t+1] = (k>>2)&01;
			v6f[t+2] = (k>>1)&01;
			v6f[t+3] = (k>>0)&01;
		}
		/*
		 * The new v6R is v6L ^ v6f(v6R, K).
		 * The v6f here has to be permuted first, though.
		 */
		for (j=0; j<32; j++)
			v6R[j] = v6L[j] ^ v6f[v6P[j]-1];
		/*
		 * Finally, the new v6L (the original v6R)
		 * is copied back.
		 */
		for (j=0; j<32; j++)
			v6L[j] = v6tempL[j];
	}
	/*
	 * The output v6L and v6R are reversed.
	 */
	for (j=0; j<32; j++) {
		t = v6L[j];
		v6L[j] = v6R[j];
		v6R[j] = t;
	}
	/*
	 * The final output
	 * gets the inverse permutation of the very original.
	 */
	for (j=0; j<64; j++)
		block[j] = v6L[v6FP[j]-1];
}

char *
v6crypt(pw)
char *pw;
{
	register i, j, c;
	static char block[66], iobuf[16];
	int l;

	for(i=0; i<66; i++)
		block[i] = 0;
	for(i=0; (c= *pw) && i<64; pw++){
		for(j=0; j<7; j++, i++)
			block[i] = (c>>(6-j)) & 01;
		i++;
	}
	
	l = v6norm(block, j);
	v6setkey(block);
	
	for(i=0; i<66; i++)
		block[i] = 0;
	
	for(i=0; i<29+l; i++)
		v6encrypt(block,0);
	
	for(i=0; i<11; i++){
		c = 0;
		for(j=0; j<6; j++){
			c =<< 1;
			c =| block[6*i+j];
			}
		c =+ '.';
		if(c>'9') c =+ 7;
		if(c>'Z') c =+ 6;
		iobuf[i] = c;
	}
	iobuf[i] = 0;
	return(iobuf);
}

v6norm(block, start)
char *block;
{
	register char *cp;
	register i, k;

	i = start;
	for (cp=block; cp < &block[66]; cp++)
		if (*cp == 0)
			i++;
	k = 2;
	for (; i < 66; i =+ k) {
		block[i] =^ 1;
		k++;
	}
	return(k&03);
}
