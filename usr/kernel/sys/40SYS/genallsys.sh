#
cp ovunix rlunix
cp ovunix rpunix
cp ovunix rkunix
cp ovunix hkunix
cp ovunix xpunix
cp ovunix dvunix
# A terrible kludge!!!!
# If the system is changed then this constant (q) must be changed
# also. It is the value of f2 in the ?map
#
# Sorry, Bob Kridle (Perhaps someone will finish ovadb???
#
ovadb -w rlunix << 'EOF'
$> /dev/null
0176340>q
<q-060000>x
?m 0 0xfffff <x
$>
swapdev?w 04000
rootdev?w 04000
pipedev?w 04000
'EOF'
ovadb -w rpunix << 'EOF'
0254?w rpio
$> /dev/null
0176340>q
<q-060000>x
?m 0 0xfffff <x
$>
swapdev?w 0400
rootdev?w 0400
pipedev?w 0400
'EOF'
ovadb -w hkunix << 'EOF'
$> /dev/null
0176340>q
<q-060000>x
?m 0 0xfffff <x
$>
swapdev?w 05000
rootdev?w 05000
pipedev?w 05000
'EOF'
ovadb -w xpunix << 'EOF'
0254?w xpio
$> /dev/null
0176340>q
<q-060000>x
?m 0 0xfffff <x
$>
swapdev?w 06000
rootdev?w 06000
pipedev?w 06000
'EOF'
ovadb -w rkunix << 'EOF'
$> /dev/null
0176340>q
<q-060000>x
?m 0 0xfffff <x
$>
swapdev?w 0
rootdev?w 0
pipedev?w 0
swplo?W 4000
nswap?w 872
'EOF'
ovadb -w dvunix << 'EOF'
0254?w dvhpio
$> /dev/null
0176340>q
<q-060000>x
?m 0 0xfffff <x
$>
swapdev?w 07000
rootdev?w 07000
pipedev?w 07000
'EOF'
