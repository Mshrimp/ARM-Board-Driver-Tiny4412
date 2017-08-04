#include <linux/module.h>
#include <linux/fs.h>

#include "../include/board.h"
#include "../include/head.h"
#include "common.h"
#include "adc.h"


static volatile unsigned int *adc_cfg_p = NULL;
static volatile unsigned int *adc_con_p = NULL;
static volatile unsigned int *adc_dat_p = NULL;
static volatile unsigned int *adc_mux_p = NULL;


/////////////////////////////////////////////////////////////////////裸板驱动

void adc_conv_normal(void)
{
	set_bit_val(adc_con_p, 2, ADCCON_CONV_NORMAL);
}

void adc_conv_standby(void)
{
	set_bit_val(adc_con_p, 2, ADCCON_CONV_STANDBY);
}

int adc_config(void)
{
	//set_bit_val(adc_cfg_p, 16, ADC_GENERAL);
	set_bit_val(adc_con_p, 16, ADCCON_12BIT_CONV);
	set_bit_val(adc_con_p, 14, ADCCON_CONV_PRESC_ENABLE);
	set_nbits_val(adc_con_p, 6, 8, 19);
	adc_conv_normal();
	//printk("ADC: adc_config, get val = 0x%X\n", get_nbits_val(adc_con_p, 6, 8));
	return 0;
}

int is_adc_conv_end(void)
{
	return get_bit_val(adc_con_p, 15);
}

void adc_conv_read_enable(void)
{
	set_bit_val(adc_con_p, 1, ADCCON_CONV_READ_START_ENABLE);
}

void adc_conv_read_disable(void)
{
	set_bit_val(adc_con_p, 1, ADCCON_CONV_READ_START_DISABLE);
}

void adc_conv_start_enable(void)
{
	set_bit_val(adc_con_p, 0, ADCCON_CONV_START_ENABLE);
}

unsigned int adc_get_data(void)
{
	return get_nbits_val(adc_dat_p, 0, ADCDAT_12BIT);
}

void adc_set_channel(int num)
{
	set_nbits_val(adc_mux_p, 0, ADCMUX_AIN_BIT, num);
}

unsigned long adc_get_con_val(void)
{
	return get_nbits_val(adc_con_p, 0, 17);
}

int adc_init(void)
{
	int ret = -1;

	printk("ADC: Init pin\n");
	ret = adc_config();
	if (ret < 0)
	{
		printk("ADC: init, adc_config failed\n");
		return -1;
	}

	adc_set_channel(0);
	printk("ADC: set channel 0\n");

	return 0;
}

void adc_conv(void)
{
	int ret = -1;
	int i = 0;
	int data = 0;
	int count = 0;
	unsigned long result = 0;

	adc_conv_read_enable();
	adc_conv_start_enable();
	printk("ADC: start conv\n");

	while(i < 10 && count < 10)
	{
		result = adc_get_con_val();
		printk("ADC: result = 0x%lX, i = %d, count = %d\n", result, i, count);
		msleep(1000);
		printk("ADC: result = 0x%lX, i = %d, count = %d\n", result, i, count);
		count++;
		if (is_adc_conv_end())
		{
			printk("ADC: conv end\n");
			data = adc_get_data();
			printk("ADC: get data = %d, i = %d\n", data, i);
			i++;
			msleep(1000);
			adc_conv_start_enable();
			printk("ADC: start conv, i = %d\n", i);
		}
	}
}

////////////////////////////////////////////////////////////////////////////字符设备框架

long adc_test_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk("Driver: ADC test ioctl!\n");

/*
 *    if (_IOC_TYPE(cmd) != ADC_IOC_TYPE)
 *    {
 *        printk("Driver: ADC cmd type error!\n");
 *        return	-EINVAL;
 *    }
 *
 *    if (_IOC_NR(cmd) > ADC_IOC_MAX_NR)
 *    {
 *        printk("Driver: ADC cmd error, No such cmd!\n");
 *        return	-EINVAL;
 *    }
 */

/*
 *    if (_IOC_DIR(cmd) & _IOC_READ) {
 *        ret = access_ok(VERIFY_WRITE, (void *)args, _IOC_SIZE(cmd));
 *    } else {
 *        ret = access_ok(VERIFY_READ, (void *)args, _IOC_SIZE(cmd));
 *    }
 */

/*
 *    switch(cmd)
 *    {
 *        case 0:
 *            break;
 *
 *        default:
 *            printk("Driver: ADC cmd error");
 *            return	-EINVAL;
 *    }
 */
	return 0;
}

ssize_t adc_test_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	printk("adc test write!\n");
	return size;
}

ssize_t adc_test_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	printk("adc test read!\n");
	return size;
}

int adc_test_open (struct inode *inodep, struct file *filp)
{
	printk("adc test open!\n");
	return 0;
}

int adc_test_close (struct inode *inodep, struct file *filp)
{
	printk("adc test close!\n");
	return 0;
}

/////////////////////////////////////////////////////////////模块
struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = adc_test_open,
	.release = adc_test_close,
	.read = adc_test_read,
	.write = adc_test_write,
	.unlocked_ioctl = adc_test_ioctl,
};

int major = 0;

static int adc_test_init(void)
{
	int ret = -1;
	printk("Hello, adc driver chrdev register test begin!\n");

	adc_con_p = ((volatile unsigned int *)ioremap((ADCCON_ADDR), 32));
	ERRP_K(adc_con_p == NULL, "ADC", "adc_con_p ioremap", goto ERR_ioremap1);

	adc_dat_p = ((volatile unsigned int *)ioremap((ADCDAT_ADDR), 32));
	ERRP_K(adc_dat_p == NULL, "ADC", "adc_dat_p ioremap", goto ERR_ioremap2);

	adc_mux_p = ((volatile unsigned int *)ioremap((ADCMUX_ADDR), 32));
	ERRP_K(adc_mux_p == NULL, "ADC", "adc_mux_p ioremap", goto ERR_ioremap3);

	adc_cfg_p = ((volatile unsigned int *)ioremap((ADCMUX_ADDR), 32));
	ERRP_K(adc_cfg_p == NULL, "ADC", "adc_cfg_p ioremap", goto ERR_ioremap4);

	printk("ADC: Init adc\n");
	ret = adc_init();
	ERRP_K(ret < 0, "ADC", "adc_init", goto ERR_init);

	adc_conv();

/*
 *    ret = register_chrdev(major, ADC_DEV_NAME, &fops);
 *    ERRP_K(ret < 0, "ADC", "register_chrdev", goto ERR_dev_register);
 *
 *    if (major == 0) {
 *        major = ret;
 *        printk("major = %d\n", major);
 *    } else {
 *        printk("major = %d\n", major);
 *    }
 */


	return 0;

ERR_init:
	iounmap(adc_cfg_p);
ERR_ioremap4:
	iounmap(adc_mux_p);
ERR_ioremap3:
	iounmap(adc_dat_p);
ERR_ioremap2:
	iounmap(adc_con_p);
ERR_ioremap1:
	return -1;
}

static void adc_test_exit(void)
{
	printk("Goodbye, adc test over!\n");
	iounmap(adc_con_p);
	iounmap(adc_dat_p);
	iounmap(adc_mux_p);
	iounmap(adc_cfg_p);
	//unregister_chrdev(major, ADC_DEV_NAME);
}


module_init(adc_test_init);
module_exit(adc_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mshrimp");
MODULE_DESCRIPTION("Test adc");
MODULE_VERSION("V0.1");

