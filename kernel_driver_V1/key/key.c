#include <linux/module.h>
#include <linux/fs.h>

#include "../include/board.h"
#include "../include/head.h"
#include "common.h"
#include "../led/led.h"
#include "key.h"


static volatile unsigned int *key_con_p = NULL;
static volatile unsigned int *key_dat_p = NULL;


/////////////////////////////////////////////////////////////////////裸板驱动

void key_config(void)
{
#if 1
	set_nbits_val(key_con_p, 2*4, 4*KEY_TOTLE_NUM, 0x0);	// Set GPIO as input
#else
	int i = 0;
	for (i = 0; i < KEY_TOTLE_NUM; i++)
	{
		set_nbits_val(key_con_p, (i+2)*4, 4, 0x0);	// Set GPIO to input
	}
#endif
}

unsigned int key_get_all_status(void)
{
	return get_nbits_val(key_dat_p, 2, KEY_TOTLE_NUM);
}

void key_init(void)
{
	int i = 0;
	int ret = -1;

	printk("Init pin\n");
	key_config();

	ret = key_get_all_status();
	printk("KEY: key all status = 0x%X\n", ret);
}

int key_get_val(int num)
{
	return get_bit_val(key_dat_p, num + 2);
}

int is_key_down(int num)
{
	return (key_get_val(num) ? 0 : 1);
}

void key_chk_status(void)
{
	int i = 0;
	int count = 0;
	int ret = -1;

	while(count < 5)
	{
		ret = key_get_all_status();
		printk("KEY: key all status = 0x%X\n", ret);

		for (i = 0; i < KEY_TOTLE_NUM; i++)
		{
			if (is_key_down(i))
			{
				printk("KEY: key %d down, count = %d\n", i, count);
				count++;
				if (count == 10)
				{
					break;
				}
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////中断

static irqreturn_t	irq_function(int irq, void *dev_id)
{
	printk("KEY: key_irq: %d\n", irq);

	return IRQ_HANDLED;
}

void key_irq_init(void)
{
	int irq_num1 = 0;
	int irq_num2 = 0;
	int irq_num3 = 0;
	int irq_num4 = 0;

	printk("KEY: irq_key init\n");

	// 申请中断号
	irq_num1 = gpio_to_irq(EXYNOS4_GPX3(2));
	irq_num2 = gpio_to_irq(EXYNOS4_GPX3(3));
	irq_num3 = gpio_to_irq(EXYNOS4_GPX3(4));
	irq_num4 = gpio_to_irq(EXYNOS4_GPX3(5));

	// 请求中断
	request_irq(irq_num1, irq_fuction, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "tiny4412_key1", (void *)"key1");
	request_irq(irq_num1, irq_fuction, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "tiny4412_key2", (void *)"key2");
	request_irq(irq_num1, irq_fuction, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "tiny4412_key3", (void *)"key3");
	request_irq(irq_num1, irq_fuction, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "tiny4412_key4", (void *)"key4");
}

void key_irq_exit(void)
{
	int irq_num1 = 0;
	int irq_num2 = 0;
	int irq_num3 = 0;
	int irq_num4 = 0;

	printk("KEY: irq_key init\n");

	// 申请中断号
	irq_num1 = gpio_to_irq(EXYNOS4_GPX3(2));
	irq_num2 = gpio_to_irq(EXYNOS4_GPX3(3));
	irq_num3 = gpio_to_irq(EXYNOS4_GPX3(4));
	irq_num4 = gpio_to_irq(EXYNOS4_GPX3(5));

	// 请求中断
	free_irq(irq_num1, (void *)"key1");
	free_irq(irq_num2, (void *)"key2");
	free_irq(irq_num3, (void *)"key3");
	free_irq(irq_num4, (void *)"key4");
}

////////////////////////////////////////////////////////////////////////////字符设备框架

long key_test_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk("Driver: KEY test ioctl!\n");

	if (_IOC_TYPE(cmd) != KEY_IOC_TYPE)
	{
		printk("Driver: KEY cmd type error!\n");
		return	-EINVAL;
	}

	if (_IOC_NR(cmd) > KEY_IOC_MAX_NR)
	{
		printk("Driver: KEY cmd error, No such cmd!\n");
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
		case 0:
			break;

		default:
			printk("Driver: KEY cmd error");
			return	-EINVAL;
	}
	return 0;
}

ssize_t key_test_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	printk("key test write!\n");
	return size;
}

ssize_t key_test_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	printk("key test read!\n");
	return size;
}

int key_test_open (struct inode *inodep, struct file *filp)
{
	printk("key test open!\n");
	return 0;
}

int key_test_close (struct inode *inodep, struct file *filp)
{
	printk("key test close!\n");
	return 0;
}

/////////////////////////////////////////////////////////////模块
struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = key_test_open,
	.release = key_test_close,
	.read = key_test_read,
	.write = key_test_write,
	.unlocked_ioctl = key_test_ioctl,
};

int major = 0;

static int key_test_init(void)
{
	int ret = -1;
	printk("Hello, key driver chrdev register test begin!\n");

	key_con_p = ((volatile unsigned int *)ioremap((GPX3CON_ADDR), 32));
	ERRP_K(key_con_p == NULL, "KEY", "key_con_p ioremap", goto ERR_ioremap1);

	key_dat_p = ((volatile unsigned int *)ioremap((GPX3DAT_ADDR), 32));
	ERRP_K(key_dat_p == NULL, "KEY", "key_dat_p ioremap", goto ERR_ioremap2);

	printk("Init key\n");
	key_init();

/*
 *    ret = register_chrdev(major, KEY_DEV_NAME, &fops);
 *    ERRP_K(ret < 0, "KEY", "register_chrdev", goto ERR_dev_register);
 *
 *    if (major == 0) {
 *        major = ret;
 *        printk("major = %d\n", major);
 *    } else {
 *        printk("major = %d\n", major);
 *    }
 */

	//key_chk_status();
	key_irq_init();

	return 0;
ERR_dev_register:
	key_irq_exit();
	iounmap(key_dat_p);
ERR_ioremap2:
	iounmap(key_con_p);
ERR_ioremap1:
	return -1;
}

static void key_test_exit(void)
{
	printk("Goodbye, key test over!\n");
	key_irq_exit();
	iounmap(key_con_p);
	iounmap(key_dat_p);
	//unregister_chrdev(major, KEY_DEV_NAME);
}


module_init(key_test_init);
module_exit(key_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mshrimp");
MODULE_DESCRIPTION("Test key");
MODULE_VERSION("V0.1");

