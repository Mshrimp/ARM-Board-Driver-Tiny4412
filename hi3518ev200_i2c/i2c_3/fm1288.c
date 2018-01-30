
#include <module.h>

#define	FM1288_NAME		"fm1288_i2c"


#define	fm1288_debug(fmt, args...)		\
			printk("fm1288 debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);
#define	fm1288_error(fmt, args...)		\
			printk("fm1288 error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);

static int __devinit fm1288_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct i2c_adapter *adapter = to_i2c_adapter(client->dev.parent);
	struct input_dev *idev;
	

	if (fm1288_initialize(client) < 0) {
		goto error_fm1288_initialize_client;
	}


}

/*
 *static int fm1288_resume(struct i2c_client *client)
 *{
 *
 *    
 *}
 */

static const struct i2c_device_id fm1288_ids[] = {
	{"fm1288", 0},
	{},
};

static struct i2c_driver i2c_fm1288_driver = {
	.driver = {
		.name = FM1288_NAME,
	},

	.probe = fm1288_probe,
	.remove = fm1288_remove,
    /*
	 *.suspend = fm1288_suspend,
	 *.resume = fm1288_resume,
     */
	.id_table = fm1288_ids,
};

static int __init init_fm1288(void)
{
	int ret = -1;
	fm1288_debug("driver init");

	ret = i2c_add_driver(&i2c_fm1288_driver);

	return ret;
}

static void __exit exit_fm1288(void)
{
	fm1288_debug("driver exit");

	i2c_del_driver(&i2c_fm1288_driver);
}

module_init(init_fm1288);
module_exit(exit_fm1288);

MODULE_LICENSE("GPL");

