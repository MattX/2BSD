as produm.s
strip a.out
dd if=a.out of=produm bs=16 skip=1
as rauboot.s
strip a.out
dd if=a.out of=t0 bs=16 skip=1
cat produm t0>t1
dd if=t1 of=rauboot conv=sync
rm produm a.out t0 t1
