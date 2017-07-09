#ifndef	__LED_H__
#define	__LED_H__


#define	GPM4_BASE				0x11000000

#define	GPM4CON_OFFSET			0x02E0
#define	GPM4DAT_OFFSET			0x02E4


#define	GPM4CON_ADDR			(GPM4_BASE+GPM4CON_OFFSET)
#define	GPM4DAT_ADDR			(GPM4_BASE+GPM4DAT_OFFSET)



/************************************************************/

#define	LED_CON_ADDR			GPM4CON_ADDR
#define	LED_DAT_ADDR			GPM4DAT_ADDR


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

#define	LED_IOC_GET_STATUS		_IOR(LED_IOC_TYPE, 10, unsigned int)
#define	LED_IOC_SET_STATUS		_IOW(LED_IOC_TYPE, 11, unsigned int)

#define	LED_IOC_SET_BLINK_DATA		_IOW(LED_IOC_TYPE, 12, int)
#define	LED_IOC_GET_BLINK_DATA		_IOR(LED_IOC_TYPE, 13, int)

#define	LED_IOC_SET_RUN_LAMP_POS	_IOW(LED_IOC_TYPE, 14, int)
#define	LED_IOC_SET_RUN_LAMP_NEG	_IOW(LED_IOC_TYPE, 15, int)

#define	LED_IOC_MAX_NR			15


/*
bit		|	4bit	|	1bit	|	3bit	|	24bit
name	|  reserve	| directior	|  led_num	|   data

directior:
	0	Set data to driver
	1	Get data to driver

led_num:
	0~3
	led_num < LED_TOTLE_NUM

data:
	The data set to driver or get from driver
*/


#define	LED_CTL_DIR_BIT		1
#define	LED_CTL_NUM_BIT		3
#define	LED_CTL_DATA_BIT	24

#define	LED_IOC_DATA(dir, num, data)	\
		((dir)<<((LED_CTL_NUM_BIT)+(LED_CTL_DATA_BIT)) | ((num)<<(LED_CTL_DATA_BIT)) | (data))

#define	LED_CTL_DIR(data)	\
		(((data)>>((LED_CTL_NUM_BIT)+(LED_CTL_DATA_BIT)))&((0x1<<(LED_CTL_DIR_BIT))-1))

#define	LED_CTL_NUM(data)	(((data)>>(LED_CTL_DATA_BIT))&((0x1<<(LED_CTL_NUM_BIT))-1))

#define	LED_CTL_DATA(data)	((data)&((0x1<<(LED_CTL_DATA_BIT))-1))



#endif


