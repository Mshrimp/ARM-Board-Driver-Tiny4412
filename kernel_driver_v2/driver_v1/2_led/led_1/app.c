#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/ioctl.h>

#include "common.h"

int main(int argc, char *argv[])
{
	int fd;
	int ret = -1;
	int cmd = -1;

	if (argc != 2)
	{
		fprintf(stderr, "App: Usage: %s /dev/xxx\n", argv[0]);
		return -1;
	}

	fd = open(argv[1], O_RDWR);
	ERRP(fd < 0, "App", "open dev", goto ERR1);
	printf("App: open, fd = %d\n", fd);

	while(1)
	{
		printf("App：Please input the cmd:\n");
		scanf("%d", &cmd);
		ret = write(fd, &cmd, sizeof(int));
		ERRP(ret < 0, "App", "write failed", goto ERR2);
		printf("App: cmd = %d, ret = %d\n", cmd, ret);
	}

	close(fd);

	return 0;
ERR2:
ERR1:
	return -1;
}
