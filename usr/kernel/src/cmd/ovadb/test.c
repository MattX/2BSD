main(){
	test();
}

test(){
	static int x = 0;

	if (x++<15) test();
	abort();
}
