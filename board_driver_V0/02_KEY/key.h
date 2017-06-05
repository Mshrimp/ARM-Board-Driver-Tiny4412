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




#define	KEY_TOTLE_NUM		4

void key_config(void);
void key_config_n(int n);
uchar8 key_get_bit(int n);
uint32 key_get_val(void);


#endif	/* __KEY_H__ */
