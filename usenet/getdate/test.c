#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <time.h>
char *ctime();
struct tm *localtime();
char *timezone();
time_t getdate();
struct timeb now;
main() {
	time_t ret; char buf[300];
	register char *p;
	register struct tm *l;

#ifndef TNULL
	ftime(&now);
#endif
	while(fgets(buf, 300, stdin) != NULL) {
		buf[strlen(buf)-1] = '\0';
#ifdef TNULL
		ftime(&now);
		ret = getdate(buf, (struct timeb *) NULL);
#else
		ret = getdate(buf, &now);
#endif
		if (ret >= 0) {
			p = ctime(&ret);
			p[strlen(p)-1] = '\0';
			l = localtime(&ret);
			printf("%D  %s %s\n", ret, p,
				timezone(now.timezone, l->tm_isdst));
		}
		else printf("%D\n", ret);
	}
}
