#ifndef __LED_H__
#define __LED_H__

#define	GPIO_BASE_ADDR		0x11000000

#define	GPM4CON_ADDR		(GPIO_BASE_ADDR + 0x02E0)
#define	GPM4DAT_ADDR		(GPIO_BASE_ADDR + 0x02E4)

#define	LED_CON_ADDR		GPM4CON_ADDR
#define	LED_DAT_ADDR		GPM4DAT_ADDR

#define	GPIO_INPUT			0x0
#define	GPIO_OUTPUT			0x1

#define	GPIO_OUTPUT_HIGH	0x1
#define	GPIO_OUTPUT_LOW		0x0

#define	LED_TOTLE			4


#define drv_info(fmt, args...)      \
			printk("Info Driver: " fmt "(function: %s, line: %d)\n", ##args, __func__, __LINE__)

#define drv_debug(fmt, args...)     \
			printk("Debug Driver: " fmt "(function: %s, line: %d)\n", ##args, __func__, __LINE__)

#define drv_error(fmt, args...)     \
			printk("Error Driver: " fmt "(function: %s, line: %d)\n", ##args, __func__, __LINE__)

#endif

