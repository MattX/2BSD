unset noglob
set path=(/etc /usr/ucb /bin /usr/bin /usr/local /usr/new)
umask 26
stty dec erase ^H kill ^U eof ^D
set prompt="[\!] root--> "

if ( "$TERM" == dialup || "$TERM" == network) then
  setenv TERM vt100
  echo -n "Terminal ($TERM)? "
  set argv = "$<"
  if ( "$argv" != '' ) setenv TERM "$argv"
endif

biff y
