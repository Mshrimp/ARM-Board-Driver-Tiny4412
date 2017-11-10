
#ifndef	__KEY_H__
#define	__KEY_H__


#define	GPIO_BASE_ADDR		0x11000000


#define	GPX3CON_ADDR		(GPIO_BASE_ADDR + 0x0C60)
#define	GPX3DAT_ADDR		(GPIO_BASE_ADDR + 0x0C64)


#define	KEY_CON_ADDR		GPX3CON_ADDR
#define	KEY_DAT_ADDR		GPX3DAT_ADDR


#define	GPIO_INPUT			0x0
#define	GPIO_OUTPUT			0x1


#define	GPIO_OUTPUT_HIGH	0x1
#define	GPIO_OUTPUT_LOW		0x0
#define	GPIO_INT			0xF


#define	KEY_TOTLE			4


#endif
