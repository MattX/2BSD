umask	022

if ( -e ~/.login-l ) then
	source ~/.login-l
endif

# Set some standard terminal features.  Then, if this is a dialup login or
# the terminal type just isn't known, get the terminal type from the user
# (default being a ~/.lastdialup).

if ($TERM == dialup || $TERM == unknown || $TERM == network || $TERM == switch || $TERM == su) then
	if ( ! -e ~/.lastdialup ) then
		echo  dumb >~/.lastdialup
	endif

	set  default_term = `cat ~/.lastdialup`
	echo  -n "TERM = ($default_term) "

	stty $STTY
	set   term = $<

	if  ($TERM == '')  then
		set  term = $default_term
	else
		echo  $TERM >~/.lastdialup
	endif

	unset default_term
endif

source ~/.tset
