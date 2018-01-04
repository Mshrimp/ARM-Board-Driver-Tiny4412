#include <stdio.h>
#include <fcntl.h>

#define	LED_NAME		"/dev/driver_tiny4412_led"


int main(void)
{
	int num = 0;
	int status = 0;
	int ret = -1;
	int fd = -1;

	fd = open(LED_NAME, O_RDONLY | O_WRONLY);
	if (fd < 0) {
		printf("Demo: open %s failed\n", LED_NAME);
		return -1;
	}

	while(1) {
		printf("Input LED number:\n");
		scanf("%d", &num);
		printf("0: Off\t\t\t1: On\n");
		scanf("%d", &status);

		if (num > 4 || status > 1) {
			printf("Demo: LED number or status error\n");
			break;
		}

		printf("LED[%d]: %s\n", num, status ? "On" : "Off");
		ret = ioctl(fd, num, &status);
		if (ret < 0) {
			printf("Demo: ioctl failed, ret = %d\n", ret);
			break;
		}
	}
	close(fd);

	return 0;
}

