#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/cdev.h>
// 每个cdev至少要有一个设备号
// 主设备号+次设备号=设备号
// 同一类设备的主设备号是相同的

// include/linux/xxx.h

/****************************************************************/
static __init int cdev_init(void)
{
	printk("cdev_init\n");

	return 0;
}

static __exit void cdev_exit(void)
{
	printk("cdev_exit\n");

}

module_init(cdev_init);
module_exit(cdev_exit);

MODULE_DESCRIPTION("Driver for eeprom, at2402 device");
MODULE_AUTHOR("Mshrimp");
MODULE_LICENSE("GPL");

