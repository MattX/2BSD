static	char	*rcsid =
	"$Header: files.c,v 1.12 83/11/28 09:49:39 tony Exp $";

/*
 * $Log:	files.c,v $
 * Revision 1.12  83/11/28  09:49:39  tony
 * Copied in Charlie`s newest version.
 * 
 * Revision 1.14  83/11/25  14:02:18  crl
 * Re-installed a change I had removed--to be safe, in srchdir(), check to
 * 	make sure dirf is non-null before rewinddir().
 * 
 * Revision 1.13  83/11/21  12:58:43  crl
 * In fixing the problem mentioned below (1.12), I broke the directory search
 * 	for implicits.  The directory was not searched the first time it
 * 	was opened.
 * 
 * Revision 1.12  83/11/18  23:22:39  crl
 * Fixed bug in srchdir():  if the RCS dir wasn't present, the dir was
 * 	entered in the open dir chain anyways, causing the second reference
 * 	to the directory to think it was already open.
 * 
 * Revision 1.11  83/11/02  21:18:50  crl
 * Fixes bug where RCS files in . were not handled properly
 * 
 * Revision 1.10  83/11/02  10:30:47  tony
 * Fixed an old bug that was aggravated by Charlie`s changes.
 * 
 * Revision 1.9  83/11/01  20:47:54  tony
 * Added crl`s latest changes (11/1/83)
 * 
 * Revision 1.8  83/10/24  14:33:36  tony
 * Added Charlie`s latest changes to :
 * 	1. set isrcs inside exists().
 * 	2. co() now uses docom() and ".CO" target.
 * 	3. rm() now uses docom() and ".CLEANUP" target.
 * 
 * Revision 1.7  83/10/14  14:14:13  tony
 * Added code to interpret '@' and '-' characters on the CO and RM
 * commands and call docom1 accordingly.
 * 
 * Revision 1.6  83/10/12  11:05:51  tony
 * Fixed bug in srchdir that caused files in RCS subdirectories
 * to be overlooked unless mentioned in another rule in the makefile.
 * 
 * Revision 1.5  83/10/11  13:51:17  tony
 * Added a new argument to co() that tells whether or not to mark a
 * checked out file for later deletion.
 * 
 * Revision 1.4  83/10/11  12:50:59  tony
 * Changed rm() routine to use the command "$(RM) $(RMFLAGS) file"
 * and call docom1 so that the way a file is removed is consistent
 * with the way it is checked out.
 * 
 * Revision 1.3  83/10/06  16:08:25  tony
 * Converted to use new directory routines.
 * 
 * Revision 1.2  83/10/06  10:50:46  tony
 * Added code from physics for RCS.
 * 
 */

/*	@(#)/usr/src/cmd/make/files.c	3.4	*/

#ifndef	MAXPATH
#define	MAXPATH	1024
#endif

#include "defs.h"
#include <sys/stat.h>
#include <pwd.h>
#include "ar.h"
#include "ar11.h"
#include <a.out.h>
/* UNIX DEPENDENT PROCEDURES */




TIMETYPE exists(pname,ch)
NAMEBLOCK pname;
struct	chain **ch;
{
	register CHARSTAR s;
	struct stat buf;
	TIMETYPE lookarch();
	CHARSTAR filename;
	time_t tmp;

	filename = pname->namep;

	if(any(filename, LPAREN))
		return(lookarch(filename));

	if(stat(filename,&buf) < 0) 
	{
		if(IS_ON(COFLAG) && ch)
			return(getrcs(pname,ch));
		s = findfl(filename);
		if(s != (CHARSTAR )-1)
		{
			pname->alias = copys(s);
			if(stat(pname->alias, &buf) == 0)
				return(buf.st_mtime);
		}
		return(0);
	}
	else
		return(buf.st_mtime);
}


FSTATIC char nbuf[MAXNAMLEN+1];
FSTATIC CHARSTAR nbufend = &nbuf[MAXNAMLEN];



