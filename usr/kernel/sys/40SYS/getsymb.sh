#
# A terrible kludge!!!!
# If the system is changed then this constant (q) must be changed
# also. It is the value of f2 in the ?map
#
# Sorry, Bob Kridle (Perhaps someone will finish ovadb???)
#
ovadb ovunix << 'EOF'
$> /dev/null
0176340>q
<q-060000>x
<x-040>z
?m 0 0xfffff <x
$>
$m
0="		Default Vect	Contents	Bdev/Cdev""
tmio="tmio		0224		"o"	3/12"
htio="htio		0230		"o"	7/15"
tsio="tsio		0234		"o"	13/26"
rkio="rkio		0220		"o"	0/9"
hkio="hkio		0210		"o"	10/25"
rlio="rlio		0320		"o"	8/18"
rpio="rpio		0254		"o"	1/14"
dvhpio="dvhpio		0260		"o"	14/23"
xpio="xpio		0264		"o"	12/20"
dzin="dzin		0300		"o"	-/22"
dzou="dzou		0304		"o"	-/22"
lpou="lpou		0320		"o"	-/2"
<x="Offset of data in load image "o
<z="Offset of data in loaded core image "o
xp_type="Virtual address of xp_type = "o
xp_type+<z="Physical address of xp_type = "o
xp_type+<x="Load image address of xp_type = "o
dz_addr="Virtual address of dz_addr = "o
dz_addr+<z="Physical address of dz_addr = "o
dz_addr+<x="Load image address of dz_addr = "o
dz_addr?"Value of dz_addr[0] = "o
rootdev?"Rootdev = "o
swapdev?"Swapdev = "o
swplo?"Swplo = "D "(512 block offset)
nswap?"Nswap = "d "(512 blocks)"
'EOF'
