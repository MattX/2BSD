if  ($?prompt)  then
	set	history	= 30
	
	if ( -e ~/.cshrc-l ) then
		source ~/.cshrc-l
	endif
	
	alias	setprompt 'set noglob; set dirs = `dirs`; set prompt = "$ston${WHOAMI}@${HOSTNAME}:$dirs[1]$stoff "; unset dirs noglob'
	
	if  (! $?SHLVL)  then
		setenv	SHLVL  1
		# .login will perform a setprompt after ston/off have been
		# defined
	else
		setenv	SHLVL  `expr $SHLVL + 1`
		setprompt
	endif
	
	alias	cd	'cd    \!*;  setprompt'
	alias	pushd	'pushd \!* >& /dev/null;  setprompt'
	alias	popd	'popd  \!* >& /dev/null;  setprompt'
	alias	pd	pushd
endif
