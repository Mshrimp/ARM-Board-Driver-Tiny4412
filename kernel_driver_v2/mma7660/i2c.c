#include <linux/mutex.h>
#include <linux/delay.h>

#define	I2C_DELAY			10

#define	NV_GPIO_SCL			0
#define	NV_GPIO_SDA			1

#define	NV_SCL_SET_OUT		nv_i2c_info.chip_func->gpio->nv_gpio_conf_out(&nv_i2c_info.scl)
#define	NV_SDA_SET_OUT		nv_i2c_info.chip_func->gpio->nv_gpio_conf_out(&nv_i2c_info.sda)
#define	NV_SDA_SET_IN		nv_i2c_info.chip_func->gpio->nv_gpio_conf_out(&nv_i2c_info.sda)

#define	NV_SCL_SET_HIGH		nv_i2c_info.chip_func->gpio->nv_gpio_set_val(&nv_i2c_info.scl, 1)
#define	NV_SCL_SET_LOW		nv_i2c_info.chip_func->gpio->nv_gpio_set_val(&nv_i2c_info.scl, 0)

#define	NV_SDA_SET_HIGH		nv_i2c_info.chip_func->gpio->nv_gpio_set_val(&nv_i2c_info.sda, 1)
#define	NV_SDA_SET_LOW		nv_i2c_info.chip_func->gpio->nv_gpio_set_val(&nv_i2c_info.sda, 0)

#define	NV_SCL_GET_VAL		nv_i2c_info.chip_func->gpio->nv_gpio_get_val(&nv_i2c_info.scl)
#define	NV_SDA_GET_VAL		nv_i2c_info.chip_func->gpio->nv_gpio_get_val(&nv_i2c_info.sda)



typedef struct {
	nv_gpio_t scl;
	nv_gpio_t sda;
	const nv_chip_t *chip_func;
	spinlock_t lock;
	struct mutex i2c_mutex;
} nv_i2c_info_t;

static nv_i2c_info_t nv_i2c_info = {
	.scl = {
	},
	.sda = {
	},
	.chip_func = NULL,
};

static void nv_i2c_scl_set_high(void)
{
	int i = 0;

	NV_SCL_SET_HIGH;
	for (i = 0; i < 10; i++) {
		if (!NV_SCL_GET_VAL) {
			NV_SCL_SET_HIGH;
			udelay(I2C_DELAY);
		} else {
			break;
		}
	}

	if (i == 10) {
		nv_error("i2c: scl busy, set scl high failed");
	}
}

static void nv_i2c_scl_init(void)
{
	nv_debug("i2c: scl init");
	NV_SCL_SET_OUT;
	nv_i2c_scl_set_high();
}

static void nv_i2c_sda_init(void)
{
	nv_debug("i2c: sda init");
	NV_SDA_SET_OUT;
	NV_SDA_SET_HIGH;
}

static void nv_chip_i2c_gpio_init(void)
{
	nv_debug("nv_chip_i2c_gpio_init");
	nv_i2c_sda_init();
	nv_i2c_scl_init();
}


static int nv_chip_i2c_init(nv_i2c_gpio_t *pin)
{
	nv_debug("nv_chip_i2c_init");
	nv_i2c_info.chip_func = nv_chip();
	if (!nv_i2c_info.chip_func) {
		nv_error("get chip function failed");
		return -1;
	}
	spin_lock_init(&nv_i2c_info.lock);
	mutex_init(&nv_i2c_info.i2c_mutex);

	spin_lock(&nv_i2c_info.lock);
	nv_i2c_info.scl.group = pin->scl.group;
	nv_i2c_info.scl.number = pin->scl.number;
	nv_i2c_info.sda.group = pin->sda.group;
	nv_i2c_info.sda.number = pin->sda.number;
	spin_unlock(&nv_i2c_info.lock);

	nv_chip_i2c_gpio_init();

	return 0;
}

static int nv_chip_i2c_start(void)
{
	nv_debug("nv_chip_i2c_start");

	mutex_lock(&nv_i2c_info.i2c_mutex);
	NV_SDA_SET_HIGH;
	udelay(I2C_DELAY);

	nv_i2c_scl_set_high();
	udelay(I2C_DELAY);

	NV_SDA_SET_LOW;
	udelay(I2C_DELAY);

	NV_SCL_SET_LOW;
	udelay(I2C_DELAY);
	mutex_unlock(&nv_i2c_info.i2c_mutex);

	return 0;
}

static int nv_chip_i2c_stop(void)
{
	nv_debug("nv_chip_i2c_stop");

	mutex_lock(&nv_i2c_info.i2c_mutex);
	NV_SCL_SET_LOW;
	udelay(I2C_DELAY);

	NV_SDA_SET_LOW;
	udelay(I2C_DELAY);

	nv_i2c_scl_set_high();
	udelay(I2C_DELAY);

	NV_SDA_SET_HIGH;
	udelay(I2C_DELAY);
	mutex_unlock(&nv_i2c_info.i2c_mutex);

	return 0;
}

