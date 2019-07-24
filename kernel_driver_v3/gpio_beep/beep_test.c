#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>


#define BEEP_SWITCH	_IOW('B', 0x12, int)

int main(void)
{
	int fd = -1;
	int val = -1;

	fd = open("/dev/beep_device", O_RDWR);
	if (fd < 0) {
		perror("open");
		exit(1);
	}

	while(1) {
		ioctl(fd, BEEP_SWITCH, 1);
		sleep(1);

		ioctl(fd, BEEP_SWITCH, 0);
		sleep(1);	
	}

	return 0;
}

