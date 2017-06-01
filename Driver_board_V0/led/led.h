#ifndef	__LED_H__
#define	__LED_H__
#include "regs.h"


void led_init(void);
void led_on_all(void);
void led_off_all(void);
void led_on(int bit);
void led_off(int bit);

#endif /* __LED_H__ */

