#include "sdef.h"
#include "d.h"
#include "v.h"
#include "tw.h"
/*
sroff10.c

Device interfaces
*/

extern int lss;
extern int nlss;
extern char obuf[];
extern char *obufp;
extern int xfont;
extern int esc;
extern int lead;
extern int oline[];
extern int *olinep;
extern int sps;
extern int Em;
extern int ics;
extern int ttysave;
extern struct sgttyb ttys;
extern char fontlib[];
extern int ptid;
extern int waitf;
extern int pipeflg;
extern int tabtab[];
extern int xxx;
extern int dotF;
extern int physfont;
#ifndef ADJ
extern int spacesz;	/* width of space in physfont */
#endif
int spfont;	/* flag to reset font after printing special char, mjk */
extern int bdtab[];
extern int smnt;
extern int sbold;
extern int chbits;
extern int fontab[];
extern int fontab1[];
extern char *codetab[];
extern int ce;
extern int ad;
extern int pl;
extern int brflg;
int bdmode;
int plotmode;
int ulmode = 0;	/* Underline toggle, stan */
int steps;	/* Number of 3.5 mills for Sanders leading, stan */
ptinit(){
	register i, j;
	register char **p;
	char *q;
	struct fonts *fp;
	extern char *setbrk();

	if (ptid)
		ioctl(ptid,TIOCEXCL,(struct sgttyb *)0);
	if (dotF) {
		if((i = open(fontlib,0)) < 0){
			prstr("Cannot open ");
			prstr(fontlib);
			prstr("\n");
			exit(-1);
		}
		lseek(i,(long)8*sizeof(int),0);	/* past header */
		read(i,(char *)&fonts[0],
			j=sizeof(int) *
			    (int)((int *)&fontab1[0200-32]-(int *)&fonts[0])+1);
	}

	mchbits();
	ics = Em*2;
	for(i=0; i<16; i++)tabtab[i] = DTAB * (i+1);
/*	setpl();	temporarily deleted */
#ifdef PGOFF
	setpo();
#endif
#ifdef ADJ
	setll();
	setin();
	setad();
#endif
	for (i=0; i<NFONTS; i++) {
	    fp = &fonts[i];
	    if (fp->mount[0]) {
		oputs("\033d");
		oput(0100+(fp->phys & FONTMSK));
		oputs(fp->mount);
	    }
	}
}
twdone(){
	obufp = obuf;
	oputs(t.twrest);
	flusho();
	if(pipeflg){
		close(ptid);
		wait(&waitf);
	}
	if(ttysave != -1) {
		ttys.sg_flags = ttysave;
		stty(1, &ttys);
	}
}
ptout(i)
int i;
{
	register pos;
	*olinep++ = i;
	if(olinep >= &oline[LNSIZE])olinep--;
	if((i&CMASK) != '\n')return;
	olinep--;
	lead += dip->blss + lss - t.Newline;
	pos = v.nl - nlss;
	if (pos < TOP_MARG) {
		lead -= TOP_MARG - pos;		/* already at margin */
		if (olinep>oline && lead<0)
			lead = 0;	/* ignore motion in margin */
	}
	dip->blss = 0;
	esc = 0;
	if(olinep>oline){
		move();
		ptout1();
#ifdef ADJ
		if (i & EBIT)
			oputs(t.twfp);
		else
#endif
		oputs(t.twnl);
	}else{
		lead += t.Newline;
		if (pos<TOP_MARG && lead<0) lead = 0;
		move();
	}
	lead += dip->alss;
	dip->alss = 0;
	olinep = oline;
}
ptout1()
{
	register i, k;
	register char *codep;
	register struct fonts *fp;
	extern char *plot();
	extern int oput();
	int *q, w, j, phyw, bd;

	for(q=oline; q<olinep; q++){
	if((i = *q) & MOT){
		j = i & ~MOTV;
		if(i & NMOT)j = -j;
		if(i & VMOT)lead += j;
		else esc += j;
		continue;
	}
	if((k = (i & CMASK)) < 040){
		continue;
	}
	xfont = getfont(i);
	fp = &fonts[xfont];
	j = fp->phys & FONTMSK;
	if (k > 0177) {
		codep = codetab[k-0200];
		if (*codep & FONTMSK) j = *codep & FONTMSK;
		bd = (xfont==sbold) ? bdtab[smnt] : 0;
	} else {
		codep = (char *) 0;
		bd = bdtab[xfont];
	}
	if ((j != physfont) || spfont) {
		oputs("\033a");	/* change font */
		oput(0100+j);
		physfont = j;
#ifndef ADJ	/* don't need spacesz if move doesn't use spaces */
		if (fp->phys & PROPOR)
			spacesz = (fontab[0] & WDMASK);
		else spacesz = fp->Char;
#endif
	}
	if(esc || lead) move();
	if(bdmode && (bdmode != bd)){
		oputs(t.bdoff);
		bdmode = 0;
	}
	if(!bdmode && bd){
		oputs(t.bdon);
		oput((bdmode=bd) + 0140);
	}
	if ((i & ULMODE)  && !ulmode) {
        	oputs(t.ulon);
		ulmode++;
	}
	else if (ulmode && !(i & ULMODE)) {
		oputs(t.uloff);
		ulmode = 0;
	}
	if (codep == (char *)0) {
		*obufp++ = (char) k;
		if(obufp == (obuf + OBUFSZ)) flusho();
		spfont = 0;
	} else {
	    spfont = *codep++ & 0100;	/* reset font after this char */
	    while(*codep != 0){
		if(*codep & 0200){
			codep = plot(codep);
			oputs(t.plotoff);
			oput(' ');
		}else{
			if(plotmode)oputs(t.plotoff);
			*obufp++ = *codep++;
			if(obufp == (obuf + OBUFSZ)) flusho();
		}
	    }
	}
	if (i & ZBIT)
		esc -= getcw(i);
	}
}
char *plot(x)
char *x;
{
	register int i;
	register char *j, *k;

	if(!plotmode)oputs(t.ploton);
	k = x;
	if((*k & 0377) == 0200)k++;
	for(; *k; k++){
		if(*k & 0200){
			if(*k & 0100){
				if(*k & 040)j = t.up; else j = t.down;
			}else{
				if(*k & 040)j = t.left; else j = t.right;
			}
			if(!(i = *k & 037))return(++k);
			while(i--)oputs(j);
		}else oput(*k);
	}
	return(k);
}
move(){
    /* lead is in chunks of 2047 steps for vertical spacing */
    /* esc is in chunks of 2047 steps for reverse horizontal motion */
	register k, steps;
	int dt, DidLead;
	DidLead = 0;
    if(ulmode) {
	ulmode = 0;
	oputs(t.uloff);
    }
    if(lead){
        if (v.nl >= pl) {
	    oput('\f');
	} else {
	    DidLead++;		/* did vertical motion */
	    steps = lead/3.5;	/* 3.5 mil increments */
	    if (steps < 0) steps = -steps;
	    for (; steps > 2047; steps -= 2047)
		oputs((lead < 0) ? "\033o`A" : "\033o_\177");
	    if (steps) {  /*travel rest of the way*/
	        oputs("\033o");
		donum2(oput,(lead<0)?-steps:steps);
	    }
	}
	lead = 0;
    }
    if(esc > 0) {
#ifndef ADJ	/* spaces get adjusted by the sanders */
        k = esc/spacesz; esc = esc%spacesz;
        while(k--)oput(' ');
#else
        for( ; esc > 2047; esc -= 2047) oputs("\033l_\177");
#endif
        if(esc > 0) {
            oputs("\033l");
	    donum2(oput,esc);
        }
    }
    if(esc < 0) {
	if (!DidLead)
	        oputs("\033o@A");
        for( ; esc < -2047;esc += 2047) oputs("\033l`A");
	if (esc < 0) {
            oputs("\033l");
	    donum2(oput,esc);
	}
	if (!DidLead)
        	oputs("\033o\177\177");
    }
    esc = 0;
}
ptlead(){move();}

donum1(output,i)		/* output int in 1-byte sanders ascii */
int (*output)();
register i;
{
	if (i>=0)
		(*output)((i+0100) | EBIT);
	else
		(*output)((64+i+0100) | EBIT);
}

donum2(output,i)		/* output int in 2-byte sanders ascii */
int (*output)();
register i;
{
	if (i<0)
		i = 4096 + i;
	(*output)(((i/64) + 0100) | EBIT);
	(*output)(((i%64) + 0100) | EBIT);
}
donum3(output,i)		/* output int in 3-byte sanders ascii */
int (*output)();
register i;
{
	long l;
	if (i>=0)
		l = i;
	else
		l = 65536L + (long)i;
	(*output)(((int)(l/4096) + 0100) | EBIT);
	donum2(output,(int)(l%4096));
}