DEPBLOCK srchdir(pat, mkchain, nextdbl,die)
register CHARSTAR pat;		/* pattern to be matched in directory */
int mkchain;			/* nonzero if results to be remembered */
DEPBLOCK nextdbl;		/* final value for chain */
int die;
{
	DIR *dirf;
	int i, nread, cldir;
	CHARSTAR dirname, dirpref, endir, filepat, p;
	char temp[MAXPATH];
	char fullname[MAXPATH];
	CHARSTAR p1, p2;
	NAMEBLOCK q;
	DEPBLOCK thisdbl;
	OPENDIR od;
	PATTERN patp;
	char temp2[MAXPATH],*RCSpref;

	struct direct *dptr;

	thisdbl = 0;

	if(mkchain == NO)
		for(patp=firstpat ; patp!=0 ; patp = patp->nextpattern)
			if(equal(pat,patp->patval))
				return(0);

	patp = ALLOC(pattern);
	patp->nextpattern = firstpat;
	firstpat = patp;
	patp->patval = copys(pat);

	if((endir = rindex(pat,'/')) == NULL)
	{
		dirname = ".";
		dirpref = "";
		filepat = pat;
	}
	else
	{
		dirname = pat;
		*endir = CNULL;
		dirpref = concat(pat, "/", temp);
		filepat = endir+1;
	}

	RCSpref = NULL;
	if(IS_ON(COFLAG) && !mkchain) {
		if(suffix(dirname,RCSdir,temp2))
			RCSpref = temp2;
	}

	dirf = NULL;
	cldir= NO;

	for(od=firstod ; od!=0; od = od->nextopendir)
		if(equal(dirname, od->dirn))
		{
			if ((dirf = od->dirfc) != NULL)
				rewinddir(dirf); /* start over */
			break;
		}

	if(dirf == NULL)
	{
		if ((dirf = opendir(dirname)) == NULL) {
			if(die) {
				fprintf(stderr, "Directory %s: ", dirname);
				fatal("Cannot open");
			}
			if (endir)
				*endir = '/';
			return(NULL);
		}
		if(nopdir >= MAXDIR)
			cldir = YES;
		else {
			++nopdir;
			od = ALLOC(opendir);
			od->nextopendir = firstod;
			firstod = od;
			od->dirfc = dirf;
			od->dirn = copys(dirname);
		}
	}
	for(dptr = readdir(dirf); dptr != NULL; dptr = readdir(dirf)) {
		p1 = dptr->d_name;
		p2 = nbuf;
		while((p2<nbufend) && (*p2++ = *p1++) != CNULL)
			;
		if(amatch(nbuf,filepat)) {
			concat(dirpref,nbuf,fullname);
			if( (q=srchname(fullname)) ==0)
				q = makename(copys(fullname));
			if(mkchain) {
				thisdbl = ALLOC(depblock);
				thisdbl->nextdep = nextdbl;
				thisdbl->depname = q;
				nextdbl = thisdbl;
			}
	/* if RCSpref is non-null, it is the dirpref
	 * without the "RCS/". This, along with RCS_SUF
	 * is stripped so implicit rules can find the
	 * corresponding files.
	 */

			if(!suffix(nbuf,RCSsuf,nbuf)) 
				continue;
			if (RCSpref)
				p1 = ncat(fullname,RCSpref,-1);
			else
				p1 = fullname;
			ncat(p1,nbuf,-1);

			if(srchname(fullname) == NULL)
				makename(copys(fullname));
		}
	}

	if(endir != 0)
		*endir = SLASH;

	if(cldir) {
		closedir(dirf);
		dirf = NULL;
	}

	return(thisdbl);
}

/* stolen from glob through find */

amatch(s, p)
CHARSTAR s, p;
{
	register int cc, scc, k;
	int c, lc;

	scc = *s;
	lc = 077777;
	switch (c = *p)
	{

	case LSQUAR:
		k = 0;
		while (cc = *++p)
		{
			switch (cc)
			{

			case RSQUAR:
				if (k)
					return(amatch(++s, ++p));
				else
					return(0);

			case MINUS:
				k |= lc <= scc & scc <= (cc=p[1]);
			}
			if(scc==(lc=cc))
				k++;
		}
		return(0);

	case QUESTN:
	caseq:
		if(scc)
			return(amatch(++s, ++p));
		return(0);
	case STAR:
		return(umatch(s, ++p));
	case 0:
		return(!scc);
	}
	if(c==scc)
		goto caseq;
	return(0);
}

umatch(s, p)
register CHARSTAR s, p;
{
	if(*p==0)
		return(1);
	while(*s)
		if(amatch(s++,p))
			return(1);
	return(0);
}

#ifdef METERFILE
int meteron 0;	/* default: metering off */

meter(file)
CHARSTAR file;
{
	TIMETYPE tvec;
	CHARSTAR p, ctime();
	FILE * mout;
	struct passwd *pwd, *getpwuid();

	if(file==0 || meteron==0)
		return;

	pwd = getpwuid(getuid());

	time(&tvec);

	if( (mout=fopen(file,"a")) != NULL )
	{
		p = ctime(&tvec);
		p[16] = CNULL;
		fprintf(mout,"User %s, %s\n",pwd->pw_name,p+4);
		fclose(mout);
	}
}
#endif


