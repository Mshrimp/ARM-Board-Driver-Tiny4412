#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <mach/irqs.h>

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

#define EXYNOS4_PA_WDT	0x10060000
#define	WTCON	0x00
#define	WTDAT	0x04
#define	WTCNT	0x08
#define	WTCLEARINT	0x0C

//===========================================硬件相关
void test_wdt_init(struct wdt_st *w)
{
	printk("%s\n", __func__);

	// 100M / (99 + 1) / 64 = 15625Hz
	w->wtcon = (99 << 8) | (2 << 3) | (1 << 2);	// 产生中断
	w->wtcnt = 15625 * 3;	// 15625Hz, 3s, < 65535

	// writel
	iowrite32(w->wtcon, w->v +WTCON);
	iowrite32(w->wtcnt, w->v +WTCNT);
	iowrite32(w->wtcnt, w->v +WTDAT);
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

	printk("%s, %d\n", __func__, __LINE__);
	w = filp->private_data;
	printk("%s, %d\n", __func__, __LINE__);

	ret = copy_from_user(&wfs, buf, sizeof(wfs));
	if (ret) {
		printk("%s, copy_from_user failed, ret = %d\n", __func__, ret);
		return -EFAULT;
		goto copy_error;
	}
	printk("%s, %d\n", __func__, __LINE__);
	
	test_wdt_feed(w, wfs.time);
	printk("%s, %d\n", __func__, __LINE__);

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

irqreturn_t wdt_isr(int irq, void *data)
{
	struct wdt_st *w;

	w = data;

	printk("%s, no: %d\n", __func__, w->no);
	// 清中断操作, 内部中断必须有清中断操作
	writel(1, w->v + WTCLEARINT);

	return IRQ_HANDLED;
}

struct wdt_st *wdt;

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
	ret = alloc_chrdev_region(&wdt->no, 1, 1, "test_wdt");
	if (ret) {
		printk("%s, alloc_chrdev_region failed, ret = %d\n", __func__, ret);
		goto alloc_no_error;
	}
	printk("%s, wdt->no: %d\n", __func__, wdt->no);


    /*
	 *major = register_chrdev(major, DEV_NAME, &fops);
	 *if (major < 0) {
	 *    DRV_ERROR("register_chrdev failed");
	 *    goto ERR_dev_register;
	 *}
     */

    /*
	 *driver_class = class_create(THIS_MODULE, "driver_class");
	 *if (!driver_class) {
	 *    DRV_ERROR("class_create failed");
	 *    goto ERR_class_create;
	 *}
     */

/*
 *    driver_class_device = device_create(driver_class, NULL, MKDEV(major, 0), NULL, "button_device");
 *    if (!driver_class_device) {
 *        DRV_ERROR("device_create failed");
 *        goto ERR_class_device_create;
 *    }
 *
 *    printk("major = %d\n", major);
 */

	wdt->wdt_ops.open = wdt_open;
	wdt->wdt_ops.release = wdt_close;
	wdt->wdt_ops.write = wdt_write;
	wdt->wdt_ops.unlocked_ioctl = wdt_ioctl;

	cdev_init(&wdt->dev, &wdt->wdt_ops);

	ret = cdev_add(&wdt->dev, wdt->no, 1);
	if (ret) {
		goto cdev_add_error;
	}

	wdt->v = ioremap(EXYNOS4_PA_WDT, SZ_4K);
	if (!wdt->v) {
		printk("%s, ioremap failed\n", __func__);
		ret = -ENOMEM;
		goto ioremap_error;
	}

	spin_lock_init(&wdt->lock);

	wdt->clk = clk_get(NULL, "watchdog");
	clk_enable(wdt->clk);

	ret = request_irq(EXYNOS4_IRQ_WDT, wdt_isr, 0, "test_wdt", wdt);
	if (ret < 0) {
		goto request_irq_error;
	}

	test_wdt_init(wdt);

	return 0;
request_irq_error:
	clk_disable(wdt->clk);
	clk_put(wdt->clk);
	iounmap(wdt->v);
ioremap_error:
	cdev_del(&wdt->dev);
cdev_add_error:
	unregister_chrdev_region(wdt->no, 1);
alloc_no_error:
	kfree(wdt);
alloc_wdt_error:
	return ret;
}

static __exit void test_exit(void)
{
	test_wdt_exit(NULL);
	free_irq(EXYNOS4_IRQ_WDT, wdt);
	clk_disable(wdt->clk);
	clk_put(wdt->clk);
	iounmap(wdt->v);
	cdev_del(&wdt->dev);
	unregister_chrdev_region(wdt->no, 1);

    /*
	 *unregister_chrdev(major, DEV_NAME);
	 *device_destroy(driver_class, MKDEV(major, 0));
	 *class_destroy(driver_class);
     */
	kfree(wdt);

}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");

