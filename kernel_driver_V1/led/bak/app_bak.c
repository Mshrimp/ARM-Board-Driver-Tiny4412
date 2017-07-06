#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#define	LED_Device_PATH		/dev/led


int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s dev_file\n", argv[0]);
		return -1;
	}

	int fd;

	fd = open(argv[1], O_RDWR);
	if (-1 == fd)
	{
		perror("open");
		return -1;
	}
	printf("open: fd = %d\n", fd);

	getchar();

	int ret;

	ret = write(fd, "hello", 3);
	printf("write: ret = %d\n", ret);

    /*
	 *ret = read(fd, NULL, 5);
	 *printf("read: ret = %d\n", ret);
     */


	return 0;
}
