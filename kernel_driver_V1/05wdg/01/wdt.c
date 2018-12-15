#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/clk.h>
#include <asm/uaccess.h>
#include <asm/io.h>
// 每个cdev至少要有一个设备号
// 主设备号+次设备号=设备号
// 同一类设备的主设备号是相同的

// include/linux/xxx.h

dev_t devno = 0;

struct wdt_st {
	int wtcon, wtdat, wdtcnt;
	void *__iomem v;
	struct clk *clk;
	dev_t no;
	struct cdev dev;
	struct file_operations wdt_ops;
	spinlock_t lock;
};

struct wdt_feed_st {

};
//================================================================
//硬件相关
void exynos4_wdt_init(struct wdt_st *w)
{

}

void exynos4_wdt_exit(struct wdt_st *w)
{

}

void exynos4_wdt_enable(struct wdt_st *w)
{

}

void exynos4_wdt_disable(struct wdt_st *w)
{

}

void exynos4_wdt_feed(struct wdt_st *w)
{

}

//================================================================
//硬件无关
// 每个文件对应一个inode	唯一
// 每次打开都对应一个file	不唯一
// /dev/test/wdt
int my_open(struct inode *inode, struct file *fp)
{
	struct wdt_st *m;
	m = container_of(inode->i_cdev, struct wdt_st, dev);
	memset(m->buf, 0, 256);
	fp->private_data = m;

	spin_lock(&m->lock);
	if (m->count) {
		spin_unlock(&m->lock);
		return -EBUSY;
	}

	m->count = 1;
	spin_unlock(&m->lock);

	return 0;
}

int my_close(struct inode *inode, struct file *fp)
{
	struct wdt_st *m;
	m = fp->private_data;

	spin_lock(&m->lock);
	if (m->count == 0) {
		spin_unlock(&m->lock);
		return -ENODEV;
	}
	m->count = 0;
	spin_unlock(&m->lock);

	return 0;
}

ssize_t my_read(struct file *fp, char __user *buffer, size_t count, loff_t *off)
{
	struct wdt_st *m;
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
	struct wdt_st *w;
	struct wdt_feed_st *wfs;

	int ret = 0;

	if (count != sizeof(wfs)) {
		return -EINVAL;
	}

	w = fp->private_data;
	count = min((int)count, 256);

	ret = copy_from_user(&wdt, buffer, count);
	if (ret) {
		ret = -EFAULT;
		goto copy_error;
	}
	exynos4_feed_wdt(w, wfs->time);

	return count;
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

struct wdt_st *wdt;

/****************************************************************/
static __init int my_cdev_init(void)
{
	int ret = 0;
	printk("cdev_init\n");

	wdt = kzalloc(sizeof(*wdt), GFP_KERNEL);
	if (!wdt) {
		ret = -ENOMEM;
		goto alloc_wdt_error;
	}
	
	ret = alloc_chrdev_region(&wdt->no, 1, 1, "my_wdt");
	if (ret < 0) {
		goto register_error;
	}

	cdev_init(&wdt->dev, &my_ops);
	ret = cdev_add(&wdt->dev, wdt->no, 1);
	if (ret < 0) {
		goto cdev_add_error;
	}
	printk("major: %d, minor: %d\n", MAJOR(wdt->no), MINOR(wdt->no));

	wdt->v = ioremap(EXYNOS4_PA_WDT, SZ_4K);
	if (!wdt->v) {
		ret = -ENOMEM;
		goto ioremmap_error;
	}
	spin_lock_init(&wdt->lock);
	wdt->clk = clk_get(NULL, "watchdog");
	clk_enable(wdt->clk);

	exynos4_wdt_init(wdt);

	return 0;
ioremmap_error:
	cdev_del(&wdt->dev);
cdev_add_error:
	unregister_chrdev_region(wdt->no, 1);
register_error:
	kfree(wdt);
alloc_wdt_error:
	return ret;
}

static __exit void my_cdev_exit(void)
{
	printk("cdev_exit\n");
	exynox4_wdt_exit(wdt);
	clk_disable(wdt->clk);
	clk_put(wdt->clk);

	iounmap(&wdt->v);

	cdev_del(&wdt->dev);
	unregister_chrdev_region(devno, 1);

}

module_init(my_cdev_init);
module_exit(my_cdev_exit);

MODULE_DESCRIPTION("Driver for eeprom, at2402 device");
MODULE_AUTHOR("Mshrimp");
MODULE_LICENSE("GPL");

