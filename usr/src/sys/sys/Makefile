default:
	@echo "Use \"make tags\" to rebuild the tags file."

tags:	*.c
	-mv tags tags.old
	ctags -w *.c
	-rm -f tags.old
