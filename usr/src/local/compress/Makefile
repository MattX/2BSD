CFLAGS=	-O -Dpdp11

compress:
	cc $(CFLAGS) -o compress compress.c

install: compress
	install -s -o bin -g bin -m 755 compress /usr/local/compress
	-rm -f /usr/local/uncompress /usr/local/zcat
	ln /usr/local/compress /usr/local/uncompress
	ln /usr/local/compress /usr/local/zcat
	install -c -o bin -g bin -m 755 zmore /usr/local/zmore
	install -c -o bin -g bin -m 644 compress.1l /usr/man/man1/compress.1l
	install -c -o bin -g bin -m 644 zmore.1l /usr/man/man1/zmore.1l

clean:
	-rm -f *.o compress a.out errs
