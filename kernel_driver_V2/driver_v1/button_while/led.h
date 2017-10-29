
#ifndef __LED_H__
#define __LED_H__


#define LED_BASE_ADDR		0x11000000

#define LED_CON_ADDR		(LED_BASE_ADDR + 0x02E0)
#define	LED_DAT_ADDR		(LED_BASE_ADDR + 0x02E4)


#define	GPIO_INPUT			0x0
#define	GPIO_OUTPUT			0x1


#define	GPIO_OUTPUT_HIGH	0x1
#define	GPIO_OUTPUT_LOW		0x0


#define	LED_TOTLE			4


#endif


