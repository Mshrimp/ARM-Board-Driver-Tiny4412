#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/gpio.h>
#include <mach/gpio.h>
#include <asm/io.h>

#include "key.h"

#define	DRV_DEBUG(fmt, arg...)		\
			printk("Driver debug: "fmt"(function: %s, line: %d)\n", ##arg, __func__, __LINE__);

#define	DRV_ERROR(fmt, arg...)		\
			printk("Driver error: "fmt"(function: %s, line: %d)\n", ##arg, __func__, __LINE__);

#define DEV_NAME	"key_driver"
#define	BUTTON_DELAY_MS				50

typedef struct button_pin {
	unsigned long gpio;
	unsigned long val;
} button_t;

typedef struct button_irq {
	int irq_num;
	unsigned long flag;
	char *name;
	struct timer_list timer;
} button_irq_t;

static button_t button_desc[] = {
	{EXYNOS4_GPX3(2), 1},
	{EXYNOS4_GPX3(3), 2},
	{EXYNOS4_GPX3(4), 3},
	{EXYNOS4_GPX3(5), 4},
};

static button_irq_t button_irq[] = {
	{0, IRQF_TRIGGER_FALLING, "Key1"},
	{0, IRQF_TRIGGER_FALLING, "Key2"},
	{0, IRQF_TRIGGER_FALLING, "Key3"},
	{0, IRQF_TRIGGER_FALLING, "Key4"},
};

static wait_queue_head_t button_wait_queue;

/*
 *static DECLEAR_WAIT_QUEUE_HEAD(button_wait_queue);
 */

static unsigned long key_val;
static volatile int ev_press = 0;

////////////////////////////////////////////////////////////////////////////字符设备框架
static void button_timer_handler(unsigned long data)
{
	button_t *button_desc = (button_t *)data;
	unsigned long pin_val;
	DRV_DEBUG("Driver: button timer handler\n");

	if (!button_desc) {
		return;
	}

	pin_val = gpio_get_value(button_desc->gpio);
	
	if (pin_val) {
		key_val = 0x80 | button_desc->val;
	} else {
		key_val = button_desc->val;
	}

	ev_press = 1;

	wake_up_interruptible(&button_wait_queue);
}

static irqreturn_t button_irq_handler(int irq, void *dev_id)
{
	button_irq_t *button_irq = (button_irq_t *)dev_id;
	DRV_DEBUG("Driver: button interrupt, irq: %d\n", irq);

	mod_timer(&button_irq->timer, jiffies + msecs_to_jiffies(BUTTON_DELAY_MS));

	return IRQ_HANDLED;
}

ssize_t driver_test_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	DRV_DEBUG("Driver: test write!\n");
	return size;
}

ssize_t driver_test_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	int ret = -1;
	DRV_DEBUG("Driver: test read!\n");
	if (size != 1) {
		return -EINVAL;
	}

	if (filp->f_flags & O_NONBLOCK) {
		if (!ev_press) {
			return -EAGAIN;
		}
	} else {
		wait_event_interruptible(button_wait_queue, ev_press);
	}
	ev_press = 0;

	ret = copy_to_user(buf, &key_val, 1);
	if (ret < 0) {
		DRV_ERROR("copy_to_user failed, ret = %d", ret);
		return -EFAULT;
	}

	return 0;
}

int driver_test_open (struct inode *inodep, struct file *filp)
{
	int i = 0;
	int ret = 0;
	DRV_DEBUG("Driver: test open!\n");

	for (i = 0; i < ARRAY_SIZE(button_desc); i++) {
		if (!button_desc[i].gpio) {
			continue;
		}

		setup_timer(&button_irq[i].timer, button_timer_handler, (unsigned long)&button_desc[i]);

		button_irq[i].irq_num = gpio_to_irq(button_desc[i].gpio);
		DRV_DEBUG("irq[%d] = %d", i, button_irq[i].irq_num);

		ret = request_irq(button_irq[i].irq_num, button_irq_handler, button_irq[i].flag, button_irq[i].name, &button_irq[i]);
		if (ret) {
			DRV_ERROR("request_irq irq[%d] failed, ret = %d", i, ret);
			break;
		}

	}
	if (ret) {
		--i;
		for (; i >= 0; i--) {
			if (!button_desc[i].gpio) {
				continue;
			}
			free_irq(button_irq[i].irq_num, (void *)&button_desc[i]);
			del_timer_sync(&button_irq[i].timer);
		}

		return -EBUSY;
	}

	init_waitqueue_head(&button_wait_queue);
	ev_press = 1;

	return 0;
}

int driver_test_close (struct inode *inodep, struct file *filp)
{
	int i = 0;
	DRV_DEBUG("Driver: test close!\n");

	for (i = 0; i < ARRAY_SIZE(button_desc); i++) {
		if (!button_desc[i].gpio) {
			continue;
		}
		free_irq(button_irq[i].irq_num, (void *)&button_desc[i]);

		del_timer_sync(&button_irq[i].timer);
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
};

int major = 0;
struct class *driver_class;
struct device *driver_class_device;

static int driver_test_init(void)
{
	DRV_DEBUG("Hello, driver chrdev register test begin!\n");

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

	driver_class_device = device_create(driver_class, NULL, MKDEV(major, 0), NULL, "driver_class_device");
	if (!driver_class_device) {
		DRV_ERROR("device_create failed");
		goto ERR_class_device_create;
	}

	DRV_DEBUG("major = %d\n", major);

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

