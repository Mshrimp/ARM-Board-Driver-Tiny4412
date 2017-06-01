#include "led.h"





void led_test_init(void)
{
	int ret = 0;
	printk("Module init\n");

	ret = register_chrdev(tdev.major, DEV_NAME, &tdev.fops);
	if (ret < 0)
	{
		return ret;
	}

	if (tdev.major == 0)
	{
		tdev.major = ret;
		printk("major = %d\n", tdev.major);
	}

	return 0;
}


void led_test_exit(void)
{
	printk("Cleanup module\n");
	unregister_chrdev(tdev.major, DEV_NAME);
}


module_init(led_test_init);
module_exit(led_test_exit);
MODULE_LICENSE("GPL");


