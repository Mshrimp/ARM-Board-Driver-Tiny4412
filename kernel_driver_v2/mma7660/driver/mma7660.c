#include <linux/kernel.h>
#include "mma7660.h"



int mma7660_init(void)
{
	i2c_init();

	return 0;
}


int mma7660_write_data(char *data, unsigned long len)
{
	int i = 0;

	i2c_start();

	for (i = 0; i < len; i++) {
		printk("mma7660: write data[%d] = 0x%X\n", i, data[i]);
		i2c_write_byte(data[i]);
		if (!i2c_ack(I2C_WAIT_ACK)) {
			printk("mma7660: write data failed, i = %d\n", i);
			return -1;
		}
	}

	i2c_stop();

	printk("mma7660: write data OK\n");

	return 0;
}



