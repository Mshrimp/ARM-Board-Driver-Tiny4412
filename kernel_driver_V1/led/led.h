#ifndef	__LED_H__
#define	__LED_H__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/gpio.h>

#define	GPM4_BASE				0x11000000

#define	GPM4CON_OFFSET			0x02E0
#define	GPM4DAT_OFFSET			0x02E4


#define	GPM4CON_ADDR			(GPM4_BASE+GPM4CON_OFFSET)
#define	GPM4DAT_ADDR			(GPM4_BASE+GPM4DAT_OFFSET)



/************************************************************/

#define	LED_CON_ADDR			GPM4CON_ADDR
#define	LED_DAT_ADDR			GPM4DAT_ADDR


#define	DEV_NAME				"test_dev_led"

#define LED_CTRL_TYPE			"L"

#define	LED_TOTLE_NUM			4

#define LED_ALL_OFF				_IO(LED_CTRL_TYPE, 0)
#define	LED_ALL_ON				_IO(LED_CTRL_TYPE, 1)

#define LED1_OFF				_IO(LED_CTRL_TYPE, 2)
#define LED1_ON					_IO(LED_CTRL_TYPE, 3)

#define LED2_OFF				_IO(LED_CTRL_TYPE, 4)
#define LED2_ON					_IO(LED_CTRL_TYPE, 5)

#define LED3_OFF				_IO(LED_CTRL_TYPE, 6)
#define LED3_ON					_IO(LED_CTRL_TYPE, 7)

#define LED4_OFF				_IO(LED_CTRL_TYPE, 8)
#define LED4_ON					_IO(LED_CTRL_TYPE, 9)




#endif


