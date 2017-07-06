#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/ioctl.h>
#include "led.h"

#define	LED_Device_PATH		/dev/led


int main(int argc, char *argv[])
{
	int fd;
	int ret = -1;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s dev_file\n", argv[0]);
		return -1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
	{
		perror("open");
		return -1;
	}
	printf("open: fd = %d\n", fd);

	printf("main: ioctl all LED Off\n");
	ret = ioctl(fd, LED_ALL_OFF, NULL);
	if (ret < 0)
	{
		perror("ioctl: All LED Off\n");
		return -1;
	}

	close(fd);

	return 0;
}
