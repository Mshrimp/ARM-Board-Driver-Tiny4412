#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include "common.h"

#include "led.h"

#define DEFINE_SYSTEM	0
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
	int minor = 0;
	int val = 0;
	int ret = 0;

	minor = MINOR(filp->f_dentry->d_inode->i_rdev);
	printk("Driver: minor = %d\n", minor);

	printk("Driver: test write!\n");
	printk("Driver: buf = %d, size = %d\n", *(int *)buf, size);
	ret = copy_from_user((void *)&val, buf, size);
	printk("Driver: val = %d, ret = %d\n", val, ret);

#if DEFINE_SYSTEM
	switch (minor) {
		case 0:
			if (val == 0) {
				*led_dat_p &= ~0xF;
			} else {
				*led_dat_p |= 0xF;
			}
			break;
		case 1:
			if (val == 0) {
				*led_dat_p &= ~(0x1 << 0);
			} else {
				*led_dat_p |= 0x1 << 0;
			}
			break;
		case 2:
			if (val == 0) {
				*led_dat_p &= ~(0x1 << 1);
			} else {
				*led_dat_p |= 0x1 << 1;
			}
			break;
		case 3:
			if (val == 0) {
				*led_dat_p &= ~(0x1 << 2);
			} else {
				*led_dat_p |= 0x1 << 2;
			}
			break;
		case 4:
			if (val == 0) {
				*led_dat_p &= ~(0x1 << 3);
			} else {
				*led_dat_p |= 0x1 << 3;
			}
			break;
		default:
			printk("Driver: wrong cmd!\n");
			break;
	}
#else
	switch (minor) {
		case 0:
			if (val == 0) {
				*led_dat_p &= ~0xF;
			} else {
				*led_dat_p |= 0xF;
			}
			break;
		case 1:
			if (val == 0) {
				*led_dat_p &= ~(0x1 << 0);
			} else {
				*led_dat_p |= 0x1 << 0;
			}
			break;
		case 2:
			if (val == 0) {
				*led_dat_p &= ~(0x1 << 1);
			} else {
				*led_dat_p |= 0x1 << 1;
			}
			break;
		case 3:
			if (val == 0) {
				*led_dat_p &= ~(0x1 << 2);
			} else {
				*led_dat_p |= 0x1 << 2;
			}
			break;
		case 4:
			if (val == 0) {
				*led_dat_p &= ~(0x1 << 3);
			} else {
				*led_dat_p |= 0x1 << 3;
			}
			break;
		default:
			printk("Driver: wrong cmd!\n");
			break;
	}
#endif

	return size;
}

ssize_t driver_test_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	printk("Driver: test read!\n");
	return size;
}

int driver_test_open (struct inode *inodep, struct file *filp)
{
	int minor = 0;
	int i = 0;
	printk("Driver: test open!\n");

	minor = MINOR(inodep->i_rdev);
	printk("Driver: minor = %d\n", minor);
	printk("Driver: led config\n");
#ifdef DEFINE_SYSTEM
	*led_con_p &= ~0xFFFF;
	*led_con_p |= 0x1111;
#else
	for (i = 0; i < LED_TOTLE; i++) {
		s3c_gpio_cfgpin(EXYNOS4_GPM4(i), S3C_GPIO_SFN(0x1));
	}
#endif
	printk("Driver: led off\n");
	*led_dat_p |= 0xF;

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
struct class *driver_class = NULL;
struct device *driver_class_device[LED_TOTLE + 1] = { NULL };

static int driver_test_init(void)
{
	int i = 0;
	printk("Hello, driver chrdev register test begin!\n");

	major = register_chrdev(major, DEV_NAME, &fops);
	ERRP_K(major < 0, "Driver", "register_chrdev", goto ERR_dev_register);

	driver_class = class_create(THIS_MODULE, "driver_class");
	ERRP_K(driver_class == NULL, "Driver", "class_create", goto ERR_class_create);

	driver_class_device[0] = device_create(driver_class, NULL, MKDEV(major, 0), NULL, "driver_class_devices");
	ERRP_K(driver_class_device[0] == NULL, "Driver", "class_device_create", goto ERR_class_device_creates);

	for (i = 1; i <= LED_TOTLE; i++) {
		driver_class_device[i] = device_create(driver_class, NULL, MKDEV(major, i), NULL, "driver_class_device%d", i);
		if (unlikely(IS_ERR(driver_class_device[i]))) {
			return PTR_ERR(driver_class_device[i]);
		}
		//ERRP_K(driver_class_device[i] == NULL, "Driver", "class_device_create", goto ERR_class_device_createi);
	}

	printk("major = %d\n", major);
	
	led_con_p = (volatile unsigned long *)ioremap(LED_CON_ADDR, 16);
	ERRP_K(NULL == led_con_p, "Driver", "led_con_p ioremap", goto ERR_ioremap);
	led_dat_p = led_con_p + 1;

	return 0;
ERR_ioremap:
	//device_unregister(driver_class_device);
	device_destroy(driver_class, MKDEV(major, i));
	for (i = 1; i <= LED_TOTLE; i++) {
		device_destroy(driver_class, MKDEV(major, i));
	}
//ERR_class_device_createi:
	device_destroy(driver_class, MKDEV(major, 0));
ERR_class_device_creates:
	class_destroy(driver_class);
ERR_class_create:
	unregister_chrdev(major, DEV_NAME);
ERR_dev_register:
	return -1;
}

static void driver_test_exit(void)
{
	int i = 0;
	printk("Goodbye, test over!\n");

	iounmap(led_con_p);
	//device_unregister(driver_class_device);
	for (i = 0; i <= LED_TOTLE; i++) {
		device_destroy(driver_class, MKDEV(major, i));
	}
	class_destroy(driver_class);
	unregister_chrdev(major, DEV_NAME);
}


module_init(driver_test_init);
module_exit(driver_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mshrimp");
MODULE_DESCRIPTION("Test driver");
MODULE_VERSION("V0.1");

