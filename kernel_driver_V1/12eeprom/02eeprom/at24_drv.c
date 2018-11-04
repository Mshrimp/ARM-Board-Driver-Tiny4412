#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/err.h>

static struct class *class;
static struct i2c_client *at24_client;

/*
buf[0]: addr	传入的地址
buf[1]: data	读取到的数据
*/
static ssize_t at24_read(struct file *file, char __user *buf, size_t count, loff_t *off)
{
	unsigned char addr, data;

	copy_from_user((void *)&addr, buf, 1);

	data = i2c_smbus_read_byte_data(at24_client, addr);

	copy_to_user(&buf[1], (void *)&data, 1);

	return 1;
}

/*
buf[0]: addr
buf[1]: data
*/
static ssize_t at24_write(struct file *file, const char __user *buf, size_t count, loff_t *off)
{
	unsigned char ker_buf[2];
	unsigned char addr, data;

	copy_from_user(ker_buf, buf, 2);

	addr = ker_buf[0];
	data = ker_buf[1];

	if (!i2c_smbus_write_byte_data(at24_client, addr, data)) {
		return 2;
	} else {
		return -EIO;
	}

}

static int major;

static struct file_operations at24_fops = {
	.owner = THIS_MODULE,
	.read = at24_read,
	.write = at24_write,
};

static int at24_drv_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	printk("at24_drv, %s, %d\n", __func__, __LINE__);

	at24_client = client;

	major = register_chrdev(0, "at24_dev", &at24_fops);
	printk("register_chrdev, major: %d\n", major);

	class = class_create(THIS_MODULE, "at24_dev");
	device_create(class, NULL, MKDEV(major, 0), NULL, "at24_dev");	// /dev/at24_dev

	return 0;
}

static int at24_drv_remove(struct i2c_client *client)
{
	printk("at24_drv, %s, %d\n", __func__, __LINE__);

	device_destroy(class, MKDEV(major, 0));
	class_destroy(class);
	unregister_chrdev(major, "at24_dev");

	return 0;
}

static const struct i2c_device_id at24_id_table[] = {
	{"at24_mcy", 0},
	{}
};

/****************************************************************/

/* 1. 分配/设置i2c_driver */
static struct i2c_driver at24_driver = {
	.driver = {
		.name = "at24025_ok",
		.owner = THIS_MODULE,
	},
	.probe = at24_drv_probe,
	.remove = __devexit_p(at24_drv_remove),
	.id_table = at24_id_table,
};

static int __init at24_drv_init(void)
{
	int ret = -1;
	printk("at24_drv_init\n");

	/* 2. 注册i2c_driver */
	ret = i2c_add_driver(&at24_driver);
	if (ret) {
		printk("i2c_add_driver failed\n");
	}

	return ret;
}

static void __exit at24_drv_exit(void)
{
	printk("at24_drv_exit\n");

}

module_init(at24_drv_init);
module_exit(at24_drv_exit);

MODULE_DESCRIPTION("Driver for eeprom, at2402 device");
MODULE_AUTHOR("Mshrimp");
MODULE_LICENSE("GPL");

