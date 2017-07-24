#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/ioctl.h>

#include "common.h"
#include "led.h"

#define	LED_Device_PATH		/dev/led

void led_note(void)
{
	printf("Please enter a cmd:\n");
	printf("LED_All:	0--Off		1--On\n");
	printf("LED_1:		2--Off		3--On\n");
	printf("LED_2:		4--Off		5--On\n");
	printf("LED_3:		6--Off		7--On\n");
	printf("LED_4:		8--Off		9--On\n");
	printf("LED_Status:	10--Get		11--Set\n");
	printf("LED_Blink:	12--Set		13--Get\n");
	printf("LED_RunLamp:14--Set		15--Set\n");
}

int	led_ioctl(int fd, unsigned int cmd)
{
	int ret = -1;
	unsigned long arg = 0;

	switch(cmd)
	{
		case 0:
			printf("App: ioctl cmd = %d, all LED Off\n", cmd);
			ret = ioctl(fd, LED_ALL_OFF, NULL);
			ERRP(ret < 0, "LED", "LED_ALL_OFF", goto ERR1);
			/*
			 *if (ret < 0)
			 *{
			 *    perror("ioctl: All LED Off\n");
			 *    return -1;
			 *}
			 */
			break;	
		case 1:
			printf("App: ioctl cmd = %d, all LED On\n", cmd);
			ret = ioctl(fd, LED_ALL_ON, NULL);
			ERRP(ret < 0, "LED", "LED_ALL_ON", goto ERR1);
			/*
			 *if (ret < 0)
			 *{
			 *    perror("ioctl: All LED On\n");
			 *    return -1;
			 *}
			 */
			break;	
		case 2:
			printf("App: ioctl cmd = %d, LED1 Off\n", cmd);
			ret = ioctl(fd, LED1_OFF, NULL);
			ERRP(ret < 0, "LED", "LED1_OFF", goto ERR1);
			/*
			 *if (ret < 0)
			 *{
			 *    perror("ioctl: LED1 Off\n");
			 *    return -1;
			 *}
			 */
			break;	
		case 3:
			printf("App: ioctl cmd = %d, LED1 On\n", cmd);
			ret = ioctl(fd, LED1_ON, NULL);
			ERRP(ret < 0, "LED", "LED1_ON", goto ERR1);
			/*
			 *if (ret < 0)
			 *{
			 *    perror("ioctl: LED1 On\n");
			 *    return -1;
			 *}
			 */
			break;	
		case 4:
			printf("App: ioctl cmd = %d, LED2 Off\n", cmd);
			ret = ioctl(fd, LED2_OFF, NULL);
			ERRP(ret < 0, "LED", "LED2_OFF", goto ERR1);
			/*
			 *if (ret < 0)
			 *{
			 *    perror("ioctl: LED2 Off\n");
			 *    return -1;
			 *}
			 */
			break;	
		case 5:
			printf("App: ioctl cmd = %d, LED2 On\n", cmd);
			ret = ioctl(fd, LED2_ON, NULL);
			ERRP(ret < 0, "LED", "LED2_ON", goto ERR1);
			/*
			 *if (ret < 0)
			 *{
			 *    perror("ioctl: LED2 On\n");
			 *    return -1;
			 *}
			 */
			break;	
		case 6:
			printf("App: ioctl cmd = %d, LED3 Off\n", cmd);
			ret = ioctl(fd, LED3_OFF, NULL);
			ERRP(ret < 0, "LED", "LED3_OFF", goto ERR1);
			/*
			 *if (ret < 0)
			 *{
			 *    perror("ioctl: LED3 Off\n");
			 *    return -1;
			 *}
			 */
			break;	
		case 7:
			printf("App: ioctl cmd = %d, LED3 On\n", cmd);
			ret = ioctl(fd, LED3_ON, NULL);
			ERRP(ret < 0, "LED", "LED3_ON", goto ERR1);
			/*
			 *if (ret < 0)
			 *{
			 *    perror("ioctl: LED3 On\n");
			 *    return -1;
			 *}
			 */
			break;	
		case 8:
			printf("App: ioctl cmd = %d, LED4 Off\n", cmd);
			ret = ioctl(fd, LED4_OFF, NULL);
			ERRP(ret < 0, "LED", "LED4_OFF", goto ERR1);
			/*
			 *if (ret < 0)
			 *{
			 *    perror("ioctl: LED4 Off\n");
			 *    return -1;
			 *}
			 */
			break;	
		case 9:
			printf("App: ioctl cmd = %d, LED4 On\n", cmd);
			ret = ioctl(fd, LED4_ON, NULL);
			ERRP(ret < 0, "LED", "LED4_ON", goto ERR1);
			/*
			 *if (ret < 0)
			 *{
			 *    perror("ioctl: LED4 On\n");
			 *    return -1;
			 *}
			 */
			break;	
		case 10:
			printf("App: ioctl cmd = %d, Get LED Data\n", cmd);
			ret = ioctl(fd, LED_IOC_GET_STATUS, &arg);
			ERRP(ret < 0, "LED", "LED get status", goto ERR1);
			printf("App: LED get status = 0x%X\n", arg);
			/*
			 *if (ret < 0)
			 *{
			 *    perror("ioctl: Get LED data\n");
			 *    return -1;
			 *}
			 */
			break;	
		case 11:
			printf("App: ioctl cmd = %d, Set LED Data\n", cmd);
			ret = ioctl(fd, LED_IOC_SET_STATUS, NULL);
			ERRP(ret < 0, "LED", "LED set status", goto ERR1);
			/*
			 *if (ret < 0)
			 *{
			 *    perror("ioctl: Set LED data\n");
			 *    return -1;
			 *}
			 */
			break;	

		default:
			printf("App: Wrong cmd!\n");
			break;
	}

	return 0;
ERR1:
	return -1;
}

int main(int argc, char *argv[])
{
	int fd;
	int ret = -1;
	int cmd = -1;

	if (argc != 2)
	{
		fprintf(stderr, "App: Usage: %s dev_file\n", argv[0]);
		return -1;
	}

	fd = open(argv[1], O_RDWR);
	ERRP(fd < 0, "App", "open dev", goto ERR1);
    /*
	 *if (fd < 0)
	 *{
	 *    perror("App: open");
	 *    return -1;
	 *}
     */
	printf("App: open, fd = %d\n", fd);

	printf("App: ioctl all LED Off\n");
	ret = ioctl(fd, LED_ALL_OFF, NULL);
	ERRP(ret < 0, "App", "LED_ALL_OFF", goto ERR1);
    /*
	 *if (ret < 0)
	 *{
	 *    perror("App: ioctl, All LED Off\n");
	 *    return -1;
	 *}
     */

	while(1)
	{
		led_note();
		printf("App：Please input the cmd:\n");
		scanf("%d", &cmd);

		if (cmd > LED_IOC_MAX_NR)
		{
			perror("App: cmd");
			return -1;
		}

		ret = led_ioctl(fd, cmd);
		ERRP(ret < 0, "App", "led_ioctl", goto ERR1);

	}

	close(fd);

	return 0;

ERR1:
	return -1;
}
