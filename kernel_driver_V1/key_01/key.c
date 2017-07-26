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
	set_nbits_val(key_con_p, 2*4, 4*KEY_TOTLE_NUM, 0x0);	// Set GPIO as output
#else
	int i = 0;
	for (i = 0; i < KEY_TOTLE_NUM; i++)
	{
		set_nbits_val(key_con_p, i*4, 4, 0x01);	// Set GPIO to output
	}
#endif
}

void key_init(void)
{
	printk("Init pin\n");
	key_config();
}

int key_get_val(int num)
{
	return get_bit_val(key_dat_p, num + 2);
}

int is_key_down(int num)
{
	int ret = -1;

	ret = key_get_val(num);
	if (ret == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	// return (key_get_val(num) ? 0 : 1);
}

int sure_key_down(int num)
{
	if (is_key_down(num))
	{
		msleep(10);
		if (is_key_down(num))
		{
			return 1;
		}
	}

	return 0;
}

unsigned int key_all_status_get(void)
{
	return get_nbits_val(key_dat_p, 2, KEY_TOTLE_NUM);
}

void key_chk_status(void)
{
	int i = 0;
	int count = 0;
	int ret = -1;

	while(count < 10)
	{
		ret = key_all_status_get();
		printk("KEY: key all status = 0x%X\n", ret);

		for (i = 0; i < KEY_TOTLE_NUM; i++)
		{
			if (sure_key_down(i))
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

	key_con_p = ((volatile unsigned int *)ioremap((GPX3CON_OFFSET), 32));
	ERRP_K(key_con_p == NULL, "KEY", "key_con_p ioremap", goto ERR_ioremap1);

	key_dat_p = ((volatile unsigned int *)ioremap((GPX3DAT_OFFSET), 32));
	ERRP_K(key_dat_p == NULL, "KEY", "key_dat_p ioremap", goto ERR_ioremap2);

	printk("Init key\n");
	key_init();

	ret = register_chrdev(major, KEY_DEV_NAME, &fops);
	ERRP_K(ret < 0, "KEY", "register_chrdev", goto ERR_dev_register);

	if (major == 0) {
		major = ret;
		printk("major = %d\n", major);
	} else {
		printk("major = %d\n", major);
	}

	key_chk_status();

	return 0;
ERR_dev_register:
	iounmap(key_dat_p);
ERR_ioremap2:
	iounmap(key_con_p);
ERR_ioremap1:
	return -1;
}

static void key_test_exit(void)
{
	printk("Goodbye, key test over!\n");
	iounmap(key_con_p);
	iounmap(key_dat_p);
	unregister_chrdev(major, KEY_DEV_NAME);
}


module_init(key_test_init);
module_exit(key_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mshrimp");
MODULE_DESCRIPTION("Test key");
MODULE_VERSION("V0.1");

