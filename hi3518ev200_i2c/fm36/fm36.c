#include <linux/uaccess.h>

#include "../common.h"
#include "../i2c/i2c.h"
#include "fm36.h"

#define	FM36_CHIP_ADDR			0xC0

#define	fm36_debug(fmt, args...)		\
			printk("FM36 debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);
#define	fm36_error(fmt, args...)		\
			printk("FM36 error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);

typedef struct {
	gpio_t reset_pin;
	unsigned char reset_pin_value;
	fm36_data_t init_data;
	spinlock_t lock;
} fm36_info_t;

static unsigned char fm36_init_data[3136] = {
	0xC0,
	0xFC, 0xF3, 0x6A, 0x10, 0x00, 0x00,
	0xFC, 0xF3, 0x68, 0x64, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x00, 0x83, 0x01, 0xF8,
	0xFC, 0xF3, 0x0D, 0x3F, 0x01, 0x92, 0x12, 0x98,
	0xFC, 0xF3, 0x0D, 0x3F, 0x02, 0x83, 0x0F, 0xFA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x03, 0x93, 0x01, 0xFA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x04, 0x81, 0xCC, 0x8C,
	0xFC, 0xF3, 0x0D, 0x3F, 0x05, 0x40, 0x09, 0x0A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x06, 0x26, 0x7C, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x07, 0x23, 0xA2, 0xD0,
	0xFC, 0xF3, 0x0D, 0x3F, 0x08, 0x92, 0x12, 0xAA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x09, 0x93, 0xFF, 0xEA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x0A, 0x83, 0xFF, 0xEA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x0B, 0x27, 0x8A, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x0C, 0x1B, 0xF0, 0xA1,
	0xFC, 0xF3, 0x0D, 0x3F, 0x0D, 0x82, 0x12, 0x98,
	0xFC, 0xF3, 0x0D, 0x3F, 0x0E, 0x93, 0x01, 0xF8,
	0xFC, 0xF3, 0x0D, 0x3F, 0x0F, 0x81, 0xCC, 0x8C,
	0xFC, 0xF3, 0x0D, 0x3F, 0x10, 0x19, 0x18, 0x4F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x11, 0x83, 0x01, 0xF8,
	0xFC, 0xF3, 0x0D, 0x3F, 0x12, 0x92, 0x12, 0x98,
	0xFC, 0xF3, 0x0D, 0x3F, 0x13, 0x83, 0x0F, 0xFA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x14, 0x93, 0x01, 0xFA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x15, 0x82, 0x12, 0xAA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x16, 0x93, 0xFF, 0xEA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x17, 0x83, 0xFF, 0xEA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x18, 0x27, 0x8A, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x19, 0x1B, 0xF1, 0x71,
	0xFC, 0xF3, 0x0D, 0x3F, 0x1A, 0x82, 0x12, 0x98,
	0xFC, 0xF3, 0x0D, 0x3F, 0x1B, 0x93, 0x01, 0xF8,
	0xFC, 0xF3, 0x0D, 0x3F, 0x1C, 0x81, 0x17, 0xE5,
	0xFC, 0xF3, 0x0D, 0x3F, 0x1D, 0x19, 0x1B, 0x4F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x1E, 0x83, 0x01, 0xF8,
	0xFC, 0xF3, 0x0D, 0x3F, 0x1F, 0x92, 0x12, 0x98,
	0xFC, 0xF3, 0x0D, 0x3F, 0x20, 0x83, 0x0F, 0xFA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x21, 0x93, 0x01, 0xFA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x22, 0x82, 0x12, 0xAA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x23, 0x93, 0xFF, 0xEA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x24, 0x83, 0xFF, 0xEA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x25, 0x27, 0x8A, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x26, 0x1B, 0xF2, 0x41,
	0xFC, 0xF3, 0x0D, 0x3F, 0x27, 0x82, 0x12, 0x98,
	0xFC, 0xF3, 0x0D, 0x3F, 0x28, 0x93, 0x01, 0xF8,
	0xFC, 0xF3, 0x0D, 0x3F, 0x29, 0x81, 0x17, 0xE5,
	0xFC, 0xF3, 0x0D, 0x3F, 0x2A, 0x19, 0x21, 0x7F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x2B, 0x83, 0x01, 0xF8,
	0xFC, 0xF3, 0x0D, 0x3F, 0x2C, 0x92, 0x12, 0x98,
	0xFC, 0xF3, 0x0D, 0x3F, 0x2D, 0x83, 0x07, 0xFA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x2E, 0x93, 0x01, 0xFA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x2F, 0x81, 0xCC, 0x8C,
	0xFC, 0xF3, 0x0D, 0x3F, 0x30, 0x40, 0x09, 0x0A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x31, 0x26, 0x7C, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x32, 0x23, 0xA2, 0xD0,
	0xFC, 0xF3, 0x0D, 0x3F, 0x33, 0x92, 0x12, 0xAA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x34, 0x93, 0xFF, 0xEA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x35, 0x83, 0xFF, 0xEA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x36, 0x27, 0x8A, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x37, 0x1B, 0xF3, 0x51,
	0xFC, 0xF3, 0x0D, 0x3F, 0x38, 0x82, 0x12, 0x98,
	0xFC, 0xF3, 0x0D, 0x3F, 0x39, 0x93, 0x01, 0xF8,
	0xFC, 0xF3, 0x0D, 0x3F, 0x3A, 0x81, 0xCC, 0x8C,
	0xFC, 0xF3, 0x0D, 0x3F, 0x3B, 0x34, 0x00, 0x4E,
	0xFC, 0xF3, 0x0D, 0x3F, 0x3C, 0x1A, 0x09, 0xEF,
	0xFC, 0xF3, 0x0D, 0x3F, 0x3D, 0x83, 0x01, 0xF8,
	0xFC, 0xF3, 0x0D, 0x3F, 0x3E, 0x92, 0x12, 0x98,
	0xFC, 0xF3, 0x0D, 0x3F, 0x3F, 0x83, 0x07, 0xFA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x40, 0x93, 0x01, 0xFA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x41, 0x82, 0x12, 0xAA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x42, 0x93, 0xFF, 0xEA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x43, 0x83, 0xFF, 0xEA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x44, 0x27, 0x8A, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x45, 0x1B, 0xF4, 0x31,
	0xFC, 0xF3, 0x0D, 0x3F, 0x46, 0x82, 0x12, 0x98,
	0xFC, 0xF3, 0x0D, 0x3F, 0x47, 0x93, 0x01, 0xF8,
	0xFC, 0xF3, 0x0D, 0x3F, 0x48, 0x81, 0x17, 0xE5,
	0xFC, 0xF3, 0x0D, 0x3F, 0x49, 0x34, 0x00, 0x4E,
	0xFC, 0xF3, 0x0D, 0x3F, 0x4A, 0x1A, 0x0C, 0xEF,
	0xFC, 0xF3, 0x0D, 0x3F, 0x4B, 0x83, 0x01, 0xF8,
	0xFC, 0xF3, 0x0D, 0x3F, 0x4C, 0x92, 0x12, 0x98,
	0xFC, 0xF3, 0x0D, 0x3F, 0x4D, 0x83, 0x07, 0xFA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x4E, 0x93, 0x01, 0xFA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x4F, 0x82, 0x12, 0xAA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x50, 0x93, 0xFF, 0xEA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x51, 0x83, 0xFF, 0xEA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x52, 0x27, 0x8A, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x53, 0x1B, 0xF5, 0x11,
	0xFC, 0xF3, 0x0D, 0x3F, 0x54, 0x82, 0x12, 0x98,
	0xFC, 0xF3, 0x0D, 0x3F, 0x55, 0x93, 0x01, 0xF8,
	0xFC, 0xF3, 0x0D, 0x3F, 0x56, 0x81, 0x17, 0xE5,
	0xFC, 0xF3, 0x0D, 0x3F, 0x57, 0x34, 0x00, 0x4E,
	0xFC, 0xF3, 0x0D, 0x3F, 0x58, 0x1A, 0x13, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x59, 0x8E, 0x77, 0xC5,
	0xFC, 0xF3, 0x0D, 0x3F, 0x5A, 0x34, 0xB8, 0x01,
	0xFC, 0xF3, 0x0D, 0x3F, 0x5B, 0x39, 0x60, 0x02,
	0xFC, 0xF3, 0x0D, 0x3F, 0x5C, 0x17, 0xF6, 0x0E,
	0xFC, 0xF3, 0x0D, 0x3F, 0x5D, 0x60, 0x00, 0x16,
	0xFC, 0xF3, 0x0D, 0x3F, 0x5E, 0x70, 0x00, 0x5A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x5F, 0x68, 0x00, 0x55,
	0xFC, 0xF3, 0x0D, 0x3F, 0x60, 0x78, 0x00, 0x19,
	0xFC, 0xF3, 0x0D, 0x3F, 0x61, 0x18, 0x59, 0xBF,
	0xFC, 0xF3, 0x0D, 0x3F, 0x62, 0x82, 0x30, 0x10,
	0xFC, 0xF3, 0x0D, 0x3F, 0x63, 0x26, 0xE0, 0xDF,
	0xFC, 0xF3, 0x0D, 0x3F, 0x64, 0x1B, 0xF6, 0x72,
	0xFC, 0xF3, 0x0D, 0x3F, 0x65, 0x3C, 0x00, 0x15,
	0xFC, 0xF3, 0x0D, 0x3F, 0x66, 0x19, 0x4F, 0xDF,
	0xFC, 0xF3, 0x0D, 0x3F, 0x67, 0x0C, 0x20, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x68, 0x82, 0x40, 0x03,
	0xFC, 0xF3, 0x0D, 0x3F, 0x69, 0x83, 0x24, 0x07,
	0xFC, 0xF3, 0x0D, 0x3F, 0x6A, 0x20, 0x29, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x6B, 0x92, 0x40, 0x0C,
	0xFC, 0xF3, 0x0D, 0x3F, 0x6C, 0x82, 0x4C, 0x03,
	0xFC, 0xF3, 0x0D, 0x3F, 0x6D, 0x20, 0x29, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x6E, 0x92, 0x4C, 0x0C,
	0xFC, 0xF3, 0x0D, 0x3F, 0x6F, 0x0C, 0x30, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x70, 0x19, 0x50, 0x2F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x71, 0x96, 0x2C, 0x76,
	0xFC, 0xF3, 0x0D, 0x3F, 0x72, 0x96, 0x2C, 0x86,
	0xFC, 0xF3, 0x0D, 0x3F, 0x73, 0x40, 0x01, 0x9A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x74, 0x92, 0x2C, 0x9A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x75, 0x35, 0x21, 0x91,
	0xFC, 0xF3, 0x0D, 0x3F, 0x76, 0x35, 0x21, 0x62,
	0xFC, 0xF3, 0x0D, 0x3F, 0x77, 0x35, 0x21, 0xC0,
	0xFC, 0xF3, 0x0D, 0x3F, 0x78, 0x3C, 0x00, 0xC5,
	0xFC, 0xF3, 0x0D, 0x3F, 0x79, 0x0C, 0x0C, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x7A, 0x17, 0xF8, 0xEE,
	0xFC, 0xF3, 0x0D, 0x3F, 0x7B, 0x60, 0x00, 0xA5,
	0xFC, 0xF3, 0x0D, 0x3F, 0x7C, 0x22, 0xFA, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x7D, 0x60, 0x00, 0x49,
	0xFC, 0xF3, 0x0D, 0x3F, 0x7E, 0x66, 0xE2, 0x51,
	0xFC, 0xF3, 0x0D, 0x3F, 0x7F, 0x1B, 0xF8, 0xC3,
	0xFC, 0xF3, 0x0D, 0x3F, 0x80, 0x26, 0xEA, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x81, 0x1B, 0xF8, 0xC3,
	0xFC, 0xF3, 0x0D, 0x3F, 0x82, 0x82, 0x2C, 0x8A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x83, 0x22, 0x62, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x84, 0x82, 0x2C, 0x94,
	0xFC, 0xF3, 0x0D, 0x3F, 0x85, 0x26, 0xE2, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x86, 0x1B, 0xF8, 0xA0,
	0xFC, 0xF3, 0x0D, 0x3F, 0x87, 0x82, 0x2C, 0x7A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x88, 0x22, 0x62, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x89, 0x92, 0x2C, 0x7A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x8A, 0x82, 0x2C, 0x9A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x8B, 0x92, 0x2C, 0x8A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x8C, 0x82, 0x2C, 0x9A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x8D, 0x22, 0x62, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x8E, 0x92, 0x2C, 0x9A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x8F, 0x0C, 0x08, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x90, 0x82, 0x30, 0x30,
	0xFC, 0xF3, 0x0D, 0x3F, 0x91, 0x23, 0x98, 0x5F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x92, 0x1B, 0xFA, 0x20,
	0xFC, 0xF3, 0x0D, 0x3F, 0x93, 0x82, 0x2C, 0x7A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x94, 0x82, 0x2C, 0xC4,
	0xFC, 0xF3, 0x0D, 0x3F, 0x95, 0x26, 0xE2, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x96, 0x1B, 0xF9, 0xE5,
	0xFC, 0xF3, 0x0D, 0x3F, 0x97, 0x82, 0x2C, 0xAA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x98, 0x22, 0xE2, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x99, 0x92, 0x2C, 0xAA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x9A, 0x1B, 0xFA, 0x22,
	0xFC, 0xF3, 0x0D, 0x3F, 0x9B, 0x96, 0x2C, 0xA6,
	0xFC, 0xF3, 0x0D, 0x3F, 0x9C, 0x96, 0x2C, 0xB6,
	0xFC, 0xF3, 0x0D, 0x3F, 0x9D, 0x1B, 0xFA, 0x2F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x9E, 0x41, 0x00, 0x14,
	0xFC, 0xF3, 0x0D, 0x3F, 0x9F, 0x92, 0x2C, 0xB4,
	0xFC, 0xF3, 0x0D, 0x3F, 0xA0, 0x40, 0x01, 0x44,
	0xFC, 0xF3, 0x0D, 0x3F, 0xA1, 0x92, 0x2C, 0xA4,
	0xFC, 0xF3, 0x0D, 0x3F, 0xA2, 0x3C, 0x00, 0x75,
	0xFC, 0xF3, 0x0D, 0x3F, 0xA3, 0x19, 0x3C, 0xAF,
	0xFC, 0xF3, 0x0D, 0x3F, 0xA4, 0x82, 0x30, 0x30,
	0xFC, 0xF3, 0x0D, 0x3F, 0xA5, 0x23, 0x98, 0x5F,
	0xFC, 0xF3, 0x0D, 0x3F, 0xA6, 0x1B, 0xFB, 0x50,
	0xFC, 0xF3, 0x0D, 0x3F, 0xA7, 0x82, 0x2C, 0xBA,
	0xFC, 0xF3, 0x0D, 0x3F, 0xA8, 0x26, 0x7A, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0xA9, 0x1B, 0xFB, 0x50,
	0xFC, 0xF3, 0x0D, 0x3F, 0xAA, 0x82, 0x30, 0x1A,
	0xFC, 0xF3, 0x0D, 0x3F, 0xAB, 0x26, 0x7A, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0xAC, 0x40, 0x01, 0x6A,
	0xFC, 0xF3, 0x0D, 0x3F, 0xAD, 0x40, 0x01, 0xF1,
	0xFC, 0xF3, 0x0D, 0x3F, 0xAE, 0x22, 0x79, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0xAF, 0x0D, 0x04, 0x7A,
	0xFC, 0xF3, 0x0D, 0x3F, 0xB0, 0x37, 0x24, 0x01,
	0xFC, 0xF3, 0x0D, 0x3F, 0xB1, 0x09, 0x00, 0x07,
	0xFC, 0xF3, 0x0D, 0x3F, 0xB2, 0xA7, 0xFF, 0xF5,
	0xFC, 0xF3, 0x0D, 0x3F, 0xB3, 0xA7, 0xFF, 0xF5,
	0xFC, 0xF3, 0x0D, 0x3F, 0xB4, 0xA7, 0xFF, 0xF5,
	0xFC, 0xF3, 0x0D, 0x3F, 0xB5, 0x3B, 0x24, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0xB6, 0x19, 0x4F, 0x3F,
	0xFC, 0xF3, 0x0D, 0x3F, 0xB7, 0x82, 0x2E, 0xA0,
	0xFC, 0xF3, 0x0D, 0x3F, 0xB8, 0x27, 0x80, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0xB9, 0x1B, 0xFB, 0xE0,
	0xFC, 0xF3, 0x0D, 0x3F, 0xBA, 0x3C, 0x00, 0x25,
	0xFC, 0xF3, 0x0D, 0x3F, 0xBB, 0x39, 0x16, 0x12,
	0xFC, 0xF3, 0x0D, 0x3F, 0xBC, 0x35, 0x01, 0xA1,
	0xFC, 0xF3, 0x0D, 0x3F, 0xBD, 0x1D, 0xBE, 0x8F,
	0xFC, 0xF3, 0x0D, 0x3F, 0xBE, 0x81, 0x0B, 0xF1,
	0xFC, 0xF3, 0x0D, 0x3F, 0xBF, 0x18, 0x31, 0xDF,
	0xFC, 0xF3, 0x68, 0x64, 0x00,
	0xFC, 0xF3, 0x3B, 0x3F, 0xA0, 0x91, 0x83,
	0xFC, 0xF3, 0x3B, 0x3F, 0xB0, 0x3F, 0x00,
	0xFC, 0xF3, 0x3B, 0x3F, 0xA1, 0x91, 0xB3,
	0xFC, 0xF3, 0x3B, 0x3F, 0xB1, 0x3F, 0x11,
	0xFC, 0xF3, 0x3B, 0x3F, 0xA2, 0x92, 0x16,
	0xFC, 0xF3, 0x3B, 0x3F, 0xB2, 0x3F, 0x1E,
	0xFC, 0xF3, 0x3B, 0x3F, 0xA3, 0xE0, 0x9D,
	0xFC, 0xF3, 0x3B, 0x3F, 0xB3, 0x3F, 0x2B,
	0xFC, 0xF3, 0x3B, 0x3F, 0xA4, 0xE0, 0xCD,
	0xFC, 0xF3, 0x3B, 0x3F, 0xB4, 0x3F, 0x3D,
	0xFC, 0xF3, 0x3B, 0x3F, 0xA5, 0xE1, 0x30,
	0xFC, 0xF3, 0x3B, 0x3F, 0xB5, 0x3F, 0x4B,
	0xFC, 0xF3, 0x3B, 0x3F, 0xA6, 0x85, 0x9A,
	0xFC, 0xF3, 0x3B, 0x3F, 0xB6, 0x3F, 0x59,
	0xFC, 0xF3, 0x3B, 0x3F, 0xA7, 0x94, 0xFC,
	0xFC, 0xF3, 0x3B, 0x3F, 0xB7, 0x3F, 0x62,
	0xFC, 0xF3, 0x3B, 0x3F, 0xA8, 0x93, 0xC9,
	0xFC, 0xF3, 0x3B, 0x3F, 0xB8, 0x3F, 0x71,
	0xFC, 0xF3, 0x3B, 0x3F, 0xA9, 0x94, 0xF2,
	0xFC, 0xF3, 0x3B, 0x3F, 0xB9, 0x3F, 0xA4,
	0xFC, 0xF3, 0x3B, 0x22, 0xCC, 0x00, 0x01,
	0xFC, 0xF3, 0x3B, 0x3F, 0xAA, 0x82, 0x3A,
	0xFC, 0xF3, 0x3B, 0x3F, 0xBA, 0x02, 0x3E,
	0xFC, 0xF3, 0x3B, 0x3F, 0xAB, 0x83, 0x1C,
	0xFC, 0xF3, 0x3B, 0x3F, 0xBB, 0x3F, 0xB7,
	0xFC, 0xF3, 0x6A, 0x10, 0x00, 0x01,
	0xFC, 0xF3, 0x68, 0x64, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x00, 0x26, 0xE1, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x01, 0x19, 0x24, 0x9F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x02, 0x82, 0xF5, 0x50,
	0xFC, 0xF3, 0x0D, 0x3F, 0x03, 0x26, 0xE0, 0xDF,
	0xFC, 0xF3, 0x0D, 0x3F, 0x04, 0x1B, 0xF0, 0x72,
	0xFC, 0xF3, 0x0D, 0x3F, 0x05, 0x34, 0x00, 0x08,
	0xFC, 0xF3, 0x0D, 0x3F, 0x06, 0x19, 0x8E, 0x3F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x07, 0x0C, 0x20, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x08, 0x80, 0xF5, 0xB3,
	0xFC, 0xF3, 0x0D, 0x3F, 0x09, 0x81, 0x49, 0xB7,
	0xFC, 0xF3, 0x0D, 0x3F, 0x0A, 0x20, 0x29, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x0B, 0x90, 0xF5, 0xBC,
	0xFC, 0xF3, 0x0D, 0x3F, 0x0C, 0x81, 0x01, 0xB3,
	0xFC, 0xF3, 0x0D, 0x3F, 0x0D, 0x20, 0x29, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x0E, 0x91, 0x01, 0xBC,
	0xFC, 0xF3, 0x0D, 0x3F, 0x0F, 0x0C, 0x30, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x10, 0x19, 0x8E, 0x9F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x11, 0x94, 0x33, 0x06,
	0xFC, 0xF3, 0x0D, 0x3F, 0x12, 0x94, 0x33, 0x16,
	0xFC, 0xF3, 0x0D, 0x3F, 0x13, 0x40, 0x01, 0x9A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x14, 0x90, 0x33, 0x2A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x15, 0x35, 0x27, 0x41,
	0xFC, 0xF3, 0x0D, 0x3F, 0x16, 0x35, 0x27, 0x12,
	0xFC, 0xF3, 0x0D, 0x3F, 0x17, 0x35, 0x27, 0x70,
	0xFC, 0xF3, 0x0D, 0x3F, 0x18, 0x3C, 0x00, 0xC5,
	0xFC, 0xF3, 0x0D, 0x3F, 0x19, 0x0C, 0x0C, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x1A, 0x17, 0xF2, 0xEE,
	0xFC, 0xF3, 0x0D, 0x3F, 0x1B, 0x60, 0x00, 0xA5,
	0xFC, 0xF3, 0x0D, 0x3F, 0x1C, 0x22, 0xFA, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x1D, 0x60, 0x00, 0x49,
	0xFC, 0xF3, 0x0D, 0x3F, 0x1E, 0x66, 0xE2, 0x51,
	0xFC, 0xF3, 0x0D, 0x3F, 0x1F, 0x1B, 0xF2, 0xC3,
	0xFC, 0xF3, 0x0D, 0x3F, 0x20, 0x26, 0xEA, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x21, 0x1B, 0xF2, 0xC3,
	0xFC, 0xF3, 0x0D, 0x3F, 0x22, 0x80, 0x33, 0x1A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x23, 0x22, 0x62, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x24, 0x80, 0x33, 0x24,
	0xFC, 0xF3, 0x0D, 0x3F, 0x25, 0x26, 0xE2, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x26, 0x1B, 0xF2, 0xA0,
	0xFC, 0xF3, 0x0D, 0x3F, 0x27, 0x80, 0x33, 0x0A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x28, 0x22, 0x62, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x29, 0x90, 0x33, 0x0A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x2A, 0x80, 0x33, 0x2A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x2B, 0x90, 0x33, 0x1A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x2C, 0x80, 0x33, 0x2A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x2D, 0x22, 0x62, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x2E, 0x90, 0x33, 0x2A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x2F, 0x0C, 0x08, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x30, 0x82, 0xF5, 0x60,
	0xFC, 0xF3, 0x0D, 0x3F, 0x31, 0x23, 0x98, 0x5F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x32, 0x1B, 0xF4, 0x20,
	0xFC, 0xF3, 0x0D, 0x3F, 0x33, 0x80, 0x33, 0x0A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x34, 0x80, 0x33, 0x54,
	0xFC, 0xF3, 0x0D, 0x3F, 0x35, 0x26, 0xE2, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x36, 0x1B, 0xF3, 0xE5,
	0xFC, 0xF3, 0x0D, 0x3F, 0x37, 0x80, 0x33, 0x3A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x38, 0x22, 0xE2, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x39, 0x90, 0x33, 0x3A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x3A, 0x1B, 0xF4, 0x22,
	0xFC, 0xF3, 0x0D, 0x3F, 0x3B, 0x94, 0x33, 0x36,
	0xFC, 0xF3, 0x0D, 0x3F, 0x3C, 0x94, 0x33, 0x46,
	0xFC, 0xF3, 0x0D, 0x3F, 0x3D, 0x1B, 0xF4, 0x2F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x3E, 0x41, 0x00, 0x14,
	0xFC, 0xF3, 0x0D, 0x3F, 0x3F, 0x90, 0x33, 0x44,
	0xFC, 0xF3, 0x0D, 0x3F, 0x40, 0x40, 0x01, 0x44,
	0xFC, 0xF3, 0x0D, 0x3F, 0x41, 0x90, 0x33, 0x34,
	0xFC, 0xF3, 0x0D, 0x3F, 0x42, 0x3C, 0x00, 0x75,
	0xFC, 0xF3, 0x0D, 0x3F, 0x43, 0x19, 0x2F, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x44, 0x82, 0xF5, 0x60,
	0xFC, 0xF3, 0x0D, 0x3F, 0x45, 0x23, 0x98, 0x5F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x46, 0x1B, 0xF5, 0x50,
	0xFC, 0xF3, 0x0D, 0x3F, 0x47, 0x80, 0x33, 0x4A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x48, 0x26, 0x7A, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x49, 0x1B, 0xF5, 0x50,
	0xFC, 0xF3, 0x0D, 0x3F, 0x4A, 0x82, 0xF5, 0x5A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x4B, 0x26, 0x7A, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x4C, 0x40, 0x01, 0x6A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x4D, 0x40, 0x01, 0xF1,
	0xFC, 0xF3, 0x0D, 0x3F, 0x4E, 0x22, 0x79, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x4F, 0x0D, 0x04, 0x7A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x50, 0x35, 0x49, 0xB1,
	0xFC, 0xF3, 0x0D, 0x3F, 0x51, 0x09, 0x00, 0x07,
	0xFC, 0xF3, 0x0D, 0x3F, 0x52, 0xA7, 0xFF, 0xF5,
	0xFC, 0xF3, 0x0D, 0x3F, 0x53, 0xA7, 0xFF, 0xF5,
	0xFC, 0xF3, 0x0D, 0x3F, 0x54, 0xA7, 0xFF, 0xF5,
	0xFC, 0xF3, 0x0D, 0x3F, 0x55, 0x39, 0x49, 0xB0,
	0xFC, 0xF3, 0x0D, 0x3F, 0x56, 0x19, 0x84, 0x3F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x57, 0x82, 0xB6, 0x61,
	0xFC, 0xF3, 0x0D, 0x3F, 0x58, 0x17, 0xF8, 0x6E,
	0xFC, 0xF3, 0x0D, 0x3F, 0x59, 0x82, 0x91, 0x6A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x5A, 0x60, 0x2A, 0x65,
	0xFC, 0xF3, 0x0D, 0x3F, 0x5B, 0x20, 0x84, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x5C, 0x0D, 0x00, 0xFC,
	0xFC, 0xF3, 0x0D, 0x3F, 0x5D, 0x0D, 0x00, 0xEB,
	0xFC, 0xF3, 0x0D, 0x3F, 0x5E, 0x20, 0x58, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x5F, 0x10, 0x67, 0xBE,
	0xFC, 0xF3, 0x0D, 0x3F, 0x60, 0x10, 0x73, 0x87,
	0xFC, 0xF3, 0x0D, 0x3F, 0x61, 0x10, 0x47, 0xA9,
	0xFC, 0xF3, 0x0D, 0x3F, 0x62, 0x0E, 0x5B, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x63, 0x22, 0x62, 0x9F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x64, 0x90, 0x2F, 0x9F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x65, 0x90, 0x2F, 0xAA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x66, 0x1C, 0x95, 0xCF,
	0xFC, 0xF3, 0x0D, 0x3F, 0x67, 0x1C, 0x50, 0xCF,
	0xFC, 0xF3, 0x0D, 0x3F, 0x68, 0x0F, 0x22, 0xFF,
	0xFC, 0xF3, 0x0D, 0x3F, 0x69, 0x80, 0x2F, 0x9A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x6A, 0x0D, 0x00, 0x5F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x6B, 0x40, 0x00, 0x04,
	0xFC, 0xF3, 0x0D, 0x3F, 0x6C, 0x06, 0x0A, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x6D, 0x07, 0x12, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x6E, 0x07, 0x12, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x6F, 0x07, 0x12, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x70, 0x07, 0x12, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x71, 0x07, 0x12, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x72, 0x07, 0x12, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x73, 0x07, 0x12, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x74, 0x07, 0x12, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x75, 0x07, 0x12, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x76, 0x07, 0x12, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x77, 0x07, 0x12, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x78, 0x07, 0x12, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x79, 0x07, 0x12, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x7A, 0x07, 0x12, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x7B, 0x07, 0x12, 0x00,
	0xFC, 0xF3, 0x0D, 0x3F, 0x7C, 0x80, 0x2F, 0xAA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x7D, 0x2B, 0x3A, 0xB4,
	0xFC, 0xF3, 0x0D, 0x3F, 0x7E, 0x22, 0x62, 0x1F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x7F, 0x0D, 0x00, 0x9A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x80, 0x10, 0x23, 0x61,
	0xFC, 0xF3, 0x0D, 0x3F, 0x81, 0x20, 0x27, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x82, 0x42, 0x8B, 0xE6,
	0xFC, 0xF3, 0x0D, 0x3F, 0x83, 0x20, 0x24, 0x0F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x84, 0x82, 0xB7, 0x84,
	0xFC, 0xF3, 0x0D, 0x3F, 0x85, 0x2B, 0x24, 0x78,
	0xFC, 0xF3, 0x0D, 0x3F, 0x86, 0x92, 0xB7, 0x8A,
	0xFC, 0xF3, 0x0D, 0x3F, 0x87, 0x1A, 0x20, 0x7F,
	0xFC, 0xF3, 0x0D, 0x3F, 0x88, 0x47, 0xFF, 0xFA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x89, 0x92, 0xB5, 0xFA,
	0xFC, 0xF3, 0x0D, 0x3F, 0x8A, 0x35, 0x6D, 0xB1,
	0xFC, 0xF3, 0x0D, 0x3F, 0x8B, 0x19, 0xE4, 0x0F,
	0xFC, 0xF3, 0x68, 0x64, 0x00,
	0xFC, 0xF3, 0x3B, 0x3F, 0xA0, 0x92, 0x48,
	0xFC, 0xF3, 0x3B, 0x3F, 0xB0, 0x3F, 0x00,
	0xFC, 0xF3, 0x3B, 0x3F, 0xA1, 0x98, 0xE2,
	0xFC, 0xF3, 0x3B, 0x3F, 0xB1, 0x3F, 0x02,
	0xFC, 0xF3, 0x3B, 0x3F, 0xA2, 0x92, 0xF0,
	0xFC, 0xF3, 0x3B, 0x3F, 0xB2, 0x3F, 0x11,
	0xFC, 0xF3, 0x3B, 0x3F, 0xA3, 0x98, 0x42,
	0xFC, 0xF3, 0x3B, 0x3F, 0xB3, 0x3F, 0x44,
	0xFC, 0xF3, 0x3B, 0x03, 0x35, 0x00, 0x01,
	0xFC, 0xF3, 0x3B, 0x3F, 0xA4, 0xA1, 0xD6,
	0xFC, 0xF3, 0x3B, 0x3F, 0xB4, 0x3F, 0x57,
	0xFC, 0xF3, 0x3B, 0x3F, 0xA5, 0x9E, 0x3F,
	0xFC, 0xF3, 0x3B, 0x3F, 0xB5, 0x3F, 0x88,
	0xFC, 0xF3, 0x6A, 0x10, 0x00, 0x00,
	0xFC, 0xF3, 0x3B, 0x22, 0x66, 0x00, 0x1B,
	0xFC, 0xF3, 0x3B, 0x22, 0x67, 0x7D, 0x00,
	0xFC, 0xF3, 0x3B, 0x22, 0x6A, 0x7D, 0x00,
	0xFC, 0xF3, 0x3B, 0x22, 0x82, 0x00, 0x00,
	0xFC, 0xF3, 0x3B, 0x22, 0x83, 0x00, 0x01,
	0xFC, 0xF3, 0x3B, 0x22, 0x88, 0x00, 0x00,
	0xFC, 0xF3, 0x3B, 0x22, 0xE5, 0x04, 0x20,
	0xFC, 0xF3, 0x3B, 0x22, 0x72, 0x00, 0x00,
	0xFC, 0xF3, 0x3B, 0x23, 0x2B, 0x00, 0x20,
	0xFC, 0xF3, 0x3B, 0x23, 0x0C, 0x00, 0x4A,
	0xFC, 0xF3, 0x3B, 0x22, 0x61, 0xF8, 0xD9,
	0xFC, 0xF3, 0x3B, 0x22, 0x6E, 0x00, 0x24,
	0xFC, 0xF3, 0x3B, 0x22, 0xEB, 0x00, 0x01,
	0xFC, 0xF3, 0x3B, 0x22, 0xF2, 0x00, 0x48,
	0xFC, 0xF3, 0x3B, 0x22, 0xF8, 0x80, 0x01,
	0xFC, 0xF3, 0x3B, 0x23, 0x03, 0x07, 0x00,
	0xFC, 0xF3, 0x3B, 0x23, 0x02, 0x00, 0x01,
	0xFC, 0xF3, 0x3B, 0x22, 0xFD, 0x00, 0x9E,
	0xFC, 0xF3, 0x3B, 0x23, 0x05, 0x00, 0x00,
	0xFC, 0xF3, 0x3B, 0x22, 0x74, 0x00, 0x02,
	0xFC, 0xF3, 0x3B, 0x23, 0x0D, 0x00, 0x80,
	0xFC, 0xF3, 0x3B, 0x22, 0x68, 0x3E, 0x80,
	0xFC, 0xF3, 0x3B, 0x22, 0x69, 0x3E, 0x80,
	0xFC, 0xF3, 0x3B, 0x23, 0x01, 0x00, 0x02,
	0xFC, 0xF3, 0x3B, 0x22, 0xFB, 0x00, 0x00
};

static fm36_info_t fm36_info = {
	.reset_pin = {
		.group = 0,
		.bit = 0,
	},
	.init_data = {
		.data = NULL,
		.len = 0,
	},
};

static int fm36_write_data(fm36_data_t *data)
{
	int i = 0;
	fm36_debug("fm36_write_data");

	if ((!data) || (!data->data) || (!data->len)) {
		fm36_error("i2c: data error");
		return -EINVAL;
	}

	i2c_start();

	while (i < data->len) {
		i2c_write_byte(data->data[i]);
		if (i2c_ack(I2C_WAIT_ACK)) {
			fm36_error("fm36: wait ack failed, no ack");
			i2c_stop();
			return -1;
		}
		if (0xFC == data->data[i]) {
			printk("\n");
		}
		printk("0x%X  ", data->data[i]);
		i++;
	}
	printk("\n");
	i2c_stop();

	return i;
}

static int fm36_only_write_data(fm36_data_t *data)
{
	int i = 0;
	fm36_debug("fm36_only_write_data");

	if ((!data) || (!data->data) || (!data->len)) {
		fm36_error("data error");
		return -EINVAL;
	}

	while (i < data->len) {
		i2c_write_byte(data->data[i]);
		if (i2c_ack(I2C_WAIT_ACK)) {
			fm36_error("fm36: wait ack failed, no ack");
			i2c_stop();
			return -1;
		}
		if (0xFC == data->data[i]) {
			printk("\n");
		}
		printk("0x%X  ", data->data[i]);
		i++;
	}
	printk("\n");

	return i;
}

static int fm36_write_chip_addr(unsigned char chip_addr)
{
	int i = 0;
	fm36_debug("fm36_write_chip_addr");

	i2c_write_byte(chip_addr);
	if (i2c_ack(I2C_WAIT_ACK)) {
		fm36_error("wait ack failed, no ack");
		i2c_stop();
		return -1;
	}
	fm36_debug("chip address: 0x%X", chip_addr);

	return i;
}

static int fm36_write_register(fm36_write_reg_t *reg_data)
{
	fm36_debug("fm36_write_register");

	return 0;
}

static int fm36_read_register(fm36_read_reg_t *reg_data)
{
	unsigned char write_buf[5] = { 0 };
	unsigned char read_buf[5] = { 0 };
	unsigned char data = 0;
	fm36_data_t fm36_data;
	int ret = -1;
	fm36_debug("fm36_read_register");

	if (!reg_data) {
		fm36_error("i2c: data error");
		return -EINVAL;
	}

	memset(&fm36_data, 0, sizeof(fm36_data));

	i2c_start();
	ret = fm36_write_chip_addr(FM36_CHIP_ADDR);
	if (ret < 0) {
		fm36_error("write chip address failed, ret = %d", ret);
		return -EFAULT;
	}

	write_buf[0] = 0xFC;
	write_buf[1] = 0xF3;
	write_buf[2] = 0x37;
	write_buf[3] = (reg_data->reg & 0xFF00) >> 8;
	write_buf[4] = reg_data->reg & 0xFF;

	fm36_data.data = write_buf;
	fm36_data.len = sizeof(write_buf);
	ret = fm36_only_write_data(&fm36_data);
	if (ret < 0) {
		fm36_error("write data failed, ret = %d", ret);
		return -EFAULT;
	}

	i2c_start();
	ret = fm36_write_chip_addr(FM36_CHIP_ADDR + 1);
	if (ret < 0) {
		fm36_error("write chip address failed, ret = %d", ret);
		return -EFAULT;
	}

	ret = i2c_read_byte(&data);
	i2c_stop();

	// Read high register
	i2c_start();
	ret = fm36_write_chip_addr(FM36_CHIP_ADDR);
	if (ret < 0) {
		fm36_error("write chip address failed, ret = %d", ret);
		return -EFAULT;
	}

	read_buf[0] = 0xFC;
	read_buf[1] = 0xF3;
	read_buf[2] = 0x60;
	read_buf[3] = 0x26;

	fm36_data.data = read_buf;
	fm36_data.len = sizeof(read_buf);
	ret = fm36_only_write_data(&fm36_data);
	if (ret < 0) {
		fm36_error("write data failed, ret = %d", ret);
		return -EFAULT;
	}

	i2c_start();
	ret = fm36_write_chip_addr(FM36_CHIP_ADDR + 1);
	if (ret < 0) {
		fm36_error("write chip address failed, ret = %d", ret);
		return -EFAULT;
	}

	ret = i2c_read_byte(&data);
	i2c_stop();
	fm36_debug("i2c read high byte: %d", data);

	//fm36_data.data |= data << 8;

	// Read low register
	i2c_start();
	ret = fm36_write_chip_addr(FM36_CHIP_ADDR);
	if (ret < 0) {
		fm36_error("write chip address failed, ret = %d", ret);
		return -EFAULT;
	}

	read_buf[0] = 0xFC;
	read_buf[1] = 0xF3;
	read_buf[2] = 0x60;
	read_buf[3] = 0x25;

	fm36_data.data = read_buf;
	fm36_data.len = sizeof(read_buf);
	ret = fm36_only_write_data(&fm36_data);
	if (ret < 0) {
		fm36_error("write data failed, ret = %d", ret);
		return -EFAULT;
	}

	i2c_start();
	ret = fm36_write_chip_addr(FM36_CHIP_ADDR + 1);
	if (ret < 0) {
		fm36_error("write chip address failed, ret = %d", ret);
		return -EFAULT;
	}

	ret = i2c_read_byte(&data);
	i2c_stop();
	fm36_debug("i2c read low byte: %d", data);

	//fm36_data.data |= data;

	//fm36_debug("i2c read byte: %d", fm36_data.data);

	return 0;
}

/*
 *static int fm36_check_reset_pin_support(void)
 *{
 *    fm36_debug("fm36_check_reset_pin_support");
 *
 *    if (fm36_info.reset_pin_value) {
 *        return 1;
 *    } else {
 *        fm36_debug("fm36: no support chip reset");
 *        return 0;
 *    }
 *}
 */

static int fm36_chip_reset(void)
{
	int ret = -1;
	fm36_debug("fm36_chip_reset");

    /*
	 *if (!fm36_check_reset_pin_support()) {
	 *    fm36_error("fm36: chip reset no support");
	 *    return -1;
	 *}
     */

	ret = set_gpio_output_val(&fm36_info.reset_pin, 0);
	if (ret) {
		fm36_error("fm36: chip reset gpio output low failed, ret = %d", ret);
		return -1;
	}
	mdelay(100);

	ret = set_gpio_output_val(&fm36_info.reset_pin, 1);
	if (ret) {
		fm36_error("fm36: chip reset gpio output high failed, ret = %d", ret);
		return -1;
	}
	mdelay(10);

	return 0;
}

static int fm36_init_register(void)
{
	int ret = -1;
	fm36_debug("fm36_init_register");

	ret = fm36_write_data(&fm36_info.init_data);
	if (ret < 0) {
		fm36_error("fm36: write data failed");
		return -1;
	} else if (ret < fm36_info.init_data.len) {
		fm36_error("fm36: write data no complete");
		return -1;
	}
	fm36_debug("fm36: write init data OK, len = %d", ret);

	return 0;
}

static int fm36_reset_gpio_init(void)
{
	int ret = -1;
	fm36_debug("fm36_reset_gpio_init");

	ret = set_gpio_output(&fm36_info.reset_pin);
	if (ret) {
		fm36_error("fm36: chip reset gpio config output failed, ret = %d", ret);
		return -1;
	}

	return 0;
}

static int fm36_i2c_gpio_init(void)
{
    /*
	 *int ret = -1;
     */
	fm36_debug("fm36_i2c_gpio_init");

/*
 *    ret = set_gpio_output(&fm36_info.i2c_pin.scl);
 *    if (ret) {
 *        fm36_error("fm36: i2c scl gpio config output failed, ret = %d", ret);
 *        return -1;
 *    }
 *
 *    ret = set_gpio_output(&fm36_info.i2c_pin.sda);
 *    if (ret) {
 *        fm36_error("fm36: i2c sda gpio config output failed, ret = %d", ret);
 *        return -1;
 *    }
 */

	return 0;
}

static int fm36_gpio_init(void)
{
	int ret = -1;
	fm36_debug("fm36_gpio_init");

	ret = fm36_reset_gpio_init();
	if (ret) {
		fm36_error("fm36: chip reset gpio init failed, ret = %d", ret);
		return -1;
	}

	ret = fm36_i2c_gpio_init();
	if (ret) {
		fm36_error("fm36: i2c gpio init failed, ret = %d", ret);
		return -1;
	}

	return 0;
}

/* FM36 module frame */

int fm36_init (void)
{
	int ret = -1;
	fm36_debug("fm36_init\n");

	spin_lock_init(&fm36_info.lock);

	spin_lock(&fm36_info.lock);
	fm36_info.init_data.data = fm36_init_data;
	fm36_info.init_data.len = sizeof(fm36_init_data);
	spin_unlock(&fm36_info.lock);

	ret = fm36_gpio_init();
	if (ret) {
		fm36_error("fm36: gpio init failed, ret = %d", ret);
		return -1;
	}

	ret = i2c_init();
	if (ret) {
		fm36_error("fm36: i2c init failed, ret = %d", ret);
		return -1;
	}

	return 0;
}

int fm36_uninit(void)
{
	printk("fm36_uninit\n");

	return 0;
}

int fm36_operation(unsigned int cmd, unsigned long args)
{
	unsigned long len = 0;
	int ret = -1;
	fm36_read_reg_t read_data;
	fm36_write_reg_t write_data;

	switch (cmd) {
	case FM36_IOC_CHIP_RESET:
		ret = fm36_chip_reset();
		break;
	case FM36_IOC_INIT_REGISTER:
		ret = fm36_init_register();
		break;
	case FM36_IOC_READ_DATA:
		len = copy_from_user((void *)&read_data, (void __user *)args, 4);
		if (len) {
			fm36_error("copy_from_user failed, len = %ld", len);
			return -EFAULT;
		}
		ret = fm36_read_register(&read_data);
		break;
	case FM36_IOC_WRITE_DATA:
		ret = fm36_write_register(&write_data);
		break;
	default:
		fm36_error("cmd error no = %d \n", cmd);
		ret = -EINVAL;
		break;
	}

	if (ret < 0) {
		fm36_error("fm36 operations failed");
	}

	return ret;
}

