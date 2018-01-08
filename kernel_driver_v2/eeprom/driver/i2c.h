#ifndef __I2C_H__
#define	__I2C_H__

#include "tiny4412.h"

#define	I2C_DELAY			10

extern volatile unsigned long *gpa1con;
extern volatile unsigned long *gpa1dat;

#define	SET_SDA_OUTPUT	set_gpio_output(gpa1con, 2)
#define	SET_SDA_INPUT	set_gpio_input(gpa1con, 2)
#define	SET_SCL_OUTPUT	set_gpio_output(gpa1con, 3)

#define	SET_SDA_HIGH	set_gpio_high(gpa1dat, 2)
#define	SET_SDA_LOW		set_gpio_low(gpa1dat, 2)

#define	SET_SCL_HIGH	set_gpio_high(gpa1dat, 3)
#define	SET_SCL_LOW		set_gpio_low(gpa1dat, 3)

#define	GET_SDA_VAL		get_gpio_value(gpa1dat, 2)
#define	GET_SCL_VAL		get_gpio_value(gpa1dat, 3)

typedef enum {
	I2C_WAIT_ACK = 0x0,
	I2C_ACK = 0x1,
	I2C_NOACK = 0x2,
} i2c_ack_e;


void i2c_scl_init(void);

void i2c_sda_init(void);

void i2c_gpio_init(void);

int i2c_init(void);

int i2c_start(void);

int i2c_stop(void);

int i2c_send_ack(void);

int i2c_send_noack(void);

int i2c_wait_ack(void);

int i2c_ack(char ack_status);

int i2c_write_byte(char data);

int i2c_read_byte(char *data);


#endif
