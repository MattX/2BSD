: '@(#)csendbatch	1.7	4/5/85'
cflags='-q'
for rmt in $*
do
	case $rmt in
	-[bBC]*)	cflags="$cflags $rmt";;
	*)
	while test $? -eq 0 -a \( -s BATCHDIR/$rmt -o -s BATCHDIR/$rmt.work \)
	do
		(echo "#! cunbatch"; LIBDIR/batch BATCHDIR/$rmt 100000 | LIBDIR/compress $cflags) | \
			if test -s BATCHDIR/$rmt.cmd
			then
				BATCHDIR/$rmt.cmd
			else
				uux - UUXFLAGS $rmt!rnews
			fi
	done;;
	esac
done
