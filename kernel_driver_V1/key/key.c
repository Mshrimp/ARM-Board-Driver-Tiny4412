#include <linux/module.h>
#include <linux/fs.h>


long key_test_ioctl (struct file *filp, unsigned int cmd, unsigned long size)
{
	printk("key test ioctl!\n");
	return 0;
}

ssize_t key_test_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	printk("key test write!\n");
	return 0;
}

ssize_t key_test_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	printk("key test read!\n");
	return 0;
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

struct file_operations fops = {
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

	ret = register_chrdev(major, "key_test_module", &fops);
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

static void key_test_exit(void)
{
	printk("Goodbye, key test over!\n");
	unregister_chrdev(major, "key_test_module");
}


module_init(key_test_init);
module_exit(key_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mshrimp");
MODULE_DESCRIPTION("Test key");
MODULE_VERSION("V0.1");

