#define UBMAP	((physadr)0170200)
typedef	struct { int r[1]; } *	physadr;
extern int cputype;
ubmapset()
{
	unsigned register i;

	if(cputype == 70)
	for(i=0; i<62; i+=2) {
		UBMAP->r[i] = i<<12;
		UBMAP->r[i+1] = i>>4;
	}
}
