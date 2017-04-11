#ifndef	__LED_H__
#define	__LED_H__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/gpio.h>

#define	GPM4_BASE			0x11000000

#define	GPM4CON_OFFSET		0x02E0
#define	GPM4DAT_OFFSET		0x02E4


#define	GPM4CON_ADDR		(GPM4_BASE+GPM4CON_OFFSET)
#define	GPM4DAT_ADDR		(GPM4_BASE+GPM4DAT_OFFSET)


#define	DEV_NAME			"test_dev_led"

#define LED_CTRL_TYPE		0x01

#define LED_OFF				_IO(LED_CTRL_TYPE, 0)
#define	LED_ON				_IO(LED_CTRL_TYPE, 1)





#endif


