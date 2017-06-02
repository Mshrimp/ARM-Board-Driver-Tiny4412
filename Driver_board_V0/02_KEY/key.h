#ifndef	__KEY_H__
#define	__KEY_H__

#include "board.h"

#define	GPIO_BASE			0x11000000

#define	GPM4CON_OFFSET		0x02E0
#define	GPM4DAT_OFFSET		0x02E4

#define	GPM4CON				(*(vuint32 *)(GPIO_BASE + GPM4CON_OFFSET))
#define	GPM4DAT				(*(vuint32 *)(GPIO_BASE + GPM4DAT_OFFSET))

#define	GPX3CON_OFFSET		0x0C60
#define	GPX3DAT_OFFSET		0x0C64

#define	GPX3CON				(*(vuint32 *)(GPIO_BASE + GPX3CON_OFFSET))
#define	GPX3DAT				(*(vuint32 *)(GPIO_BASE + GPX3DAT_OFFSET))





int led_config_n(int n);
void led_config(void);
void led_on_all(void);
void led_off_all(void);
void led_on(int n);
void led_off(int n);
void led_set_val(uint32 val);
uint32 led_get_val(void);


#endif	/* __KEY_H__ */
