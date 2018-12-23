#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/ioctl.h>

#include "key.h"

#define	DEV_PATH		"/dev/xxx"

int main(int argc, char *argv[])
{
	int fd;
	unsigned char key_vals[KEY_TOTLE] = { 0 };
	int ret = -1;
	int i = 0;

	if (argc != 2)
	{
		fprintf(stderr, "App: Usage: %s dev_file\n", argv[0]);
		return -1;
	}

	//fd = open(DEV_PATH, O_RDWR);
	//fd = open(argv[1], O_RDWR | O_NONBLOCK);
	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		printf("App: open dev failed\n");
		goto ERR1;
	}
	printf("App: open, fd = %d\n", fd);

	while(1)
	{
		ret = read(fd, key_vals, sizeof(key_vals));
		if (ret < 0) {
			printf("App: read dev file failed, ret = %d\n", ret);
			goto ERR2;
		}

		for (i = 0; i < KEY_TOTLE; i++) {
			if (!key_vals[i]) {
				printf("Button: %d down\n", i);
			}
		}
	}
	close(fd);

	return 0;
ERR2:
	close(fd);
ERR1:
	return -1;
}
