#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include <linux/spinlock.h>

struct wdt_st {
	int wtcon, wtdat, wtcnt;
	void *__iomem v;
	struct clk *clk;

	dev_t no;
	struct cdev dev;
	struct file_operations wdt_ops;
	
	spinlock_t lock;

	int count;	// lock
};

struct wdt_feed_st {
	int time;
};

#define DEV_NAME	"test_wdt"

#define EXYNOS4_PA_WDT	0x10060000
#define	WTCON	0x00
#define	WTCNT	0x08

//===========================================硬件相关
void test_wdt_init(struct wdt_st *w)
{
	printk("%s\n", __func__);

	// 100M / (99 + 1) / 64 = 15625Hz
	w->wtcon = (99 << 8) | (2 << 3) | 1;
	w->wtcnt = 15625 * 3;	// 15625Hz, 3s, < 65535

	// writel
	iowrite32(w->wtcon, w->v +WTCON);
	iowrite32(w->wtcnt, w->v +WTCNT);
}

void test_wdt_exit(struct wdt_st *w)
{
	printk("%s\n", __func__);

}

void test_wdt_enable(struct wdt_st *w)
{
	printk("%s\n", __func__);

	w->wtcon = ioread32(w->v + WTCON);
	w->wtcon |= 1 << 5;
	iowrite32(w->wtcon, w->v +WTCON);
}

void test_wdt_disable(struct wdt_st *w)
{
	printk("%s\n", __func__);

	w->wtcon = ioread32(w->v + WTCON);
	w->wtcon &= ~(1 << 5);
	iowrite32(w->wtcon, w->v +WTCON);
}

void test_wdt_feed(struct wdt_st *w, int ms)
{
	printk("%s\n", __func__);

	iowrite32(ms, w->v +WTCNT);
}


//===========================================硬件无关

struct wdt_st *wdt;

int wdt_open (struct inode *inodep, struct file *filp)
{
	struct wdt_st *w;

	printk("Driver: wdt open!\n");

	w = container_of(inodep->i_cdev, struct wdt_st, dev);

	filp->private_data = w;

	spin_lock(&w->lock);
	if (w->count) {
		printk("%s, busy\n", __func__);
		spin_unlock(&w->lock);
		return -EBUSY;
	}

	w->count = 1;

	spin_unlock(&w->lock);

	return 0;
}

int wdt_close (struct inode *inodep, struct file *filp)
{
	struct wdt_st *w;

	printk("Driver: wdt close!\n");

	w = filp->private_data;
	
	spin_lock(&w->lock);
	if (0 == w->count) {
		printk("%s, no wdt working\n", __func__);
		spin_unlock(&w->lock);
		return -ENODEV;
	}

	w->count = 0;

	spin_unlock(&w->lock);

	return 0;
}

ssize_t wdt_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	struct wdt_st *w;
	struct wdt_feed_st wfs;

	int ret;

	printk("Driver: wdt write!\n");
	printk("Driver: size = %d\n", size);

	if (size != sizeof(wfs)) {
		printk("%s, data size error, size = %d\n", __func__, size);
		return -EINVAL;
	}

	w = filp->private_data;

	ret = copy_from_user(&wfs, buf, sizeof(wfs));
	if (ret) {
		printk("%s, copy_from_user failed, ret = %d\n", __func__, ret);
		return -EFAULT;
		goto copy_error;
	}
	
	test_wdt_feed(w, wfs.time);

	return size;

copy_error:
	return ret;
}

ssize_t wdt_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	printk("Driver: wdt read!\n");

	return size;
}

// cmd: 0 disable
// cmd: 1 enable
long wdt_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct wdt_st *w;

	printk("Driver: wdt ioctl!\n");

	w = filp->private_data;

	switch (cmd) {
		case 0:
			test_wdt_disable(w);
			break;
		case 1:
			test_wdt_enable(w);
			break;
		default:
			break;
	}

	return 0;
}

//==========================================================模块
struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = wdt_open,
	.release = wdt_close,
	.read = wdt_read,
	.write = wdt_write,
	.unlocked_ioctl = wdt_ioctl,
};

int major = 0;
struct class *driver_class;
struct device *driver_class_device;

static __init int test_init(void)
{
	int ret = 0;

	wdt = kzalloc(sizeof(*wdt), GFP_KERNEL);
	if (!wdt) {
		printk("%s, kzalloc failed, ret = %d\n", __func__, ret);
		ret = -ENOMEM;
		goto alloc_wdt_error;
	}

	// test_wdt -> /proc/devices
    /*
	 *ret = alloc_chrdev_region(&wdt->no, 1, 1, "test_wdt");
	 *if (ret) {
	 *    printk("%s, alloc_chrdev_region failed, ret = %d\n", __func__, ret);
	 *    goto alloc_no_error;
	 *}
	 *printk("%s, wdt->no: %d\n", __func__, wdt->no);
     */


	major = register_chrdev(major, DEV_NAME, &fops);
	if (major < 0) {
		printk("register_chrdev failed");
		goto ERR_dev_register;
	}

	driver_class = class_create(THIS_MODULE, "driver_class");
	if (!driver_class) {
		printk("class_create failed");
		goto ERR_class_create;
	}

	driver_class_device = device_create(driver_class, NULL, MKDEV(major, 0), NULL, "wdt_device");
	if (!driver_class_device) {
		printk("device_create failed");
		goto ERR_class_device_create;
	}

	printk("major = %d\n", major);

    /*
	 *wdt->wdt_ops.open = wdt_open;
	 *wdt->wdt_ops.release = wdt_close;
	 *wdt->wdt_ops.write = wdt_write;
	 *wdt->wdt_ops.unlocked_ioctl = wdt_ioctl;
     */

/*
 *    cdev_init(&wdt->dev, &wdt->wdt_ops);
 *
 *    ret = cdev_add(&wdt->dev, wdt->no, 1);
 *    if (ret) {
 *        goto cdev_add_error;
 *    }
 */

	wdt->v = ioremap(EXYNOS4_PA_WDT, SZ_4K);
	if (!wdt->v) {
		printk("%s, ioremap failed\n", __func__);
		ret = -ENOMEM;
		goto ioremap_error;
	}

	spin_lock_init(&wdt->lock);

	wdt->clk = clk_get(NULL, "watchdog");
	clk_enable(wdt->clk);

	test_wdt_init(wdt);

	return 0;
ioremap_error:
	cdev_del(&wdt->dev);
cdev_add_error:
	unregister_chrdev_region(wdt->no, 1);
alloc_no_error:
	kfree(wdt);
alloc_wdt_error:
	return ret;

ERR_class_device_create:
	device_destroy(driver_class, MKDEV(major, 0));
ERR_class_create:
	unregister_chrdev(major, DEV_NAME);
ERR_dev_register:
	return -1;
}

static __exit void test_exit(void)
{
	test_wdt_exit(NULL);
	clk_disable(wdt->clk);
	clk_put(wdt->clk);
	iounmap(wdt->v);
    /*
	 *cdev_del(&wdt->dev);
	 *unregister_chrdev_region(wdt->no, 1);
     */

	unregister_chrdev(major, DEV_NAME);
	device_destroy(driver_class, MKDEV(major, 0));
	class_destroy(driver_class);
	kfree(wdt);

}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");

