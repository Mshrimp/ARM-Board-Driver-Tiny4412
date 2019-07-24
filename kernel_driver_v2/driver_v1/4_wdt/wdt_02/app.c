#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sched.h>

#define DEV_NAME	"/dev/wdt_device"

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

	param.sched_priority = sched_get_priority_max(SCHED_FIFO);
	sched_setscheduler(0, SCHED_FIFO, &param);

	ioctl(fd, 1, 1);
	while (1) {
		write(fd, &wfs, sizeof(wfs));
		sleep(2);
	}

	return 0;
}

