#ifndef	__LED_H__
#define	__LED_H__


/************************************************************/
#define	GPM4_BASE				0x11000000

#define	GPM4CON_OFFSET			0x02E0
#define	GPM4DAT_OFFSET			0x02E4


#define	GPM4CON_ADDR			(GPM4_BASE+GPM4CON_OFFSET)
#define	GPM4DAT_ADDR			(GPM4_BASE+GPM4DAT_OFFSET)



/************************************************************/

#define	LED_CON_ADDR			GPM4CON_ADDR
#define	LED_DAT_ADDR			GPM4DAT_ADDR

#define	LED_CON_ADDR_P			((volatile unsigned int *)ioremap((GPM4CON_ADDR), 32))
#define	LED_DAT_ADDR_P			((volatile unsigned int *)ioremap((GPM4DAT_ADDR), 32))


#define	DEV_NAME				"test_dev_led"

#define LED_IOC_TYPE			'L'

#define	LED_TOTLE_NUM			4

#define LED_ALL_OFF				_IO(LED_IOC_TYPE, 0)
#define	LED_ALL_ON				_IO(LED_IOC_TYPE, 1)

#define LED1_OFF				_IO(LED_IOC_TYPE, 2)
#define LED1_ON					_IO(LED_IOC_TYPE, 3)

#define LED2_OFF				_IO(LED_IOC_TYPE, 4)
#define LED2_ON					_IO(LED_IOC_TYPE, 5)

#define LED3_OFF				_IO(LED_IOC_TYPE, 6)
#define LED3_ON					_IO(LED_IOC_TYPE, 7)

#define LED4_OFF				_IO(LED_IOC_TYPE, 8)
#define LED4_ON					_IO(LED_IOC_TYPE, 9)

#define	LED_IOC_SET_STATUS		_IOW(LED_IOC_TYPE, 10, unsigned int)
#define	LED_IOC_GET_STATUS		_IOR(LED_IOC_TYPE, 11, unsigned int)

#define	LED_IOC_SET_BLINK_DATA		_IOW(LED_IOC_TYPE, 12, int)
#define	LED_IOC_GET_BLINK_DATA		_IOR(LED_IOC_TYPE, 13, int)

#define	LED_IOC_SET_RUN_LAMP_DATA	_IOW(LED_IOC_TYPE, 14, int)
#define	LED_IOC_GET_RUN_LAMP_DATA	_IOW(LED_IOC_TYPE, 15, int)

#define	LED_IOC_MAX_NR			15



/************************************************************/	//LED control
/* Set LED control time
bit		|	1bit	|	3bit	|	4bit	|	12bit	|	12bit
name	| directior	|  led_num	|	Count	|   Ptime	|	Ntime

directior:
	0	Set data to driver/Negative
	1	Get data to driver/Positive

led_num:
	0~3
	led_num < LED_TOTLE_NUM

Count:
	The count led control

Ptime:
	The time set to open led, < 2^12(4096)

Ntime:
	The time set to close led, < 2^12(4096)

*/


#define	LED_IOC_CTRL_DIR_BIT		1
#define	LED_IOC_CTRL_NUM_BIT		3
#define	LED_IOC_CTRL_CNT_BIT		4
#define	LED_IOC_CTRL_PTIME_BIT		12
#define	LED_IOC_CTRL_NTIME_BIT		12

#define	LED_IOC_CTRL_TIME_BIT		((LED_IOC_CTRL_NTIME_BIT) + (LED_IOC_CTRL_PTIME_BIT))


#define	LED_IOC_CTRL_NTIME_SHIFT	(0)
#define	LED_IOC_CTRL_PTIME_SHIFT	((LED_IOC_CTRL_NTIME_SHIFT) + (LED_IOC_CTRL_NTIME_BIT))
#define LED_IOC_CTRL_CNT_SHIFT		((LED_IOC_CTRL_PTIME_SHIFT) + (LED_IOC_CTRL_PTIME_BIT))
#define	LED_IOC_CTRL_NUM_SHIFT		((LED_IOC_CTRL_CNT_SHIFT) + (LED_IOC_CTRL_CNT_BIT))
#define	LED_IOC_CTRL_DIR_SHIFT		((LED_IOC_CTRL_NUM_SHIFT) + (LED_IOC_CTRL_NUM_BIT))

// Data Package
#define	LED_IOC_CTRL_TIME(Ptime, Ntime)	\
		(((Ptime)<<(LED_IOC_CTRL_PTIME_SHIFT)) | ((Ntime)<<(LED_IOC_CTRL_NTIME_SHIFT)))
#define	LED_IOC_CTRL_DATA(dir, num, cnt, time)	\
		(((dir)<<(LED_IOC_CTRL_DIR_SHIFT)) | ((num)<<(LED_IOC_CTRL_NUM_SHIFT)) | ((cnt)<<(LED_IOC_CTRL_CNT_SHIFT)) | ((Ptime)<<(LED_IOC_CTRL_PTIME_SHIFT)) | ((Ntime)<<(LED_IOC_CTRL_NTIME_SHIFT)))

// Data Unpackage
#define	LED_IOC_CTRL_DIR(data)	\
		(((data)>>(LED_IOC_CTRL_DIR_SHIFT))&((0x1<<(LED_IOC_CTRL_DIR_BIT))-1))

#define	LED_IOC_CTRL_NUM(data)	(((data)>>(LED_IOC_CTRL_NUM_SHIFT))&((0x1<<(LED_IOC_CTRL_NUM_BIT))-1))

#define	LED_IOC_CTRL_CNT(data)	(((data)>>(LED_IOC_CTRL_CNT_SHIFT))&((0x1<<(LED_IOC_CTRL_CNT_BIT))-1))


#define	LED_IOC_CTRL_DATA(data)	((data)&((0x1<<(LED_IOC_CTRL_TIME_BIT))-1))

#define	LED_IOC_CTRL_PTIME(data)	(((data)>>(LED_IOC_CTRL_PTIME_SHIFT))&((0x1<<(LED_IOC_CTRL_PTIME_BIT))-1))
#define	LED_IOC_CTRL_NTIME(data)	(((data)>>(LED_IOC_CTRL_NTIME_SHIFT))&((0x1<<(LED_IOC_CTRL_NTIME_BIT))-1))

#endif


