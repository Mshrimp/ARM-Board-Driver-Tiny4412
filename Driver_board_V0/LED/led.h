#ifndef	__LED_H__
#define	__LED_H__

#include "board.h"

#define	GPM4CON		0x1100
#define	GPM4DAT		0x1101


int led_config_n(int n);
void led_config(void);
void led_on_all(void);
void led_off_all(void);
void led_on(int n);
void led_off(int n);
void led_set_val(uint32 val);
uint32 led_get_val(void);


#endif	/* __BOARD_H__ */
