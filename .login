#csh login file
if (-x /usr/ucb/tset) then
	set noglob; eval `/usr/ucb/tset -s -m du:\?adm3a`; unset noglob
endif
if (`tty` == /dev/console) then
	stty cr2 nl2 new
else
	stty newcrt
endif
setenv HOME /
setenv SHELL /bin/csh
setenv PATH /usr/ucb:/bin:/usr/bin:/etc:.
