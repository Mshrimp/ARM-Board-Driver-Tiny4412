#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>

#define DEV_NAME	"driver_4412"

#define drv_info(fmt, args...)		\
			printk("Info Driver: " fmt "(function: %s, line: %d)\n", ##args, __func__, __LINE__)

#define drv_debug(fmt, args...)		\
			printk("Debug Driver: " fmt "(function: %s, line: %d)\n", ##args, __func__, __LINE__)

#define drv_error(fmt, args...)		\
			printk("Error Driver: " fmt "(function: %s, line: %d)\n", ##args, __func__, __LINE__)

/////////////////////////////////////////////////////////////////////裸板驱动


////////////////////////////////////////////////////////////////////////////字符设备框架

long driver_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	drv_info("ioctl!");

	return 0;
}

ssize_t driver_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	drv_info("write!");
	return size;
}

ssize_t driver_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	drv_info("read!");
	return size;
}

int driver_open (struct inode *inodep, struct file *filp)
{
	drv_info("open!");
	return 0;
}

int driver_close (struct inode *inodep, struct file *filp)
{
	drv_info("close!");
	return 0;
}

/////////////////////////////////////////////////////////////模块
struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write,
	.unlocked_ioctl = driver_ioctl,
};

static int major = 0;
static struct class *driver_class;
static struct device *driver_class_device;

static int char_driver_init(void)
{
	drv_info("Hello, driver chrdev register begin!");

	major = register_chrdev(major, DEV_NAME, &fops);
	if (major < 0) {
		drv_error("register_chrdev failed");
		goto ERR_dev_register;
	}

	driver_class = class_create(THIS_MODULE, "driver_class");
	if (!driver_class) {
		drv_error("class_create failed");
		goto ERR_class_create;
	}

	driver_class_device = device_create(driver_class, NULL, MKDEV(major, 0), NULL, "driver_class_device");
	if (!driver_class_device) {
		drv_error("device_create failed");
		goto ERR_class_device_create;
	}

	drv_debug("major = %d", major);

	return 0;
ERR_class_device_create:
	device_destroy(driver_class, MKDEV(major, 0));
ERR_class_create:
	unregister_chrdev(major, DEV_NAME);
ERR_dev_register:
	return -1;
}

static void char_driver_exit(void)
{
	drv_info("Goodbye!");
	unregister_chrdev(major, DEV_NAME);
	class_destroy(driver_class);
	device_destroy(driver_class, MKDEV(major, 0));
}

module_init(char_driver_init);
module_exit(char_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mshrimp");
MODULE_DESCRIPTION("Test driver");
MODULE_VERSION("V0.1");

