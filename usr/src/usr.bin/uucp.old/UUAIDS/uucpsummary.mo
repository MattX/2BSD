for i in `grep -l UUCP /usr/msgs/*`
do
sed '1,/^Site/d
/^[ ]*$/,$d
s!/! !g
s/  */ /g
s/\[[0-9]*\]//
s/%//
' $i
done | sort |
awk '
BEGIN {
	printf("\n\t\t\t\tUUCP Traffic Summary\n\n")
	printf("\t\t\t\tFor the previous 30 days\n\n")
	printf("\t       Calls     Minutes      Files          Bytes      Effective Unused\n")
	printf("Site          To/From    To/From     To/From      Sent/Received  Baudrate Bwidth\n")
}
{
	if ($10 < 1200) {
		phonecallsto += $2
		phonecallsfrom += $3
		phoneminto += $4
		phoneminfrom += $5
		phonefilesto += $6
		phonefilesfrom += $7
		phonebytesto += $8
		phonebytesfrom += $9
		phonebaudrate += $10
		phonepercent += $11
		phonen++
	} else {
		arpacallsto += $2
		arpacallsfrom += $3
		arpaminto += $4
		arpaminfrom += $5
		arpafilesto += $6
		arpafilesfrom += $7
		arpabytesto += $8
		arpabytesfrom += $9
		arpabaudrate += $10
		arpapercent += $11
		arpan++
	}
}
name == $1 {
	callsto += $2
	callsfrom += $3
	minto += $4
	minfrom += $5
	filesto += $6
	filesfrom += $7
	bytesto += $8
	bytesfrom += $9
	baudrate += $10
	percent += $11
	n++
}
name != $1 {
	if (NR > 1)
		printf("%-12s %4d/%-4d %5d/%-5d %5d/%-5d %9d/%-9d %6d %5d%%\n", \
		name, callsto, callsfrom, minto, minfrom, filesto, \
		filesfrom, bytesto, bytesfrom, baudrate/n, percent/n)

	callsto = $2
	callsfrom = $3
	minto = $4
	minfrom = $5
	filesto = $6
	filesfrom = $7
	bytesto = $8
	bytesfrom = $9
	baudrate = $10
	percent = $11
	n = 1
	name = $1
}
END {
	printf("%-12s %4d/%-4d %5d/%-5d %5d/%-5d %9d/%-9d %6d %5d%%\n", \
		name, callsto, callsfrom, minto, minfrom, filesto, \
		filesfrom, bytesto, bytesfrom, baudrate/n, percent/n)
	printf("------------ ----/---- -----/----- -----/----- ---------/--------- ------  -----\n")
	printf("%-12s %4d/%-4d %5d/%-5d %5d/%-5d %9d/%-9d %6d %5d%%\n", \
		"Arpanet", arpacallsto, arpacallsfrom, arpaminto, arpaminfrom, \
		arpafilesto, arpafilesfrom, arpabytesto, arpabytesfrom, \
		arpabaudrate/arpan, arpapercent/arpan)
	printf("%-12s %4d/%-4d %5d/%-5d %5d/%-5d %9d/%-9d %6d %5d%%\n", \
		"Phone", phonecallsto, phonecallsfrom, phoneminto, \
		phoneminfrom, phonefilesto, phonefilesfrom, phonebytesto, \
		phonebytesfrom, phonebaudrate/phonen, phonepercent/phonen)
	printf("%-12s %4d/%-4d %5d/%-5d %5d/%-5d %9d/%-9d %6d %5d%%\n", \
		"Total", arpacallsto + phonecallsto, \
		arpacallsfrom + phonecallsfrom, arpaminto + phoneminto, \
		arpaminfrom + phoneminfrom, arpafilesto + phonefilesto, \
		arpafilesfrom + phonefilesfrom, arpabytesto + phonebytesto, \
		arpabytesfrom + phonebytesfrom, \
		(arpabaudrate + phonebaudrate)/(phonen +arpan), \
		(arpapercent + phonepercent)/(phonen +arpan))
}'
