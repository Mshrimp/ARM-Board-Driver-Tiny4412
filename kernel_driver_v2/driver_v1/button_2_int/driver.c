#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include "common.h"

#include "key.h"

#define DEV_NAME	"test_driver"

volatile unsigned long *key_con_p = NULL;
volatile unsigned long *key_dat_p = NULL;
volatile unsigned long *key_int_con_p = NULL;
volatile unsigned long *key_int_mask_p = NULL;
volatile unsigned long *key_int_pend_p = NULL;

/////////////////////////////////////////////////////////////////////裸板驱动


////////////////////////////////////////////////////////////////////////////字符设备框架

static irqreturn_t button_irq(int irq, void *dev_id)
{
	unsigned int key_val = 0;

	printk("Driver: button interrupt\n");

	key_val = *key_int_pend_p;


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
	unsigned char key_vals[KEY_TOTLE] = { 0 };
	unsigned int key_val = 0;
	int ret = 0;
	int i = 0;
	//printk("Driver: test read!\n");

	/* Read key pin value */
	key_val = *key_dat_p;

	for (i = 0; i < KEY_TOTLE; i++) {
		key_vals[i] = (key_val & (0x1 << (i + 2))) ? 1 : 0;
	}


	ret = copy_to_user(buf, key_vals, sizeof(key_vals));

	return sizeof(key_vals);
}

int driver_test_open (struct inode *inodep, struct file *filp)
{
	int tmp = 0;
	int ret = 0;
	printk("Driver: test open!\n");

	/* Config GPX3_2 ~ GPX3_5 as interrupt */
	*key_con_p |= (0xFFFF << 2*4);

	/* Config GPX3_2 ~ GPX3_5 interrupt as low level trigger */
	*key_int_con_p &= ~(0xFFFF << 2*4);

	/* Enable GPX3_2 ~ GPX3_5 (XEINT26 ~ XEINT29) Interrupt */
	*key_int_mask_p &= ~(0xF << 2);

	/* Register IRQ handler */
	//if (request_irq(KEY_IRQ_NUM, button_irq, IRQF_SHARED, "driver_key", NULL)) {
	ret = request_irq(KEY_IRQ_NUM, button_irq, IRQF_TRIGGER_RISING | IRQF_SHARED, "driver_key", &tmp);
	if (ret) {
		printk("Driver: request_irq failed! ret = %d\n", ret);
		return -1;
	}


	//printk("Driver: open key_con_p: 0x%X\n", *key_con_p);

	return 0;
}

int driver_test_close (struct inode *inodep, struct file *filp)
{
	printk("Driver: test close!\n");
	free_irq(KEY_IRQ_NUM, NULL);
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
	printk("Hello, driver chrdev register test begin!\n");

	major = register_chrdev(major, DEV_NAME, &fops);
	ERRP_K(major < 0, "Driver", "register_chrdev", goto ERR_dev_register);

	driver_class = class_create(THIS_MODULE, "driver_class");
	ERRP_K(driver_class == NULL, "Driver", "class_create", goto ERR_class_create);

	driver_class_device = device_create(driver_class, NULL, MKDEV(major, 0), NULL, "driver_class_device");
	ERRP_K(driver_class_device == NULL, "Driver", "class_device_create", goto ERR_class_device_create);

	printk("major = %d\n", major);
	
	key_con_p = (volatile unsigned long *)ioremap(KEY_CON_ADDR, 16);
	ERRP_K(NULL == key_con_p, "Driver", "key_con_p ioremap", goto ERR_ioremap_key_con);
	key_dat_p = key_con_p + 1;
	key_int_con_p = (volatile unsigned long *)ioremap(KEY_INT_CON, 4);
	ERRP_K(NULL == key_int_con_p, "Driver", "key_int_con_p ioremap", goto ERR_ioremap_key_int_con);
	key_int_mask_p = (volatile unsigned long *)ioremap(KEY_INT_MASK, 4);
	ERRP_K(NULL == key_int_mask_p, "Driver", "key_int_mask_p ioremap", goto ERR_ioremap_key_int_mask);
	key_int_pend_p = (volatile unsigned long *)ioremap(KEY_INT_PEND, 4);
	ERRP_K(NULL == key_int_pend_p, "Driver", "key_int_pend_p ioremap", goto ERR_ioremap_key_int_pend);

	return 0;
ERR_ioremap_key_int_pend:
	iounmap(key_int_mask_p);
ERR_ioremap_key_int_mask:
	iounmap(key_int_con_p);
ERR_ioremap_key_int_con:
	iounmap(key_con_p);
ERR_ioremap_key_con:
	//device_unregister(driver_class_device);
	device_destroy(driver_class, MKDEV(major, 0));
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

	iounmap(key_con_p);
	iounmap(key_int_con_p);
	iounmap(key_int_mask_p);
	iounmap(key_int_pend_p);
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

