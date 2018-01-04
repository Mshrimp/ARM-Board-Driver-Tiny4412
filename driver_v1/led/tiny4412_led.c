#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/gpio.h>

#define drv_info(fmt, args...)		\
			printk("Info LED: " fmt "(function: %s, line: %d)\n", ##args, __func__, __LINE__)

#define drv_debug(fmt, args...)		\
			printk("Debug LED: " fmt "(function: %s, line: %d)\n", ##args, __func__, __LINE__)

#define drv_error(fmt, args...)		\
			printk("Error LED: " fmt "(function: %s, line: %d)\n", ##args, __func__, __LINE__)

#define DEVICE_NAME	"driver_tiny4412_led"

static int led_gpio[] = {
	EXYNOS4X12_GPM4(0),
	EXYNOS4X12_GPM4(1),
	EXYNOS4X12_GPM4(2),
	EXYNOS4X12_GPM4(3),
};

#define LED_NUM		ARRAY_SIZE(led_gpio)

/////////////////////////////////////////////////////////////////////裸板驱动


////////////////////////////////////////////////////////////////////////////字符设备框架

long driver_led_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	drv_info("tiny4412 led ioctl!");

	switch(cmd) {
		case 0:
		case 1:
			if (arg > LED_NUM) {
				return -EINVAL;
			}

			gpio_set_value(led_gpio[arg], !cmd);
			break;

		default:
			return -EINVAL;
	}

	return 0;
}

ssize_t driver_led_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	drv_info("tiny4412 led write!");
	return size;
}

ssize_t driver_led_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	drv_info("tiny4412 led read!");
	return size;
}

int driver_led_open (struct inode *inodep, struct file *filp)
{
	drv_info("tiny4412 led open!");
	return 0;
}

int driver_led_close (struct inode *inodep, struct file *filp)
{
	drv_info("tiny4412 led close!");
	return 0;
}

/////////////////////////////////////////////////////////////模块
static struct file_operations tiny4412_led_dev_fops = {
	.owner = THIS_MODULE,
	.open = driver_led_open,
	.release = driver_led_close,
	.read = driver_led_read,
	.write = driver_led_write,
	.unlocked_ioctl = driver_led_ioctl,
};

static struct miscdevice tiny4412_led_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &tiny4412_led_dev_fops,
};

static int driver_led_init(void)
{
	int ret = -1;
	int i = 0;
	char name[8] = { 0 };

	drv_info("Hello, driver chrdev register begin!");

	for (i = 0; i < LED_NUM; i++) {
		sprintf(name, "LED%d", i+1);
		ret = gpio_request(led_gpio[i], name);
		if (ret) {
			drv_error("gpio_request failed, gpio: %d, ret = %d", led_gpio[i], ret);
			return ret;
		}

		gpio_direction_output(led_gpio[i], 1);
		//gpio_set_value(led_gpio[i], 1);
	}

	ret = misc_register(&tiny4412_led_dev);
	if (ret < 0) {
		drv_error("misc_register failed");
		goto ERR_misc_register;
	}

	drv_info(DEVICE_NAME" initialized");

	return ret;

ERR_misc_register:
	for (i = 0; i < LED_NUM; i++) {
		gpio_free(led_gpio[i]);
	}
	return -1;
}

static void driver_led_exit(void)
{
	int i = 0;

	drv_info("Goodbye!");

	for (i = 0; i < LED_NUM; i++) {
		gpio_free(led_gpio[i]);
	}

	misc_deregister(&tiny4412_led_dev);
}

module_init(driver_led_init);
module_exit(driver_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mshrimp");
MODULE_DESCRIPTION("tiny4412 led driver");
MODULE_VERSION("V0.1");