/* look inside archives for notations a(b) and a((b))
	a(b)	is file member   b   in archive a
	a((b))	is entry point  _b  in object archive a
*/

static struct ar_hdr   arhead;
static struct ar_hdr11 arhead11;

static long arflen;
static long arfdate;
static char arfname[16];

char archmem[16];
char archname[64];		/* name of archive library */
int  ascarch;			/* flag indicating ascii archive format	*/

FILE *arfd;
long int arpos, arlen;

static struct exec objhead;

static struct nlist objentry;


TIMETYPE lookarch(filename)
register CHARSTAR filename;
{
	register int i;
	CHARSTAR p, q, send;
	char s[15];
	int nc, nsym, objarch;

	for(p = filename; *p!= LPAREN ; ++p);
	q = p++;

	if(*p == LPAREN)
	{
		objarch = YES;
		nc = 8;
		++p;
	}
	else
	{
		objarch = NO;
		nc = 14;
		for(i = 0; i < 14; i++)
		{
			if(p[i] == RPAREN)
			{
				i--;
				break;
			}
			archmem[i] = p[i];
		}
		archmem[++i] = 0;
	}
	*q = CNULL;
	copstr(archname, filename);
	i = openarch(filename);
	*q = LPAREN;
	if(i == -1)
		return(0);
	send = s + nc;

	for( q = s ; q<send && *p!=CNULL && *p!=RPAREN ; *q++ = *p++ );

	while(q < send)
		*q++ = CNULL;
	while(getarch())
	{
		if(objarch)
		{
			getobj();
			nsym = objhead.a_syms / sizeof(objentry);
			for(i = 0; i<nsym ; ++i)
			{
				fread(&objentry, sizeof(objentry),1,arfd);
				if( (objentry.n_type & N_EXT)
				   && ((objentry.n_type & ~N_EXT) || objentry.n_value)
#ifdef	BSD
				   && eqstr(objentry.n_un.n_name,s,nc))
#else
				   && eqstr(objentry.n_name,s,nc))
#endif
				{
					for(i = 0; i < 14; i++)
						archmem[i] = arfname[i];
					archmem[++i] = 0;
	out:
					clarch();
					return(arfdate);
				}
			}
		}

		else if( eqstr(arfname, s, nc))
			goto out;
	}

	clarch();
	return( 0L);
}


clarch()
{
	fclose( arfd );
}



openarch(f)
register CHARSTAR f;
{
	char magic[SARMAG];
	int word = 0;
	struct stat buf;

	if(stat(f, &buf) == -1)
		return(-1);
	arlen = buf.st_size;

	arfd = fopen(f, "r");
	if(arfd == NULL)
		return(-1);
	fread(&word, sizeof(word), 1, arfd);
	if(word == ARMAG11) {
		arpos = sizeof(word);
		ascarch = 0;
		arflen = 0;
		return(0);
	}
	fseek(arfd, 0L, 0);
	fread(magic, SARMAG, 1, arfd);
	if( eqstr(magic, ARMAG, SARMAG) ) {
		arpos = SARMAG;
		ascarch = 1;
		arflen = 0;
		return(0);
	}
	fatal1("%s is not an archive", f);
}



getarch()
{
	char	*s;
	long atol();
	arpos += (arflen +1) & ~1L; /* round archived file length up to even */
	if(arpos >= arlen)
		return(0);
	fseek(arfd, arpos, 0);
	if (ascarch) {
		fread(&arhead, sizeof(arhead), 1, arfd);
		arpos += sizeof(arhead);
		strncpy(arfname,arhead.ar_name,sizeof(arhead.ar_name));
		arflen = atol(arhead.ar_size);
		arfdate = atol(arhead.ar_date);
		for (s=arfname+sizeof(arhead.ar_name)-1;*s == ' ';s--)
			*s = CNULL;
	}
	else {
		fread(&arhead11, sizeof(arhead11), 1, arfd);
		arpos += sizeof(arhead11);
		strncpy(arfname,arhead11.ar11_name,sizeof(arhead11.ar11_name));
		arflen = arhead11.ar11_size;
		arfdate = arhead11.ar11_date;
	}
	return(1);
}


