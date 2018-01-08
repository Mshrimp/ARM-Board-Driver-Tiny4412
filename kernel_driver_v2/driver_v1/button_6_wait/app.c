#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/ioctl.h>

#include "common.h"
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
	fd = open(argv[1], O_RDWR);
	ERRP(fd < 0, "App", "open dev", goto ERR1);
	printf("App: open, fd = %d\n", fd);

	while(1)
	{
        /*
		 *ret = read(fd, key_vals, sizeof(key_vals));
		 *ERRP(ret < 0, "App", "read failed", goto ERR2);
		 *for (i = 0; i < KEY_TOTLE; i++) {
		 *    if (!key_vals[i]) {
		 *        printf("Button: %d down\n", i);
		 *    }
		 *}
         */

        /*
		 *if ((!key_vals[0]) || (!key_vals[1]) || (!key_vals[2]) || (!key_vals[3])) {
		 *{
		 *    printf("Button: %d %d %d %d\n", key_vals[0], key_vals[1], key_vals[2], key_vals[3]);
		 *}
         */
	}

	close(fd);

	return 0;
ERR2:
ERR1:
	return -1;
}
