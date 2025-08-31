procedure walk(first)	# print out an expression tree

integer first		# index of root node
integer currentnode
integer stackdepth
common(nodes) struct
	{
	character(1) op
	integer leftp, rightp
	real val
	} tree(100)	# array of structures

struct
	{
	integer nextstate
	integer nodep
	} stackframe(100)

define NODE	tree(currentnode)
define STACK	stackframe(stackdepth)

#   nextstate values
define DOWN	1
define LEFT	2
define RIGHT	3

#   initialize stack with root node
stackdepth = 1
STACK.nextstate = DOWN
STACK.nodep = first

while( stackdepth > 0 )
	{
	currentnode = STACK.nodep
	select(STACK.nextstate)
		{
		case DOWN:
			if(NODE.op == " ")   # a leaf
				{
				outval( NODE.val )
				stackdepth -= 1
				}
			else	{   # a binary operator node
				outch( "(" )
				STACK.nextstate = LEFT
				stackdepth += 1
				STACK.nextstate = DOWN
				STACK.nodep = NODE.leftp
				}

		case LEFT:
			outch( NODE.op )
			STACK.nextstate = RIGHT
			stackdepth += 1
			STACK.nextstate = DOWN
			STACK.nodep = NODE.rightp

		case RIGHT:
			outch( ")" )
			stackdepth -= 1
		}
	}
end
