as rauboot.s
strip a.out
dd if=a.out of=t0 bs=16 skip=1
dd if=t0 of=rauboot conv=sync
rm a.out t0 
