/*
 * fake the rename call.
 */

rename(from,to)
char	*from,
	*to;
{
	unlink(to);
	link(from,to);
	unlink(from);
}
