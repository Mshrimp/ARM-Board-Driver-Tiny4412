#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/ioctl.h>

#include "common.h"
#include "key.h"

int main(int argc, char *argv[])
{
	int fd;
	unsigned char key_vals[KEY_TOTLE] = { 0 };
	int ret = -1;
	int i = 0;

	if (argc != 2)
	{
		fprintf(stderr, "App: Usage: %s dev_xxx\n", argv[0]);
		return -1;
	}

	fd = open(argv[1], O_RDWR);
	ERRP(fd < 0, "App", "open dev", goto ERR1);
	printf("App: open, fd = %d\n", fd);

	while(1)
	{
	}

	close(fd);

	return 0;
ERR2:
ERR1:
	return -1;
}
