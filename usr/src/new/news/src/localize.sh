rm -f Makefile
cp Makefile.dst Makefile
chmod u+w Makefile
ed - Makefile  <<'EOF'
g/^#MKSTR /s///
g/^#V7 /s///
g/^#BSD4_3 /s///
g/^#USG /d
g/^#VMS /d
g/^#BSD4_1 /d
g/^#BSD4_2 /d
g/^#NNTP /d
g/^#NFSCLIENT /d
g/^#EXCELAN /d
g/^#RESOLVE /d
g/#NOTVMS/s/#NOTVMS.*//
/^UUXFLAGS/s/-r -z/-r -z -n -gd/
g/^LIBDIR/s;/usr/lib/news;/usr/new/lib/news;
g/^BINDIR/s;/usr/bin;/usr/new;
g/^LNRNEWS =/s/ln/ln -s/
g/^SCCSID/d
g/^CFLAGS =/s/CFLAGS =/CFLAGS = -O/
g/LFLAGS =/s/$/ -i/
g/^VOBJECTS/.,.+1t.-1
-1s/VOBJECTS/VOBJECTDEP/
g/^vnews/s/VOBJECTS/VOBJECTDEP/
g/^VOBJECTS/s/readnews.o rfuncs.o rfuncs2.o rextern.o process.o/-Z readnews.o rfuncs2.o rextern.o process.o -Z rfuncs.o /
+1s/$(OBJECTS) visual.o virtterm.o rpathinit.o/-Z virtterm.o -Y $(OBJECTS) visual.o rpathinit.o/
g/^COMMANDS/s/readnews checknews postnews vnews/checknews postnews/
w
q
EOF
rm -f defs.h
cp defs.dist defs.h
chmod u+w defs.h
ed - defs.h <<'EOF'
/MYORG/c
#define MYORG	"Ed's Peanut Farms, Inc."
.
/SPOOLNEWS/s/^...//
/MKSTR/s/^...//
/N_UMASK/s/000/022/
/DFTXMIT/s/-z/-z -gd/
/UXMIT/s/-z/-z -gd/
/INTERNET/s/^...//
/GHNAME/s/^...//
/DOXREFS/s/^...//
/BSD4_2/s/^...//
/SENDMAIL/s/^...//
w
q
EOF
echo "Make sure that /usr/new is in PATH in /usr/lib/uucp/L.cmds; read ../misc/L.cmds"
