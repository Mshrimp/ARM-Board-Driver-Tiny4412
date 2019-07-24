#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sched.h>

#define DEV_NAME	"/dev/test_wdt"

struct wdt_feed_st {
	int time;
};

int main(void)
{
	int fd;
	int ret;
	struct wdt_feed_st wfs;
	struct sched_param param;

	wfs.time = 15625 * 3;	// 3s

	fd = open(DEV_NAME, O_RDWR);
	if (fd < 0) {
		perror("open");
		exit(1);
	}

	while (1) {
		// 喂狗操作
		//write(fd, &wfs, sizeof(wfs));
		ioctl(fd, 1, 1);	// 看门狗使能
		sleep(10);
		ioctl(fd, 0, 1);	// 看门狗使能
		sleep(10);
	}

	return 0;
}

