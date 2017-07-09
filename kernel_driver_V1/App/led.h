#ifndef	__LED_H__
#define	__LED_H__



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

#define	LED_IOC_GET_DATA		_IOR(LED_IOC_TYPE, 10, int)
#define	LED_IOC_SET_DATA		_IOW(LED_IOC_TYPE, 11, int)

#define	LED_IOC_MAX_NR			11


#endif


