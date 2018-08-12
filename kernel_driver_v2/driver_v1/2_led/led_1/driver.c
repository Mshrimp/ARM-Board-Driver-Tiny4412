#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include "common.h"

#include "led.h"

#define DEV_NAME	"led_driver"

volatile unsigned long *led_con_p = NULL;
volatile unsigned long *led_dat_p = NULL;


/////////////////////////////////////////////////////////////////////裸板驱动


////////////////////////////////////////////////////////////////////////////字符设备框架

long driver_led_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk("Driver: led ioctl!\n");

	return 0;
}

ssize_t driver_led_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	int val = 0;
	int ret = 0;

	printk("Driver: led write!\n");
	printk("Driver: buf = %d, size = %d\n", *(int *)buf, size);
	ret = copy_from_user((void *)&val, buf, size);
	//copy_from_user(&(void *)val, buf, (unsigned long)size);
	printk("Driver: val = %d, ret = %d\n", val, ret);

	if (val == 0) {
		*led_dat_p &= ~0xF;
	} else {
		*led_dat_p |= 0xF;
	}


	return size;
}

ssize_t driver_led_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	printk("Driver: led read!\n");
	return size;
}

int driver_led_open (struct inode *inodep, struct file *filp)
{
	printk("Driver: led open!\n");

	printk("Driver: led config\n");
	*led_con_p &= ~0xFFFF;
	*led_con_p |= 0x1111;

	printk("Driver: led off\n");
	*led_dat_p |= 0xF;

	return 0;
}

int driver_led_close (struct inode *inodep, struct file *filp)
{
	printk("Driver: led close!\n");
	return 0;
}

/////////////////////////////////////////////////////////////模块
struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_led_open,
	.release = driver_led_close,
	.read = driver_led_read,
	.write = driver_led_write,
	.unlocked_ioctl = driver_led_ioctl,
};

int major = 0;
struct class *driver_class;
struct device *driver_class_device;

static int driver_led_init(void)
{
	printk("Hello, driver chrdev register led begin!\n");

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

static void driver_led_exit(void)
{
	printk("Goodbye, led over!\n");

	iounmap(led_con_p);
	//device_unregister(driver_class_device);
	device_destroy(driver_class, MKDEV(major, 0));
	class_destroy(driver_class);
	unregister_chrdev(major, DEV_NAME);
}


module_init(driver_led_init);
module_exit(driver_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mshrimp");
MODULE_DESCRIPTION("Test driver");
MODULE_VERSION("V0.1");

