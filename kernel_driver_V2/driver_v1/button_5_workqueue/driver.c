#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <mach/gpio.h>
#include <asm/io.h>
#include "common.h"

#include "key.h"

#define DEV_NAME	"test_driver"

struct tasklet_struct task;
struct workqueue_struct *mywork;

struct work_struct work;

/////////////////////////////////////////////////////////////////////裸板驱动


////////////////////////////////////////////////////////////////////////////字符设备框架

static void task_function(unsigned long data)
{
	if (in_interrupt()) {
		printk("Driver: %s is in interrupt handle\n", __func__);
	}
}

static void mywork_function(struct work_struct *work)
{
	if (in_interrupt()) {
		printk("Driver: %s is in interrupt handle\n", __func__);
	}
	msleep(2);
	printk("Driver: %s is in process handle\n", __func__);
}

static irqreturn_t button_irq(int irq, void *dev_id)
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
	int irq_num1 = 0;
    /*
	 *int irq_num2 = 0;
	 *int irq_num3 = 0;
	 *int irq_num4 = 0;
     */
	int data_t = 100;
	int ret = -1;

	printk("Hello, driver chrdev register test begin!\n");

/*
 *    major = register_chrdev(major, DEV_NAME, &fops);
 *    ERRP_K(major < 0, "Driver", "register_chrdev", goto ERR_dev_register);
 *
 *    driver_class = class_create(THIS_MODULE, "driver_class");
 *    ERRP_K(driver_class == NULL, "Driver", "class_create", goto ERR_class_create);
 *
 *    driver_class_device = device_create(driver_class, NULL, MKDEV(major, 0), NULL, "driver_class_device");
 *    ERRP_K(driver_class_device == NULL, "Driver", "class_device_create", goto ERR_class_device_create);
 */

    /*
	 *printk("major = %d\n", major);
     */

	mywork = create_workqueue("my work");
	INIT_WORK(&work, mywork_function);
	queue_work(mywork, &work);

	tasklet_init(&task, task_function, data_t);

	irq_num1 = gpio_to_irq(EXYNOS4_GPX3(2));
	printk("Init: irq_num1 = %d\n", irq_num1);

/*
 *    irq_num2 = gpio_to_irq(EXYNOS4_GPX3(3));
 *    printk("Init: irq_num2 = %d\n", irq_num2);
 *
 *    irq_num3 = gpio_to_irq(EXYNOS4_GPX3(4));
 *    printk("Init: irq_num3 = %d\n", irq_num3);
 *
 *    irq_num4 = gpio_to_irq(EXYNOS4_GPX3(5));
 *    printk("Init: irq_num4 = %d\n", irq_num4);
 */

	ret = request_irq(irq_num1, button_irq, IRQF_TRIGGER_FALLING, "tiny4412_key1", (void *)"key1");
	if (ret) {
		printk("Init: request_irq irq_num1 failed\n");
	}
	
    /*
	 *ret = request_irq(irq_num2, button_irq, IRQF_TRIGGER_FALLING, "tiny4412_key2", (void *)"key2");
	 *if (ret) {
	 *    printk("Init: request_irq irq_num2 failed\n");
	 *}
	 *
	 *ret = request_irq(irq_num3, button_irq, IRQF_TRIGGER_FALLING, "tiny4412_key3", (void *)"key3");
	 *if (ret) {
	 *    printk("Init: request_irq irq_num3 failed\n");
	 *}
	 *
	 *ret = request_irq(irq_num4, button_irq, IRQF_TRIGGER_FALLING, "tiny4412_key4", (void *)"key4");
	 *if (ret) {
	 *    printk("Init: request_irq irq_num4 failed\n");
	 *}
     */
	
	return 0;
/*
 *ERR_class_device_create:
 *    class_destroy(driver_class);
 *ERR_class_create:
 *    unregister_chrdev(major, DEV_NAME);
 *ERR_dev_register:
 *    return -1;
 */
}

static void driver_test_exit(void)
{
	int irq_num1 = 0;
    /*
	 *int irq_num2 = 0;
	 *int irq_num3 = 0;
	 *int irq_num4 = 0;
     */

	printk("Goodbye, test over!\n");

	irq_num1 = gpio_to_irq(EXYNOS4_GPX3(2));
	printk("Exit: irq_num1 = %d\n", irq_num1);

/*
 *    irq_num2 = gpio_to_irq(EXYNOS4_GPX3(3));
 *    printk("Init: irq_num2 = %d\n", irq_num2);
 *
 *    irq_num3 = gpio_to_irq(EXYNOS4_GPX3(4));
 *    printk("Init: irq_num3 = %d\n", irq_num3);
 *
 *    irq_num4 = gpio_to_irq(EXYNOS4_GPX3(5));
 *    printk("Init: irq_num4 = %d\n", irq_num4);
 */

 	destroy_workqueue(mywork);

	free_irq(irq_num1, (void *)"key1");
    /*
	 *free_irq(irq_num2, (void *)"key2");
	 *free_irq(irq_num3, (void *)"key3");
	 *free_irq(irq_num4, (void *)"key4");
     */

    /*
	 *device_destroy(driver_class, MKDEV(major, 0));
	 *class_destroy(driver_class);
	 *unregister_chrdev(major, DEV_NAME);
     */
}


module_init(driver_test_init);
module_exit(driver_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mshrimp");
MODULE_DESCRIPTION("Test driver");
MODULE_VERSION("V0.1");

