#include <linux/module.h>
#include <linux/fs.h>

#include "../include/board.h"
#include "led.h"


void led_all_off(void)
{
	set_nbits_val(LED_DAT_ADDR, 0, LED_TOTLE_NUM, (0x01<<LED_TOTLE_NUM)-1);	// Set all GPIO output high
}

void led_all_on(void)
{
	set_nbits_val(LED_DAT_ADDR, 0, LED_TOTLE_NUM, 0x0);	// Set all GPIO output low
}


void led_on(unsigned int num)
{
	set_bit_val(LED_DAT_ADDR, num, 0);		// Set GPIO output low, LED ON
}


void led_off(unsigned int num)
{
	set_bit_val(LED_DAT_ADDR, num, 1);		// Set GPIO output high, LED OFF
}

void led_init(void)
{
	int i = 0;

	for (i = 0; i < LED_TOTLE_NUM; i++)
	{
		set_nbits_val(LED_CON_ADDR, i*4, 4, 0x01);	// Set GPIO to output
	}

	led_all_off();
}




long led_test_ioctl (struct file *filp, unsigned int cmd, unsigned long size)
{
	printk("led test ioctl!\n");
	return 0;
}

ssize_t led_test_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	printk("led test write!\n");
	return size;
}

ssize_t led_test_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	printk("led test read!\n");
	return size;
}

int led_test_open (struct inode *inodep, struct file *filp)
{
	printk("led test open!\n");
	return 0;
}

int led_test_close (struct inode *inodep, struct file *filp)
{
	printk("led test close!\n");
	return 0;
}

struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = led_test_open,
	.release = led_test_close,
	.read = led_test_read,
	.write = led_test_write,
	.unlocked_ioctl = led_test_ioctl,
};

int major = 0;

static int led_test_init(void)
{
	int ret = -1;
	printk("Hello, led driver chrdev register test begin!\n");

	ret = register_chrdev(major, "led_test_module", &fops);
	if (ret < 0)
	{
		return ret;
	}

	if (major == 0) {
		major = ret;
		printk("major = %d\n", major);
	} else {
		printk("major = %d\n", major);
	}

	return 0;
}

static void led_test_exit(void)
{
	printk("Goodbye, led test over!\n");
	unregister_chrdev(major, "led_test_module");
}


module_init(led_test_init);
module_exit(led_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mshrimp");
MODULE_DESCRIPTION("Test led");
MODULE_VERSION("V0.1");

