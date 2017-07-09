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
	int cmd = -1;

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

	printf("App：Please input the cmd:\n");
	scanf("%d", &cmd);

	if (cmd > LED_IOC_MAX_NR)
	{
		perror("App: cmd");
		return -1;
	}

	switch(cmd)
	{
		case 0:
			printf("App: ioctl cmd = %d, all LED Off\n", cmd);
			ret = ioctl(fd, LED_ALL_OFF, NULL);
			if (ret < 0)
			{
				perror("ioctl: All LED Off\n");
				return -1;
			}
			break;	
		case 1:
			printf("App: ioctl cmd = %d, all LED On\n", cmd);
			ret = ioctl(fd, LED_ALL_ON, NULL);
			if (ret < 0)
			{
				perror("ioctl: All LED On\n");
				return -1;
			}
			break;	
		case 2:
			printf("App: ioctl cmd = %d, LED1 Off\n", cmd);
			ret = ioctl(fd, LED1_OFF, NULL);
			if (ret < 0)
			{
				perror("ioctl: LED1 Off\n");
				return -1;
			}
			break;	
		case 3:
			printf("App: ioctl cmd = %d, LED1 On\n", cmd);
			ret = ioctl(fd, LED_ALL_ON, NULL);
			if (ret < 0)
			{
				perror("ioctl: LED1 On\n");
				return -1;
			}
			break;	
		case 4:
			printf("App: ioctl cmd = %d, LED2 Off\n", cmd);
			ret = ioctl(fd, LED2_OFF, NULL);
			if (ret < 0)
			{
				perror("ioctl: LED2 Off\n");
				return -1;
			}
			break;	
		case 5:
			printf("App: ioctl cmd = %d, LED2 On\n", cmd);
			ret = ioctl(fd, LED_ALL_ON, NULL);
			if (ret < 0)
			{
				perror("ioctl: LED2 On\n");
				return -1;
			}
			break;	
		case 6:
			printf("App: ioctl cmd = %d, LED3 Off\n", cmd);
			ret = ioctl(fd, LED3_OFF, NULL);
			if (ret < 0)
			{
				perror("ioctl: LED3 Off\n");
				return -1;
			}
			break;	
		case 7:
			printf("App: ioctl cmd = %d, LED3 On\n", cmd);
			ret = ioctl(fd, LED_ALL_ON, NULL);
			if (ret < 0)
			{
				perror("ioctl: LED3 On\n");
				return -1;
			}
			break;	
		case 8:
			printf("App: ioctl cmd = %d, LED4 Off\n", cmd);
			ret = ioctl(fd, LED4_OFF, NULL);
			if (ret < 0)
			{
				perror("ioctl: LED4 Off\n");
				return -1;
			}
			break;	
		case 9:
			printf("App: ioctl cmd = %d, LED4 On\n", cmd);
			ret = ioctl(fd, LED_ALL_ON, NULL);
			if (ret < 0)
			{
				perror("ioctl: LED4 On\n");
				return -1;
			}
			break;	
		case 10:
			printf("App: ioctl cmd = %d, Get LED Data\n", cmd);
			ret = ioctl(fd, LED_IOC_GET_DATA, NULL);
			if (ret < 0)
			{
				perror("ioctl: Get LED data\n");
				return -1;
			}
			break;	
		case 11:
			printf("App: ioctl cmd = %d, Set LED Data\n", cmd);
			ret = ioctl(fd, LED_IOC_SET_DATA, NULL);
			if (ret < 0)
			{
				perror("ioctl: Set LED data\n");
				return -1;
			}
			break;	

		default:
			printf("App: Wrong cmd!\n");
			break;
	}

	close(fd);

	return 0;
}
