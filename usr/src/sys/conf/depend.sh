#! /bin/sh
#
# dependency shell file

FILE=$1
shift
ed $FILE << 'END_OF_EDIT'
/^# DO NOT DELETE THIS LINE/+1,$d
w
q
END_OF_EDIT

grep '^#include' $* /dev/null | \
sed \
	-e '/".*"/s|:[^"]*"\([^"]*\)".*|: \1|' \
	-e '/<.*>/s|:[^<]*<\([^>]*\)>.*|: ${I}/\1|' | \
awk -F: '{if ($1 != cfil) { if (cfil != "") \
		{ \
		print rec; \
		printf "\t${C} %s\n", cfil; \
		printf "\t${E} %s\n", cfil; \
		printf "\t${A} %s\n", cfil; \
		printf "\trm -f %s\n\n", cfil; \
		} \
	    printf "%s: %s\n", $1, $1; cfil = $1; rec = $0; next} \
	else { if (length(rec $2) > 80) {print rec; rec = $0;} \
		else rec = rec $2 } } \
	END { \
		printf "\t${C} %s\n", cfil; \
		printf "\t${E} %s\n", cfil; \
		printf "\t${A} %s\n", cfil; \
		printf "\trm -f %s\n\n", cfil; \
	    }' | \
sed \
	-e '/^\.\.\/[^ 	/]*\/\([^. 	/]*\.\)/s//\1/' \
	-e '/\.c:/s//.o:/' \
	-e '/^	${[EA]/s|../[^/]*/||' \
	-e '/^	${E/s| \([^/]*\)\.c| \1.s|' \
	-e '/^	${A} /s|{A} \([^/]*\)\.c|{A} \1.o \1.s|' \
	-e '/^	rm -f/s|../[^/]*/||' \
	-e '/^	rm -f/s| \([^/]*\)\.c| \1.s|' >> $FILE

echo '# DEPENDENCIES MUST END HERE' >> $FILE
echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> $FILE
