#define	ARMAG11	0177545
struct	ar_hdr11 {
	char	ar11_name[14];
	long	ar11_date;
	char	ar11_uid;
	char	ar11_gid;
	int	ar11_mode;
	long	ar11_size;
};
