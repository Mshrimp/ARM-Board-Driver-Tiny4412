#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <ioctl.hh>


int main(void)
{
	int fd;

	fd = open(DEV, O_RDWR);
	if (fd < 0)
	{
		perror("open");
		exit(1);
	}

	

	return 0;
}

