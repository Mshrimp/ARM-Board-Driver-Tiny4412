#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <mach/gpio.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/workqueue.h>

#include "common.h"

#include "key.h"

#define DEV_NAME	"test_button"

typedef struct {
	int gpio;
	int irq;
	char name[32];
} key_irq_t; 

key_irq_t key_irq[KEY_TOTLE] = {
	{EXYNOS4_GPX3(2), 0, "tiny4412_key1"},
	{EXYNOS4_GPX3(3), 0, "tiny4412_key2"},
	{EXYNOS4_GPX3(4), 0, "tiny4412_key3"},
	{EXYNOS4_GPX3(5), 0, "tiny4412_key4"},
};

struct tasklet_struct task;

struct work_struct work;
struct workqueue_struct *mywork;

////////////////////////////////////////////////////////////////////////////字符设备框架
// In interrupt
static void task_function(unsigned long data)
{
	if (in_interrupt()) {
		printk("Driver: %s is in interrupt handle\n", __func__);
	}
}

// In process
static void mywork_function(struct work_struct *work)
{
	if (in_interrupt()) {
		printk("Driver: %s is in interrupt handle\n", __func__);
	}

	msleep(20);

	printk("Driver: %s is in process handle\n", __func__);

}

// In interrupt
static irqreturn_t button_irq_handler(int irq, void *dev_id)
{
	printk("Driver: button interrupt, irq: %d\n", irq);

	tasklet_schedule(&task);
	schedule_work(&work);

	if (in_interrupt()) {
		printk("Driver: %s is in interrupt handle\n", __func__);
	}

	printk("button_irq: %d\n", irq);

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
	printk("Driver: test read!\n");

	return 0;
}

int driver_test_open (struct inode *inodep, struct file *filp)
{
	int data_t = 100;
	int i = 0;
	int ret = -1;

	printk("Driver: test open!\n");

	mywork = create_workqueue("my work");
	INIT_WORK(&work, mywork_function);
	queue_work(mywork, &work);

	tasklet_init(&task, task_function, data_t);


	for (i = 0; i < ARRAY_SIZE(key_irq); i++)
	{
		if (!key_irq[i].gpio)
		{
			printk("error, key_irq[%d].gpio: %d\n", i, key_irq[i].gpio);
			continue;
		}

		key_irq[i].irq = gpio_to_irq(key_irq[i].gpio);
		printk("Driver open: key_irq[%d].irq: %d\n", i, key_irq[i].irq);

		ret = request_irq(key_irq[i].irq, button_irq_handler, IRQF_TRIGGER_FALLING, key_irq[i].name, (void *)&key_irq[i]);
		if (ret) {
			printk("Driver open: request_irq key_irq[%d] failed, ret: %d\n", i, ret);
			break;
		}
	
	}

	if (ret) {
		i--;
		for(; i >= 0; i--) {
			if (!key_irq[i].gpio) {
				printk("error, key_irq[%d].gpio: %d\n", i, key_irq[i].gpio);
				continue;
			}
			free_irq(key_irq[i].irq, (void *)&key_irq[i]);
		}

		return -1;
	}

	return 0;
}

int driver_test_close (struct inode *inodep, struct file *filp)
{
	int i = 0;

	printk("Driver: test close!\n");

	destroy_workqueue(mywork);

	for (i = 0; i < ARRAY_SIZE(key_irq); i++)
	{
		if (!key_irq[i].gpio) {
			printk("error, key_irq[%d].gpio: %d\n", i, key_irq[i].gpio);
			continue;
		}
		free_irq(key_irq[i].irq, (void *)&key_irq[i]);
		printk("Driver close: free_irq key_irq[%d]\n", i);
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
	.unlocked_ioctl = driver_test_ioctl,
};

static int major = 0;
static struct class *driver_class;
static struct device *driver_class_device;

static int driver_test_init(void)
{
	printk("Hello, driver chrdev register test begin!\n");

	major = register_chrdev(major, DEV_NAME, &fops);
	ERRP_K(major < 0, "Driver", "register_chrdev", goto ERR_dev_register);

	driver_class = class_create(THIS_MODULE, "button_class");
	ERRP_K(driver_class == NULL, "Driver", "class_create", goto ERR_class_create);

	driver_class_device = device_create(driver_class, NULL, MKDEV(major, 0), NULL, "button_device");
	ERRP_K(driver_class_device == NULL, "Driver", "class_device_create", goto ERR_class_device_create);

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
	printk("Goodbye, test over!\n");

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

