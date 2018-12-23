#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/gpio.h>
#include <linux/poll.h>
#include <mach/gpio.h>
#include <asm/io.h>
#include <asm/signal.h>

#include "key.h"

#define	DRV_DEBUG(fmt, arg...)		\
			printk("Driver debug: "fmt"(function: %s, line: %d)\n", ##arg, __func__, __LINE__);

#define	DRV_ERROR(fmt, arg...)		\
			printk("Driver error: "fmt"(function: %s, line: %d)\n", ##arg, __func__, __LINE__);

#define DEV_NAME	"test_driver"

typedef struct button_pin {
	unsigned long gpio;
	unsigned long val;
} button_t;

typedef struct button_irq {
	int irq_num;
	unsigned long flag;
	char *name;
} button_irq_t;

static button_t button_desc[] = {
	{EXYNOS4_GPX3(2), 1},
	{EXYNOS4_GPX3(3), 2},
	{EXYNOS4_GPX3(4), 3},
	{EXYNOS4_GPX3(5), 4},
};

static button_irq_t button_irq[] = {
	{0, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "Key1"},
	{0, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "Key2"},
	{0, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "Key3"},
	{0, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "Key4"},
};

static struct timer_list button_timer;
static struct fasync_struct *fs;

static unsigned long key_val;

#define	BUTTON_EXPIRES				(HZ / 20)

/////////////////////////////////////////////////////////////////////裸板驱动


////////////////////////////////////////////////////////////////////////////字符设备框架

static void timer_handler(unsigned long data)
{
	button_t *button_desc = (button_t *)data;
	unsigned long pin_val;
	printk("timer handler\n");

	if (!button_desc) {
		printk("%s, button_desc error!\n", __func__);
		return;
	}

	if (!button_desc->gpio) {
		printk("%s, button_desc->gpio:%ld, error!\n", __func__, button_desc->gpio);
		return;
	}

	pin_val = gpio_get_value(button_desc->gpio);
	
	if (!pin_val) {
		key_val = 0x80 | button_desc->val;
	} else {
		key_val = button_desc->val;
	}

	kill_fasync(&fs, SIGIO, POLL_IN);

	return;
}

static irqreturn_t button_irq_handler(int irq, void *dev_id)
{
	//button_t *button_desc = (button_t *)dev_id;
	printk("Driver: button interrupt, irq: %d\n", irq);

	button_timer.data = (unsigned long)dev_id;

	mod_timer(&button_timer, jiffies + BUTTON_EXPIRES);

	return IRQ_HANDLED;
}


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
	int ret = -1;
	printk("Driver: test read!\n");

	if (size != sizeof(key_val)) {
		printk("error, size: %d\n", size);
		return -EINVAL;
	}

	ret = copy_to_user(buf, &key_val, sizeof(key_val));
	if (ret < 0) {
		DRV_ERROR("copy_to_user failed, ret = %d", ret);
		return -EFAULT;
	}

	return 0;
}

int driver_test_fasync(int fd, struct file *filp, int on)
{
	fasync_helper(fd, filp, on, &fs);

	return 0;
}

int driver_test_open (struct inode *inodep, struct file *filp)
{
	int i = 0;
	int ret = 0;
	printk("Driver: test open!\n");

	for (i = 0; i < ARRAY_SIZE(button_desc); i++) {
		if (!button_desc[i].gpio) {
			printk("error, button_desc[%d].gpio: %ld\n", i, button_desc[i].gpio);
			continue;
		}

		button_irq[i].irq_num = gpio_to_irq(button_desc[i].gpio);
		DRV_DEBUG("irq[%d] = %d", i, button_irq[i].irq_num);

		ret = request_irq(button_irq[i].irq_num, button_irq_handler, button_irq[i].flag, button_irq[i].name, &button_desc[i]);
		if (ret) {
			DRV_ERROR("request_irq irq[%d] failed, ret = %d", i, ret);
			break;
		}
	}

	if (ret) {
		for (--i; i >= 0; i--) {
			if (!button_desc[i].gpio) {
				printk("error, button_desc[%d].gpio: %ld\n", i, button_desc[i].gpio);
				continue;
			}
			free_irq(button_irq[i].irq_num, &button_desc[i]);
		}

		return -EBUSY;
	}

	init_timer(&button_timer);
	button_timer.function = timer_handler;
	add_timer(&button_timer);

	return 0;
}

int driver_test_close (struct inode *inodep, struct file *filp)
{
	int i = 0;
	
	printk("Driver: test close!\n");

	del_timer_sync(&button_timer);

	for (i = 0; i < ARRAY_SIZE(button_desc); i++) {
		if (!button_desc[i].gpio) {
			printk("error, button_desc[%d].gpio: %ld\n", i, button_desc[i].gpio);
			continue;
		}
		free_irq(button_irq[i].irq_num, &button_desc[i]);
		printk("Driver close: free_irq button_irq[%d]\n", i);
	}

	return 0;
}

/////////////////////////////////////////////////////////////模块
struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_test_open,
	.release = driver_test_close,
	.read = driver_test_read,
	.write = driver_test_write,
	.fasync = driver_test_fasync,
	.unlocked_ioctl = driver_test_ioctl,
};

int major = 0;
struct class *driver_class;
struct device *driver_class_device;

static int driver_test_init(void)
{
	printk("Hello, driver chrdev register test begin!\n");

	major = register_chrdev(major, DEV_NAME, &fops);
	if (major < 0) {
		DRV_ERROR("register_chrdev failed");
		goto ERR_dev_register;
	}

	driver_class = class_create(THIS_MODULE, "driver_class");
	if (!driver_class) {
		DRV_ERROR("class_create failed");
		goto ERR_class_create;
	}

	driver_class_device = device_create(driver_class, NULL, MKDEV(major, 0), NULL, "button_device");
	if (!driver_class_device) {
		DRV_ERROR("device_create failed");
		goto ERR_class_device_create;
	}

	printk("major = %d\n", major);

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
	printk("Driver: exit!\n");

	unregister_chrdev(major, DEV_NAME);
	device_destroy(driver_class, MKDEV(major, 0));
	class_destroy(driver_class);
}


module_init(driver_test_init);
module_exit(driver_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mshrimp");
MODULE_DESCRIPTION("Test driver");
MODULE_VERSION("V0.1");

