#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>

#include <linux/gpio.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>

#include <asm/io.h>
#include "common.h"

#include "led.h"

#define DEV_NAME	"test_driver"

volatile unsigned long *led_con_p = NULL;
volatile unsigned long *led_dat_p = NULL;


static int led_gpios[] = {
	EXYNOS4X12_GPM4(0),
	EXYNOS4X12_GPM4(1),
	EXYNOS4X12_GPM4(2),
	EXYNOS4X12_GPM4(3),
};

#define LED_NUM	ARRAY_SIZE(led_gpios)

////////////////////////////////////////////////////////////////////////////字符设备框架

long driver_test_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	drv_info("Driver: test ioctl!\n");

	return 0;
}

ssize_t driver_test_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	drv_info("Driver: test write!\n");
	return size;
}

ssize_t driver_test_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	drv_info("Driver: test read!\n");
	return size;
}

int driver_test_open (struct inode *inodep, struct file *filp)
{
	drv_info("Driver: test open!\n");

	return 0;
}

int driver_test_close (struct inode *inodep, struct file *filp)
{
	drv_info("Driver: test close!\n");
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
	int i = 0;
	int ret = -1;

	drv_info("Hello, driver chrdev register test begin!\n");

	major = register_chrdev(major, DEV_NAME, &fops);
	ERRP_K(major < 0, "Driver", "register_chrdev", goto ERR_dev_register);

	driver_class = class_create(THIS_MODULE, "driver_class");
	ERRP_K(driver_class == NULL, "Driver", "class_create", goto ERR_class_create);

	driver_class_device = device_create(driver_class, NULL, MKDEV(major, 0), NULL, "driver_class_device");
	ERRP_K(driver_class_device == NULL, "Driver", "class_device_create", goto ERR_class_device_create);

	drv_info("major = %d\n", major);
	
	for (i = 0; i < LED_NUM; i++) {
		ret = gpio_request(led_gpios[i], "led");
		if (ret) {
			drv_info("%s: gpio_request gpio[%d] failed, ret = %d\n", __func__, i, ret);
		}
	}

	s3c_gpio_cfgpin(led_gpios[0], S3C_GPIO_OUTPUT);
	gpio_set_value(led_gpios[0], 1);

	return 0;
ERR_class_device_create:
	class_destroy(driver_class);
ERR_class_create:
	unregister_chrdev(major, DEV_NAME);
ERR_dev_register:
	return -1;
}

static void driver_test_exit(void)
{
	int i = 0;
	drv_info("Goodbye, test over!\n");

	for (i = 0; i < LED_NUM; i++) {
		gpio_free(led_gpios[i]);
	}
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

