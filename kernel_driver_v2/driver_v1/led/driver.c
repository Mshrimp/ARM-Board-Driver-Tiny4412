#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/io.h>
#include "common.h"

#include "led.h"

#define DEV_NAME	"test_driver"

volatile unsigned long *led_con_p = NULL;
volatile unsigned long *led_dat_p = NULL;


/////////////////////////////////////////////////////////////////////裸板驱动


////////////////////////////////////////////////////////////////////////////字符设备框架

long driver_test_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk("Driver: test ioctl!\n");

	return 0;
}

ssize_t driver_test_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	printk("Driver: test write!\n");
	return size;
}

ssize_t driver_test_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	printk("Driver: test read!\n");
	return size;
}

int driver_test_open (struct inode *inodep, struct file *filp)
{
	printk("Driver: test open!\n");

	return 0;
}

int driver_test_close (struct inode *inodep, struct file *filp)
{
	printk("Driver: test close!\n");
	return 0;
}

/////////////////////////////////////////////////////////////模块
struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_test_open,
	.release = driver_test_close,
	.read = driver_test_read,
	.write = driver_test_write,
	.unlocked_ioctl = driver_test_ioctl,
};

int major = 0;
struct class *driver_class;
struct device *driver_class_device;

static int driver_test_init(void)
{
	printk("Hello, driver chrdev register test begin!\n");

	major = register_chrdev(major, DEV_NAME, &fops);
	ERRP_K(major < 0, "Driver", "register_chrdev", goto ERR_dev_register);

	driver_class = class_create(THIS_MODULE, "driver_class");
	ERRP_K(driver_class == NULL, "Driver", "class_create", goto ERR_class_create);

	driver_class_device = device_create(driver_class, NULL, MKDEV(major, 0), NULL, "driver_class_device");
	ERRP_K(driver_class_device == NULL, "Driver", "class_device_create", goto ERR_class_device_create);

	printk("major = %d\n", major);
	
	led_con_p = (volatile unsigned long *)ioremap(LED_CON_ADDR, 16);
	ERRP_K(NULL == led_con_p, "Driver", "led_con_p ioremap", goto ERR_ioremap);
	led_dat_p = led_con_p + 1;

	return 0;
ERR_ioremap:
	//device_unregister(driver_class_device);
	device_destroy(driver_class, MKDEV(major, 0));
ERR_class_device_create:
	class_destroy(driver_class);
ERR_class_create:
	unregister_chrdev(major, DEV_NAME);
ERR_dev_register:
	return -1;
}

static void driver_test_exit(void)
{
	printk("Goodbye, test over!\n");

	iounmap(led_con_p);
	//device_unregister(driver_class_device);
	device_destroy(driver_class, MKDEV(major, 0));
	class_destroy(driver_class);
	unregister_chrdev(major, DEV_NAME);
}


module_init(driver_test_init);
module_exit(driver_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mshrimp");
MODULE_DESCRIPTION("Test driver");
MODULE_VERSION("V0.1");

