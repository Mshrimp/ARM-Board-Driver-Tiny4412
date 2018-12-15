#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
// 每个cdev至少要有一个设备号
// 主设备号+次设备号=设备号
// 同一类设备的主设备号是相同的

// include/linux/xxx.h

dev_t devno = 0;

struct mydev_st {
	char buf[256];
	dev_t no;
	struct cdev dev;
	int count;
};

// 每个文件对应一个inode	唯一
// 每次打开都对应一个file	不唯一
// /dev/test/mydev
int my_open(struct inode *inode, struct file *fp)
{
	struct mydev_st *m;
	m = container_of(inode->i_cdev, struct mydev_st, dev);
	memset(m->buf, 0, 256);

	return 0;
}
int my_close(struct inode *inode, struct file *fp)
{
	struct mydev_st *m;
	m = container_of(inode->i_cdev, struct mydev_st, dev);

	return 0;
}
ssize_t my_read(struct file *fp, char __user *buffer, size_t count, loff_t *off)
{
	struct mydev_st *m;
	char buf[256];
	int ret = 0;

	m = fp->private_data;

	count = min((int)count, 256);

	memcpy(buf, m->buf, count);

	ret = copy_to_user(buffer, buf, count);
	if (ret) {
		ret = -EFAULT;
		goto copy_error;
	}

	return 0;
copy_error:
	return ret;
}

ssize_t my_write(struct file *fp, const char __user *buffer, size_t count, loff_t *off)
{
	struct mydev_st *m;
	int ret = 0;

	m = fp->private_data;
	count = min((int)count, 256);

	ret = copy_from_user(m->buf, buffer, count);
	if (ret) {
		ret = -EFAULT;
		goto copy_error;
	}

	return 0;
copy_error:
	return ret;
}

long my_ioctl(struct file *fp, unsigned int cmd, unsigned long data)
{
	return 0;
}

struct file_operations my_ops = {
	.open = my_open,
	.release = my_close,
	.read = my_read,
	.write = my_write,
	.unlocked_ioctl = my_ioctl,
};

struct mydev_st *mydev;

/****************************************************************/
static __init int my_cdev_init(void)
{
	int ret = 0;
	printk("cdev_init\n");

	mydev = kzalloc(sizeof(*mydev), GFP_KERNEL);
	if (!mydev) {
		ret = -ENOMEM;
		goto alloc_mydev_error;
	}
	
	ret = alloc_chrdev_region(&mydev->no, 1, 1, "my_dev");
	if (ret < 0) {
		goto register_error;
	}

	cdev_init(&mydev->dev, &my_ops);
	ret = cdev_add(&mydev->dev, mydev->no, 1);
	if (ret < 0) {
		goto cdev_add_error;
	}
	printk("major: %d, minor: %d\n", MAJOR(mydev->no), MINOR(mydev->no));

	return 0;
cdev_add_error:
	unregister_chrdev_region(mydev->no, 1);
register_error:
	kfree(mydev);
alloc_mydev_error:
	return ret;
}

static __exit void my_cdev_exit(void)
{
	printk("cdev_exit\n");

	cdev_del(&mydev->dev);
	unregister_chrdev_region(devno, 1);

}

module_init(my_cdev_init);
module_exit(my_cdev_exit);

MODULE_DESCRIPTION("Driver for eeprom, at2402 device");
MODULE_AUTHOR("Mshrimp");
MODULE_LICENSE("GPL");

