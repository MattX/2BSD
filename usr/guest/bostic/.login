set noglob
setenv SHELL /bin/csh
eval `tset -e"^?" -Q -s -m network:?xterm`
unset noglob
stty kill "^X" intr "^C" crt -tostop
if ($TERM != "dialup")then
	stty nohang
endif
setenv SHELL /bin/sh
msgs -q -f
echo
/usr/ucb/ruptime -l
echo
/usr/games/fortune -a
