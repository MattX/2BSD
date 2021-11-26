#####
# Makefile: added rauboot from 2.10 BSD
# 14.10.2021, Peter Klapper
#

all:	hkuboot hpuboot rkuboot rluboot rmuboot rpuboot \
	rauboot1 rauboot2 rauboot3

hkuboot: hkuboot.s
	as -o hkuboot.o hkuboot.s
	size hkuboot.o
	strip hkuboot.o
	dd if=hkuboot.o of=hkuboot bs=8w skip=1
	rm hkuboot.o

hpuboot: hpuboot.s
	as -o hpuboot.o hpuboot.s
	size hpuboot.o
	strip hpuboot.o
	dd if=hpuboot.o of=hpuboot bs=8w skip=1
	rm hpuboot.o

rkuboot: rkuboot.s
	as -o rkuboot.o rkuboot.s
	size rkuboot.o
	strip rkuboot.o
	dd if=rkuboot.o of=rkuboot bs=8w skip=1
	rm rkuboot.o

rluboot: rluboot.s
	as -o rluboot.o rluboot.s
	size rluboot.o
	strip rluboot.o
	dd if=rluboot.o of=rluboot bs=8w skip=1
	rm rluboot.o

rmuboot: rmuboot.s
	as -o rmuboot.o rmuboot.s
	size rmuboot.o
	strip rmuboot.o
	dd if=rmuboot.o of=rmuboot bs=8w skip=1
	rm rmuboot.o

rpuboot: rpuboot.s
	as -o rpuboot.o rpuboot.s
	size rpuboot.o
	strip rpuboot.o
	dd if=rpuboot.o of=rpuboot bs=8w skip=1
	rm rpuboot.o

rauboot1: rauboot1.s
	as -o rauboot1.o rauboot1.s
	size rauboot1.o
	strip rauboot1.o
	dd if=rauboot1.o of=rauboot1 bs=8w skip=1
	rm rauboot1.o

rauboot2: rauboot2.s
	as -o rauboot2.o rauboot2.s
	size rauboot2.o
	strip rauboot2.o
	dd if=rauboot2.o of=rauboot2 bs=8w skip=1
	rm rauboot2.o

rauboot3: rauboot3.s
	as -o rauboot3.o rauboot3.s
	size rauboot3.o
	strip rauboot3.o
	dd if=rauboot3.o of=rauboot3 bs=8w skip=1
	rm rauboot3.o

.DEFAULT:
	as -o $@.o $@.s
	size $@.o
	strip $@.o
	dd if=$@.o of=$@ bs=8w skip=1
	rm $@.o

clean:
	rm -f hkuboot hpuboot rkuboot rluboot rmuboot rpuboot \
	rauboot1 rauboot2 rauboot3
