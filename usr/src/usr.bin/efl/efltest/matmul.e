procedure matmul(a,b,c, m,n,p)
integer i, j, k, m, n, p
long real a(m,n), b(n,p), c(m,p)

do i = 1,m
do j = 1,p
	{
	c(i,j) = 0
	do k = 1,n
		c(i,j) += a(i,k) * b(k,j)
	}
end
