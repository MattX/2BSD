as microdum.s
strip a.out
dd if=a.out of=microdum bs=16 skip=1
as rauboot.s
strip a.out
dd if=a.out of=t0 bs=16 skip=1
cat microdum t0>t1
dd if=t1 of=rauboot conv=sync
rm microdum a.out t0 t1
