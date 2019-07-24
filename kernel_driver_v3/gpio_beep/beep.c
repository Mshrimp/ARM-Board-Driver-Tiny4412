#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#define BEEP_CONF_ADDR 0x114000A0

#define BEEP_SWITCH	_IOW('B', 0x12, int)

struct tiny4412_beep {
	unsigned int dev_major;
	struct class *cls;
	struct device *dev;
	int value;
};

struct tiny4412_beep *beep_dev;

volatile unsigned long *gpd0_conf;
volatile unsigned long *gpd0_data;



static int beep_open(struct inode *inode, struct file *filp)
{
	printk("%s, beep_open!\n", __func__);

	*gpd0_conf &= ~0x0f;
	*gpd0_conf |= 0x01;

	return 0;
}

static long beep_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int nr = _IOC_NR(cmd);

	printk("%s, beep_ioctl, cmd: 0x%X\n", __func__, cmd);

	switch(nr)
	{
		case 0x12:
		if (arg) {
			*gpd0_data |= 0x01;
		} else {
			*gpd0_data &= ~0x01;
		}

		break;
	}

	return 0;
}

static int beep_close(struct inode *inode, struct file *filp)
{
	printk("%s, beep_close!\n", __func__);

	*gpd0_data &= ~0x01;

	return 0;
}

struct file_operations beep_fops = {
	.owner = THIS_MODULE,
	.open = beep_open,
	.release = beep_close,
	//.read = beep_read,
	//.write = beep_write,
	.unlocked_ioctl = beep_ioctl,
};

static __init int beep_init(void)
{
	int ret = 0;

	printk("%s, driver init!\n", __func__);
	
	beep_dev = kzalloc(sizeof(struct tiny4412_beep), GFP_KERNEL);
	if (NULL == beep_dev) {
		printk("%s, kzalloc failed!\n", __func__);
		return -ENOMEM;
	}

	beep_dev->dev_major = register_chrdev(0, "beep_drv", &beep_fops);
	if (beep_dev->dev_major < 0) {
		printk("%s, register failed!\n", __func__);
		goto register_chrdev_err;
	}

	beep_dev->cls = class_create(THIS_MODULE, "beep_cls");
	if (IS_ERR(beep_dev->cls)) {
		printk("%s, class_create failed!\n", __func__);
		ret = PTR_ERR(beep_dev);
		goto class_create_err;
	}

	beep_dev->dev = device_create(beep_dev->cls, NULL, MKDEV(beep_dev->dev_major, 0), NULL, "beep_device");
	if (IS_ERR(beep_dev->dev)) {
		printk("%s, device_create failed!\n", __func__);
		ret = PTR_ERR(beep_dev->dev);
		goto device_create_err;
	}

	gpd0_conf = ioremap(BEEP_CONF_ADDR, 8);
	gpd0_data = gpd0_conf + 1;

	return 0;

device_create_err:
	class_destroy(beep_dev->cls);
class_create_err:
	unregister_chrdev(beep_dev->dev_major, "beep_drv");
register_chrdev_err:
	kfree(beep_dev);
	return ret;
}

static __exit void beep_exit(void)
{
	printk("%s, driver exit!\n", __func__);

	device_destroy(beep_dev->cls, MKDEV(beep_dev->dev_major, 0));
	class_destroy(beep_dev->cls);
	unregister_chrdev(beep_dev->dev_major, "beep_drv");

	iounmap(gpd0_conf);
	kfree(beep_dev);
}

module_init(beep_init);
module_exit(beep_exit);

MODULE_LICENSE("GPL");

