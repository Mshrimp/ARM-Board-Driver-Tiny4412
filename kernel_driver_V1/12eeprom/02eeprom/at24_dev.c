#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>


/****************************************************************/

static struct i2c_board_info at24_info = {
	I2C_BOARD_INFO("at24_mcy", 0x50),
};

static struct i2c_client *at24_client;

static int at24_dev_init(void)
{
	struct i2c_adapter *i2c_adap;
	printk("at24_dev_init\n");

	i2c_adap = i2c_get_adapter(0);

	at24_client = i2c_new_device(i2c_adap, &at24_info);	// 认为设备肯定存在
	// i2c_new_probed_device

	i2c_put_adapter(i2c_adap);

	return 0;
}

static void at24_dev_exit(void)
{
	printk("at24_dev_exit\n");

	i2c_unregister_device(at24_client);
}

module_init(at24_dev_init);
module_exit(at24_dev_exit);

MODULE_DESCRIPTION("Driver for eeprom, at2402 device");
MODULE_AUTHOR("Mshrimp");
MODULE_LICENSE("GPL");

