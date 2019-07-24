#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>


#define	PWM_SET_FREQ		0x12
#define	PWM_STOP			0x14

#define BEEP_PWM_SET_FREQ	_IOW('B', PWM_SET_FREQ, int)
#define BEEP_PWM_STOP		_IOW('B', PWM_STOP, int)

int main(void)
{
	int fd = -1;
	int val = -1;
	int cnt = 0;

	fd = open("/dev/beep_device", O_RDWR);
	if (fd < 0) {
		perror("open");
		exit(1);
	}

	cnt = 5;
	while(cnt) {
		ioctl(fd, BEEP_PWM_SET_FREQ, 2);
		sleep(1);

		ioctl(fd, BEEP_PWM_SET_FREQ, 4);
		sleep(1);

		ioctl(fd, BEEP_PWM_SET_FREQ, 10);
		sleep(1);

		ioctl(fd, BEEP_PWM_SET_FREQ, 0);
		sleep(1);

		cnt--;
		printf("cnt = %d\n", cnt);
	}

	ioctl(fd, BEEP_PWM_STOP, 0);

	close(fd);

	return 0;
}

