#ifndef	__MMA7660_H__
#define	__MMA7660_H__

#include "i2c.h"

int mma7660_init(void);

int mma7660_write_data(char *data, unsigned long len);


#endif
