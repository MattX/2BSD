#   block 0
procedure george
real x
x = 2
if(x > 2)
	{	# new block
	integer x	# a different variable
	do x = 1,7
		write(,x)
	}	# end of block
end	# end of procedure, return to block 0
