
rm -f Makefile
cp Makefile.dst Makefile
chmod u+w Makefile
ed Makefile  <<'EOF'
/UUXFLAGS/s/-r -z/-r -z -n -gd/
/#V7 LFLAGS/s/$/-i/
g/^#V7 /s///
w
q
EOF
rm -f defs.h
cp defs.dist defs.h
chmod u+w defs.h
ed - defs.h <<'EOF'
/ROOTID/s/10/13/
/N_UMASK/s/000/002/
/DFTXMIT/s/-z/-z -gd/
/UXMIT/s/-z/-z -gd/
/INTERNET/s;/\* ;;
/DOXREFS/s;/\* ;;
/SENDMAIL/s;/\* ;;
/MYORG/s/Frobozz Inc., St. Louis/Center for Seismic Studies, Arlington, VA/
/NICENESS/s;/\* ;;
w
q
EOF
