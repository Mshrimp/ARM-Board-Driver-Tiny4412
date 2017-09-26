#ifndef	__KEY_H__
#define	__KEY_H__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/gpio.h>

#define	GPX3_BASE				0x11000000

#define	GPX3CON_OFFSET			0x0C60
#define	GPX3DAT_OFFSET			0x0C64

#define	GPX3CON_ADDR			(GPX3_BASE + GPX3CON_OFFSET)
#define	GPX3DAT_ADDR			(GPX3_BASE + GPX3DAT_OFFSET)



// Int
#define	EXT_INT43CON_OFFSET		0x0E0C
#define	EXT_INT43_MASK_OFFSET	0x0F0C
#define	EXT_INT43_PEND_OFFSET	0x0F4C

#define	EXT_INT43CON_ADDR		(GPX3_BASE + EXT_INT43CON_OFFSET)
#define	EXT_INT43_MASK_ADDR		(GPX3_BASE + EXT_INT43_MASK_OFFSET)
#define	EXT_INT43_PEND_ADDR		(GPX3_BASE + EXT_INT43_PEND_OFFSET)



#define	KEY_INT_IRQ_NUM			64



#define	KEY_DEV_NAME			"test_dev_key"
#define	KEY_TOTLE_NUM			4

#define KEY_IOC_TYPE			'K'
#define	KEY_IOC_MAX_NR			4




#if 0
#define KEY_ALL_OFF				_IO(KEY_CTRL_TYPE, 0)
#define	KEY_ALL_ON				_IO(KEY_CTRL_TYPE, 1)

#define KEY1_OFF				_IO(KEY_CTRL_TYPE, 2)
#define KEY1_ON					_IO(KEY_CTRL_TYPE, 3)

#define KEY2_OFF				_IO(KEY_CTRL_TYPE, 4)
#define KEY2_ON					_IO(KEY_CTRL_TYPE, 5)

#define KEY3_OFF				_IO(KEY_CTRL_TYPE, 6)
#define KEY3_ON					_IO(KEY_CTRL_TYPE, 7)

#define KEY4_OFF				_IO(KEY_CTRL_TYPE, 8)
#define KEY4_ON					_IO(KEY_CTRL_TYPE, 9)
#endif



#endif