getobj()
{
	long int skip;

	fread(&objhead, sizeof(objhead), 1, arfd);
#ifdef	N_BADMAG
	if( N_BADMAG(objhead) )
#else
	if( objhead.a_magic != A_MAGIC1 &&
	    objhead.a_magic != A_MAGIC2 &&
	    objhead.a_magic != A_MAGIC3 )
#endif
			fatal1("%s is not an object module", arfname);
	skip = objhead.a_text + objhead.a_data;
#ifdef vax
	skip += objhead.a_trsize + objhead.a_drsize;
#else
	if(! objhead.a_flag )
		skip *= 2;
#endif
	fseek(arfd, skip, 1);
}


eqstr(a,b,n)
register CHARSTAR a, b;
register int n;
{
	register int i;
	for(i = 0 ; i < n ; ++i)
		if(*a++ != *b++)
			return(NO);
	return(YES);
}

/*
 * Try to find an RCS file corresponding to 'filename', and return
 *	the modified time of the RCS file.
 * If ch is non-null, then append it on the chain for later check-out
 */
time_t
getrcs(p, ch)
register struct nameblock *p;
struct chain **ch;
{
	struct stat sbuf;
	char temp[MAXPATH];
	register char *tail;
	register char *s;
	int headlen;
	struct	chain	*appendq();

	if ((tail = rindex(p->namep,'/')) == NULL) {
		headlen = 0;
		tail = p->namep;
	} else
		headlen = ++tail - p->namep;
	s = ncat(temp,p->namep,headlen);
	s = ncat(s,RCSdir,-1);
	*s++ = '/';
	s = ncat(s,tail,-1);
	ncat(s,RCSsuf,-1);
	if(stat(temp,&sbuf) < 0) {
		concat(p->namep,RCSsuf,temp);
		if(stat(temp,&sbuf) < 0)
			return(0);
	}
	p->RCSnamep = copys(temp);
	if (ch)
		*ch = appendq(*ch, p->namep);
	return(sbuf.st_mtime);
}

/*
 * Try to check-out the files specified in ch, if they do not
 *	already exist.  If rmflag is true, mark successful attempts
 *	for automatic deletion.
 * Try to make the modified time of the file the same as that of the
 *	RCS file.
 */
co(ch)
register struct chain *ch;
{
	register struct nameblock *p;
	register char *file;
	char	*RCSfile;
	struct	stat	sbuf;
	int	i;
	time_t tm[2];

	for ( ; ch; ch = ch->nextchain) {
	if ((file=ch->datap) == NULL || (p=srchname(file)) == NULL)
			continue;
		if (stat(file, &sbuf) == 0)
			continue;		/* don't do it again */

		RCSfile = p->RCSnamep;
		p->RCSnamep = NULL;
		setvar("@",file);
		setvar("<",RCSfile);
		i = docom(co_cmd);
		setvar("@",NULL);	/* so it doesn`t get deleted */
		if(i)
			continue;	/* docom() failed */
		/*
		 * since we succeeded, mark it for later deletion
		 */
		if (IS_ON(RMFLAG) && !isprecious(file))
			rmchain = appendq(rmchain, file);
		/*
		 * try to set modified time on file
		 */
		 if(stat(RCSfile,&sbuf) == 0) {
			tm[0] = time(0);
			tm[1] = sbuf.st_mtime;
			utime(file, tm);
		}
	}
}

/*
 * delete the files listed in rmchain
 */

rm()
{
	register struct chain *q;
	struct nameblock *p;
	register struct lineblock *lp;
	register struct shblock *sp;
	static int once = 0;
	
	if (once || (q = rmchain) == NULL)  /* only if we should */
		return;

	once = 1;
	if ((p = srchname(".CLEANUP")) == NULL)
		return;
	sp = NULL;
	for (lp = p->linep; lp ;lp=lp->nextline)
		if (sp = lp->shp)
			break;
	if (sp == NULL)			/* no or NULL .CLEANUP */
		return;
	setvar("?",mkqlist(q));
	docom(sp);
}

/*
 * Do the srchdir for RCS files. For a pattern a/b, it searches
 * a/RCS/b, without generating an error it it doesn`t exist.
 */
srchRCS(pat)
register char *pat;
{
	char	temp[MAXPATH];
	int	headlen;
	register char *tail, *s;

	if((tail = rindex(pat,'/')) == NULL) {
					/* quick search for "." */
		if(dotRCS) {
			s = ncat(temp,RCSdir,-1);
			*s++ = '/';
			ncat(s,pat,-1);
			srchdir(temp,NO,NULL,NO);
		}
		return;
	} else
		tail++;
	headlen = tail - pat;
	s = ncat(temp,pat,headlen);
	s = ncat(s,RCSdir,-1);
	*s++ = '/';
	ncat(s,tail,-1);
	srchdir(temp,NO,NULL,NO);
}
