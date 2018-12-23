#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/ioctl.h>
#include <signal.h>

#include "key.h"

#define	DEV_PATH	"/dev/xxx"

int fd = 0;

void signal_func(int sinnum)
{
	unsigned long key_val = 0;
	int ret = -1;

	ret = read(fd, &key_val, sizeof(key_val));
	if (ret < 0) {
		printf("App: read dev file failed, ret = %d\n", ret);
		return;
	}

	//printf("App: key_val: 0x%X\n", key_val);

	if (key_val & 0x80) {
		printf("Button %d down\n", key_val & 0x0F);
	} else {
		printf("Button %d up\n", key_val & 0x0F);
	}
}

int main(int argc, char *argv[])
{
	unsigned long oflags = 0;
	int ret = -1;
	int i = 0;

	if (argc != 2)
	{
		fprintf(stderr, "App: Usage: %s dev_file\n", argv[0]);
		return -1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		printf("App: open dev failed\n");
		goto ERR1;
	}

	printf("App: open, fd = %d\n", fd);

	signal(SIGIO, signal_func);

	fcntl(fd, F_SETOWN, getpid());
	oflags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, oflags | FASYNC);

	while(1)
	{
		sleep(1);
	}

	close(fd);

	return 0;
ERR1:
	return -1;
}