static int nv_chip_i2c_send_ack(void)
{
	nv_debug("nv_chip_i2c_send_ack");

	mutex_lock(&nv_i2c_info.i2c_mutex);
	NV_SCL_SET_LOW;
	udelay(I2C_DELAY);

	NV_SDA_SET_LOW;
	udelay(I2C_DELAY);

	nv_i2c_scl_set_high();
	udelay(I2C_DELAY);

	NV_SCL_SET_LOW;
	udelay(I2C_DELAY);
	mutex_unlock(&nv_i2c_info.i2c_mutex);

	return 0;
}

static int nv_chip_i2c_send_noack(void)
{
	nv_debug("nv_chip_i2c_send_noack");

	mutex_lock(&nv_i2c_info.i2c_mutex);
	NV_SCL_SET_LOW;
	udelay(I2C_DELAY);

	NV_SDA_SET_HIGH;
	udelay(I2C_DELAY);

	nv_i2c_scl_set_high();
	udelay(I2C_DELAY);

	NV_SCL_SET_LOW;
	udelay(I2C_DELAY);
	mutex_unlock(&nv_i2c_info.i2c_mutex);

	return 0;
}

static int nv_chip_i2c_wait_ack(void)
{
	int ack_times = 0;
	int ret = 0;

	mutex_lock(&nv_i2c_info.i2c_mutex);
	NV_SDA_SET_IN;
	udelay(I2C_DELAY);

	NV_SCL_SET_LOW;
	udelay(I2C_DELAY);

	nv_i2c_scl_set_high();
	udelay(I2C_DELAY);

	ack_times = 0;
	while (NV_SDA_GET_VAL) {
		ack_times++;
		if (ack_times == 10) {
			ret = 1;
			break;
		}
	}

	NV_SCL_SET_LOW;
	mutex_unlock(&nv_i2c_info.i2c_mutex);

	return 0;
}

static int nv_chip_i2c_ack(u8 ack_status)
{
	int ret = -1;

	switch (ack_status) {
	case NV_I2C_WAIT_ACK:
		ret = nv_chip_i2c_wait_ack();
		break;
	case NV_I2C_ACK:
		ret = nv_chip_i2c_send_ack();
		break;
	case NV_I2C_NOACK:
		ret = nv_chip_i2c_send_noack();
		break;
	default:
		nv_error("i2c: ack status error");
		return -EINVAL;
	}

	if (ret) {
		nv_error("i2c: ack error");
	}

	return 0;
}

static int nv_chip_i2c_write_byte(u8 data)
{
	unsigned long flag = 0;
	u8 i = 0;

	local_irq_save(flag);
	preempt_disable();
	mutex_lock(&nv_i2c_info.i2c_mutex);
	NV_SDA_SET_OUT;
	udelay(I2C_DELAY);

	for (i = 0; i < 8; i++) {
		if (data & 0x80) {
			NV_SDA_SET_HIGH;
		} else {
			NV_SDA_SET_LOW;
		}
		udelay(I2C_DELAY);

		nv_i2c_scl_set_high();
		udelay(I2C_DELAY);

		NV_SCL_SET_LOW;
		udelay(I2C_DELAY);

		data <<= 0x1;
	}
	mutex_unlock(&nv_i2c_info.i2c_mutex);
	preempt_enable();
	local_irq_restore(flag);

	return 0;
}

static int nv_chip_i2c_read_byte(u8 *data)
{
	unsigned long flag = 0;
	u8 ret = 0;
	u8 i = 0;

	local_irq_save(flag);
	preempt_disable();
	mutex_lock(&nv_i2c_info.i2c_mutex);

	NV_SDA_SET_IN;
	udelay(I2C_DELAY);

	for (i = 0; i < 8; i++) {
		nv_i2c_scl_set_high();
		udelay(I2C_DELAY);

		ret <<= 1;

		if (NV_SDA_GET_VAL) {
			ret |= 0x01;
		}

		NV_SCL_SET_LOW;
		udelay(I2C_DELAY);
	}

	mutex_unlock(&nv_i2c_info.i2c_mutex);
	preempt_enable();
	local_irq_restore(flag);

	*data = ret;

	return 0;
}

nv_i2c_ops_t i2c_ops = {
	.nv_i2c_init = nv_chip_i2c_init,
	.nv_i2c_start = nv_chip_i2c_start,
	.nv_i2c_stop = nv_chip_i2c_stop,
	.nv_i2c_ack = nv_chip_i2c_ack,
	.nv_i2c_write_byte = nv_chip_i2c_write_byte,
	.nv_i2c_read_byte = nv_chip_i2c_read_byte,
};
