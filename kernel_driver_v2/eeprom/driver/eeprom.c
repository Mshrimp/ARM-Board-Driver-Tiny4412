#include <linux/kernel.h>
#include "eeprom.h"



int eeprom_init(void)
{
	i2c_init();

	return 0;
}


int eeprom_write_data(char *data, unsigned long len)
{
	int i = 0;

	i2c_start();

	for (i = 0; i < len; i++) {
		printk("eeprom: write data[%d] = 0x%X\n", i, data[i]);
		i2c_write_byte(data[i]);
		if (!i2c_ack(I2C_WAIT_ACK)) {
			printk("eeprom: write data failed, i = %d\n", i);
			return -1;
		}
	}

	i2c_stop();

	printk("eeprom: write data OK\n");

	return 0;
}



