#	@(#)Makefile	4.3	(2.11BSD)	2025/11/29
#
CFLAGS=	-O
LDFLAGS= -i

quiz: quiz.c
	cc ${LDFLAGS} ${CFLAGS} -o quiz quiz.c

install: quiz quiz.k
	install -c -s -m 755 -o root quiz /usr/games/quiz
	-rm -rf /usr/games/lib/quiz.k
	cp -pr quiz.k /usr/games/lib
	chmod 755 /usr/games/lib/quiz.k

clean:
	rm -f a.out core *.s *.o quiz

depend:	quiz.c
	mkdep ${CFLAGS} quiz.c
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

quiz.o: quiz.c /usr/include/stdio.h /usr/include/signal.h

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
