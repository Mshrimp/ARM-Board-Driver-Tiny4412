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

////////////////////////////////////////////////////////////////////////////字符设备框架
ssize_t driver_led_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	int val = 0;
	int ret = 0;

	drv_info("Driver: led write!\n");
	drv_info("Driver: buf = %d, size = %d\n", *(int *)buf, size);

	ret = copy_from_user((void *)&val, buf, size);
	ERRP_K(ret != 0, "Driver", "copy_from_user", return -1);

	drv_info("Driver: val = %d, ret = %d\n", val, ret);

	if (val == 0) {
		*led_dat_p &= ~0xF;
	} else {
		*led_dat_p |= 0xF;
	}

	return size;
}

ssize_t driver_led_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	drv_info("Driver: led read!\n");

	return size;
}

long driver_led_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	drv_info("Driver: led ioctl!\n");

	return 0;
}

int driver_led_open (struct inode *inodep, struct file *filp)
{
	drv_info("Driver: led open!\n");

	drv_info("Driver: led config\n");

	*led_con_p &= ~0xFFFF;
	*led_con_p |= 0x1111;

	drv_info("Driver: led off\n");

	*led_dat_p |= 0xF;

	return 0;
}

int driver_led_close (struct inode *inodep, struct file *filp)
{
	drv_info("Driver: led close!\n");

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

static int major = 0;
static struct class *driver_class;
static struct device *driver_class_device;

static int driver_led_init(void)
{
	drv_info("Hello, driver chrdev register led begin!\n");

	major = register_chrdev(major, DEV_NAME, &fops);
	ERRP_K(major < 0, "Driver", "register_chrdev", goto ERR_dev_register);

	driver_class = class_create(THIS_MODULE, "led_class");
	ERRP_K(driver_class == NULL, "Driver", "class_create", goto ERR_class_create);

	driver_class_device = device_create(driver_class, NULL, MKDEV(major, 0), NULL, "led_device");
	ERRP_K(driver_class_device == NULL, "Driver", "class_device_create", goto ERR_class_device_create);

	drv_info("major = %d\n", major);
	
	led_con_p = (volatile unsigned long *)ioremap(LED_CON_ADDR, 16);
	ERRP_K(NULL == led_con_p, "Driver", "led_con_p ioremap", goto ERR_ioremap);
	led_dat_p = led_con_p + 1;

	return 0;
ERR_ioremap:
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
	drv_info("Goodbye, led over!\n");

	iounmap(led_con_p);
	unregister_chrdev(major, DEV_NAME);
	device_destroy(driver_class, MKDEV(major, 0));
	class_destroy(driver_class);
}

module_init(driver_led_init);
module_exit(driver_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mshrimp");
MODULE_DESCRIPTION("Test driver");
MODULE_VERSION("V0.1");

