# @(#)Makefile	4.19 (Berkeley) 7/31/83

LIB =   # the pathname of lib directory

OBJS =  access.o cerror.o chdir.o chmod.o chown.o chroot.o \
	close.o creat.o dup.o dup2.o \
	fchmod.o fchown.o fcntl.o flock.o fork.o \
	fstat.o fsync.o ftruncate.o \
	link.o lseek.o lstat.o mkdir.o mknod.o mount.o \
	open.o read.o readlink.o rename.o rmdir.o \
	stat.o symlink.o truncate.o \
	umask.o umount.o unlink.o write.o

.c.o:
	/lib/cpp -E $*.c | ${AS} -o $*.o
	-ld -x -r $*.o
	mv a.out $*.o

libra.a: ${OBJS}
	ar cru ${LIB}/libra.a ${OBJS}
	ranlib ${LIB}/libra.a

