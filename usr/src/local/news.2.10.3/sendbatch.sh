: '@(#)sendbatch.sh	1.4	2/12/85'
for rmt in $*
do
	: make sure $? is 0
	while test $? -eq 0 -a \( -s BATCHDIR/$rmt -o -s BATCHDIR/$rmt.work \)
	do
		LIBDIR/batch BATCHDIR/$rmt 50000 | \
			if test -s BATCHDIR/$rmt.cmd
			then
				BATCHDIR/$rmt.cmd
			else
				uux - UUXFLAGS $rmt!rnews
			fi
	done
done
