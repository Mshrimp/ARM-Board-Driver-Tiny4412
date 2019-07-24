#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/pwm.h>

#include <linux/gpio.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>

#define BEEP_PWM_GPIO	EXYNOS4_GPD0(1)
#define BEEP_PWM_ID		(1)

#define DEVICE_NAME		"beep_pwm"

#define NS_IN_1HZ		(1000000000UL)

#define PWM_SET_FREQ	0x12
#define	PWM_START		0x13
#define	PWM_STOP		0x14

#define BEEP_PWM_SET_FREQ	_IOW('B', PWM_SET_FREQ, int)
#define BEEP_PWM_STOP		_IOW('B', PWM_STOP, int)

struct tiny4412_beep {
	unsigned int dev_major;
	struct class *cls;
	struct device *dev;
	int value;
};

struct tiny4412_beep *beep_dev;
static struct semaphore sem_lock;
static struct pwm_device *pwm_beep = NULL;

void pwm_set_freq(unsigned long freq)
{
	int period_ns = NS_IN_1HZ / freq;

	pwm_config(pwm_beep, period_ns / 2, period_ns);
	pwm_enable(pwm_beep);

	s3c_gpio_cfgpin(BEEP_PWM_GPIO, S3C_GPIO_SFN(2));

	printk("%s, freq: 0x%d\n", __func__, freq);
}

void pwm_stop(void)
{
	s3c_gpio_cfgpin(BEEP_PWM_GPIO, S3C_GPIO_OUTPUT);

	pwm_config(pwm_beep, 0, NS_IN_1HZ/100);
	pwm_disable(pwm_beep);
}

static int exynos_pwm_open(struct inode *inode, struct file *filp)
{
	int ret = 0;
	printk("%s, exynos_pwm_open!\n", __func__);

	ret = down_trylock(&sem_lock);
	if (ret < 0) {
		printk("%s, down_trylock failed!\n", __func__);
		return -ENODEV;
	}

	ret = gpio_request(BEEP_PWM_GPIO, DEVICE_NAME);
	if (ret < 0) {
		printk("%s, gpio_request failed!\n", __func__);
		return -ENODEV;
	}

	gpio_direction_output(BEEP_PWM_GPIO, 0);

	return 0;
}

static long exynos_pwm_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int nr = _IOC_NR(cmd);

	printk("%s, exynos_pwm_ioctl, cmd: 0x%X\n", __func__, cmd);

	switch(nr)
	{
		case PWM_SET_FREQ:
			if (0 == arg) {
				printk("%s, arg = 0\n", __func__);
				return -EINVAL;
			} else {
				pwm_set_freq(arg);
			}
			break;
		
		case PWM_START:
			
			break;
		case PWM_STOP:
		default:
			pwm_stop();
			break;
	}

	return 0;
}

static int exynos_pwm_release(struct inode *inode, struct file *filp)
{
	printk("%s, exynos_pwm_release!\n", __func__);

	up(&sem_lock);
	gpio_free(BEEP_PWM_GPIO);

	return 0;
}

struct file_operations beep_fops = {
	.owner = THIS_MODULE,
	.open = exynos_pwm_open,
	.release = exynos_pwm_release,
	//.read = beep_read,
	//.write = beep_write,
	.unlocked_ioctl = exynos_pwm_ioctl,
};

static __init int exynos_pwm_init(void)
{
	int ret = 0;

	printk("%s, driver init!\n", __func__);
	
	ret = gpio_request(BEEP_PWM_GPIO, DEVICE_NAME);
	if (ret < 0) {
		printk("%s, gpio_request failed!\n", __func__);
		return -ENODEV; 
	}

	gpio_direction_output(BEEP_PWM_GPIO, 0);
	gpio_free(BEEP_PWM_GPIO);

	pwm_beep = pwm_request(BEEP_PWM_ID, DEVICE_NAME);
	if (IS_ERR(pwm_beep)) {
		printk("%s, pwm_request failed!\n", __func__);
		return -ENODEV;
	}

	pwm_stop();

	sema_init(&sem_lock, 1);

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

	return 0;

device_create_err:
	class_destroy(beep_dev->cls);
class_create_err:
	unregister_chrdev(beep_dev->dev_major, "beep_drv");
register_chrdev_err:
	kfree(beep_dev);
	return ret;
}

static __exit void exynos_pwm_exit(void)
{
	printk("%s, driver exit!\n", __func__);

	pwm_stop();
	pwm_free(pwm_beep);
	gpio_free(BEEP_PWM_GPIO);

	device_destroy(beep_dev->cls, MKDEV(beep_dev->dev_major, 0));
	class_destroy(beep_dev->cls);
	unregister_chrdev(beep_dev->dev_major, "beep_drv");

	kfree(beep_dev);
}

module_init(exynos_pwm_init);
module_exit(exynos_pwm_exit);

MODULE_LICENSE("GPL");

