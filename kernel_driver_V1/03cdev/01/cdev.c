#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
// 每个cdev至少要有一个设备号
// 主设备号+次设备号=设备号
// 同一类设备的主设备号是相同的

// include/linux/xxx.h

dev_t devno = 0;
/****************************************************************/
static __init int my_cdev_init(void)
{
	int ret = 0;
	printk("cdev_init\n");
#if 0
	devno = MKDEV(222, 2);
	ret = register_chrdev_region(devno, 1, "my_dev");
#else
	ret = alloc_chrdev_region(&devno, 1, 1, "my_dev");
#endif
	if (ret < 0) {
		goto register_error;
	}

	return 0;
register_error:
	return ret;
}

static __exit void my_cdev_exit(void)
{
	printk("cdev_exit\n");

	unregister_chrdev_region(devno, 1);

}

module_init(my_cdev_init);
module_exit(my_cdev_exit);

MODULE_DESCRIPTION("Driver for eeprom, at2402 device");
MODULE_AUTHOR("Mshrimp");
MODULE_LICENSE("GPL");

