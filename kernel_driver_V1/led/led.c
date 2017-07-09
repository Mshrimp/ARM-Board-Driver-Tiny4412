#include <linux/module.h>
#include <linux/fs.h>

#include "../include/board.h"
#include "head.h"
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

unsigned int led_all_status_get(void)
{
	return get_nbit_val(LED_DAT_ADDR, 0, LED_TOTLE_NUM);
}

void led_all_status_set(unsigned int led_status)
{
	set_nbits_val(LED_DAT_ADDR, 0, LED_TOTLE_NUM, led_status);
}

long led_test_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = -1;
	unsigned int ioarg;

	printk("led test ioctl!\n");

	if (_IOC_TYPE(cmd) != LED_IOC_TYPE)
	{
		printk("Driver: LED cmd type error!\n");
		return	-EINVAL;
	}

	if (_IOC_NR(cmd) > LED_IOC_MAX_NR)
	{
		printk("Driver: LED cmd error, No such cmd!\n");
		return	-EINVAL;
	}

/*
 *    if (_IOC_DIR(cmd) & _IOC_READ) {
 *        ret = access_ok(VERIFY_WRITE, (void *)args, _IOC_SIZE(cmd));
 *    } else {
 *        ret = access_ok(VERIFY_READ, (void *)args, _IOC_SIZE(cmd));
 *    }
 */

	switch(cmd)
	{
		case	LED_ALL_OFF:
			printk("ioctl: All LED Off\n");
			led_all_off();
			break;
		case	LED_ALL_ON:
			printk("ioctl: All LED On\n");
			led_all_on();
			break;
		case	LED1_OFF:
			printk("ioctl: LED1 Off\n");
			led_off(0);
			break;
		case	LED1_ON:
			printk("ioctl: LED1 On\n");
			led_on(0);
			break;
		case	LED2_OFF:
			printk("ioctl: LED2 Off\n");
			led_off(1);
			break;
		case	LED2_ON:
			printk("ioctl: LED2 On\n");
			led_on(1);
			break;
		case	LED3_OFF:
			printk("ioctl: LED3 Off\n");
			led_off(2);
			break;
		case	LED3_ON:
			printk("ioctl: LED3 On\n");
			led_on(2);
			break;
		case	LED4_OFF:
			printk("ioctl: LED4 Off\n");
			led_off(3);
			break;
		case	LED4_ON:
			printk("ioctl: LED4 On\n");
			led_on(3);
			break;
		case	LED_IOC_GET_DATA:
			printk("ioctl: LED get data\n");
			ioarg = led_all_status_get();
			ret = __put_user(ioarg, (int *)arg);
			break;
		case	LED_IOC_SET_DATA:
			printk("ioctl: LED set data\n");
			ret = __get_user(ioarg, (int *)arg);
			led_all_status_set(ioarg);
			printk("ioctl: LED set data, ioarg = %u\n", ioarg);
			break;

		default:
			printk("Driver: LED cmd error");
			return	-EINVAL;
	}
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

