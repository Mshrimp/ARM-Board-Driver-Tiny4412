#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/delay.h>

#include "i2c.h"


spinlock_t lock;
struct mutex i2c_mutex;

void i2c_scl_init(void)
{
	printk("i2c: scl init\n");
	SET_SCL_OUTPUT;
	SET_SCL_HIGH;
}

void i2c_sda_init(void)
{
	printk("i2c: sda init\n");
	SET_SDA_OUTPUT;
	SET_SDA_HIGH;
}

void i2c_gpio_init(void)
{
	printk("i2c_gpio_init\n");
	i2c_sda_init();
	i2c_scl_init();
}


int i2c_init(void)
{
	printk("i2c_init\n");
	spin_lock_init(&lock);
	mutex_init(&i2c_mutex);

	i2c_gpio_init();

	return 0;
}

int i2c_start(void)
{
	printk("i2c_start\n");

	mutex_lock(&i2c_mutex);
	SET_SDA_HIGH;
	udelay(I2C_DELAY);

	SET_SCL_HIGH;
	udelay(I2C_DELAY);

	SET_SDA_LOW;
	udelay(I2C_DELAY);

	SET_SCL_LOW;
	udelay(I2C_DELAY);
	mutex_unlock(&i2c_mutex);

	return 0;
}

int i2c_stop(void)
{
	printk("i2c_stop\n");

	mutex_lock(&i2c_mutex);
	SET_SCL_LOW;
	udelay(I2C_DELAY);

	SET_SDA_LOW;
	udelay(I2C_DELAY);

	SET_SCL_HIGH;
	udelay(I2C_DELAY);

	SET_SDA_HIGH;
	udelay(I2C_DELAY);
	mutex_unlock(&i2c_mutex);

	return 0;
}

int i2c_send_ack(void)
{
	printk("i2c_send_ack\n");

	mutex_lock(&i2c_mutex);
	SET_SCL_LOW;
	udelay(I2C_DELAY);

	SET_SDA_LOW;
	udelay(I2C_DELAY);

	SET_SCL_HIGH;
	udelay(I2C_DELAY);

	SET_SCL_LOW;
	udelay(I2C_DELAY);
	mutex_unlock(&i2c_mutex);

	return 0;
}

int i2c_send_noack(void)
{
	printk("i2c_send_noack\n");

	mutex_lock(&i2c_mutex);
	SET_SCL_LOW;
	udelay(I2C_DELAY);

	SET_SDA_HIGH;
	udelay(I2C_DELAY);

	SET_SCL_HIGH;
	udelay(I2C_DELAY);

	SET_SCL_LOW;
	udelay(I2C_DELAY);
	mutex_unlock(&i2c_mutex);

	return 0;
}

int i2c_wait_ack(void)
{
	int ack_times = 0;
	int ret = 0;

	printk("i2c: wait ack\n");
	mutex_lock(&i2c_mutex);
	SET_SDA_INPUT;
	udelay(I2C_DELAY);

	SET_SCL_LOW;
	udelay(I2C_DELAY);

	SET_SCL_HIGH;
	udelay(I2C_DELAY);

	ack_times = 0;
	while (GET_SDA_VAL) {
		ack_times++;
		if (ack_times == 10) {
			ret = 1;
			break;
		}
		udelay(I2C_DELAY);
	}

	SET_SCL_LOW;
	mutex_unlock(&i2c_mutex);

	return ret;
}

int i2c_ack(char ack_status)
{
	int ret = -1;

	printk("i2c: ack\n");

	switch (ack_status) {
	case I2C_WAIT_ACK:
		ret = i2c_wait_ack();
		break;
	case I2C_ACK:
		ret = i2c_send_ack();
		break;
	case I2C_NOACK:
		ret = i2c_send_noack();
		break;
	default:
		printk("i2c: ack status error\n");
		return -EINVAL;
	}

	if (ret) {
		printk("i2c: ack error\n");
	}

	return 0;
}

int i2c_write_byte(char data)
{
	unsigned long flag = 0;
	char i = 0;
	printk("i2c: i2c write data: %d\n", data);

	local_irq_save(flag);
	preempt_disable();
	mutex_lock(&i2c_mutex);
	SET_SDA_OUTPUT;
	udelay(I2C_DELAY);

	for (i = 0; i < 8; i++) {
		if (data & 0x80) {
			printk("i2c: write data, set sda high\n");
			SET_SDA_HIGH;
		} else {
			printk("i2c: write data, set sda low\n");
			SET_SDA_LOW;
		}
		udelay(I2C_DELAY);

		SET_SCL_HIGH;
		udelay(I2C_DELAY);

		SET_SCL_LOW;
		udelay(I2C_DELAY);

		data <<= 0x1;
	}
	mutex_unlock(&i2c_mutex);
	preempt_enable();
	local_irq_restore(flag);
	printk("i2c: write data OK\n");

	return 0;
}

int i2c_read_byte(char *data)
{
	unsigned long flag = 0;
	char ret = 0;
	char i = 0;

	local_irq_save(flag);
	preempt_disable();
	mutex_lock(&i2c_mutex);

	SET_SDA_INPUT;
	udelay(I2C_DELAY);

	for (i = 0; i < 8; i++) {
		SET_SCL_HIGH;
		udelay(I2C_DELAY);

		ret <<= 1;

		if (GET_SDA_VAL) {
			ret |= 0x01;
		}

		SET_SCL_LOW;
		udelay(I2C_DELAY);
	}

	mutex_unlock(&i2c_mutex);
	preempt_enable();
	local_irq_restore(flag);

	*data = ret;

	return 0;
}

