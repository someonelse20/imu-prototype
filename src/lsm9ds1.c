#include "pico/binary_info.h"
#include <hardware/i2c.h>
#include <hardware/gpio.h>
#include <stdint.h>
#include <stdio.h>

#include "lsm9ds1.h"
#include "lsm9ds1_reg.h"

static float convert_gyro(uint16_t gyro_raw);
static float convert_accel(uint16_t accel_raw);
static float convert_mag(uint16_t mag_raw);

uint8_t lsm_init(lsm9ds1_t *lsm9ds1) {
	uint8_t buf[2];

	i2c_init(i2c_default, 100 * 1000);

	gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
	gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
	gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
	gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
	bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

	// Set config.

	// Set gyro to 476Hz and 500dps scale.
	buf[0] = CTRL_REG1_G;
	buf[1] = 0b10101011;
	i2c_write_blocking(i2c_default, lsm9ds1->ADDR, buf, 2, false);

	// Set accel to 476Hz and 8g scale.
	buf[0] = CTRL_REG6_XL;
	buf[1] = 0b10111001;
	i2c_write_blocking(i2c_default, lsm9ds1->ADDR, buf, 2, false);

	// Set mag to high performance mode and 80Hz.
	buf[0] = CTRL_REG1_M;
	buf[1] = 0b01011100;
	i2c_write_blocking(i2c_default, lsm9ds1->ADDR, buf, 2, false);

	// Set mag to 12 gauss scale.
	buf[0] = CTRL_REG2_M;
	buf[1] = 0b0100000;
	i2c_write_blocking(i2c_default, lsm9ds1->ADDR, buf, 2, false);

	uint8_t reg = WHO_AM_I_XG;
	uint8_t who_am_i;

	i2c_write_blocking(i2c_default, lsm9ds1->ADDR, &reg, 1, true);
	i2c_read_blocking(i2c_default, lsm9ds1->ADDR, &who_am_i, 1, false);

	printf("%i\n", who_am_i);

	return 0;
}

void read_gyro(lsm9ds1_t *lsm9ds1, float *gyro) {
	int16_t gyro_raw[3];
	uint8_t buf[6];
	uint8_t reg = OUT_X_L_G;

	i2c_write_blocking(i2c_default, lsm9ds1->ADDR, &reg, 1, true);
	i2c_read_blocking(i2c_default, lsm9ds1->ADDR, buf, 6, false);

	gyro_raw[0] = (int16_t)buf[1];
	gyro_raw[0] = (gyro_raw[0] * 256) + (int16_t)buf[0];
	gyro_raw[1] = (int16_t)buf[3];
	gyro_raw[1] = (gyro_raw[1] * 256) + (int16_t)buf[2];
	gyro_raw[2] = (int16_t)buf[5];
	gyro_raw[2] = (gyro_raw[2] * 256) + (int16_t)buf[4];

	for (int i = 0; i < 3; i++) {
		gyro[i] = gyro_raw[i] * (17.5 / 1000);
	}
}

void read_accel(lsm9ds1_t *lsm9ds1, float *accel) {
	int16_t accel_raw[3];
	uint8_t buf[6];
	uint8_t reg = OUT_X_L_XL;

	i2c_write_blocking(i2c_default, lsm9ds1->ADDR, &reg, 1, true);
	i2c_read_blocking(i2c_default, lsm9ds1->ADDR, buf, 6, false);

	accel_raw[0] = (int16_t)buf[1];
	accel_raw[0] = (accel_raw[0] * 256) + (int16_t)buf[0];
	accel_raw[1] = (int16_t)buf[3];
	accel_raw[1] = (accel_raw[1] * 256) + (int16_t)buf[2];
	accel_raw[2] = (int16_t)buf[5];
	accel_raw[2] = (accel_raw[2] * 256) + (int16_t)buf[4];

	for (int i = 0; i < 3; i++) {
		accel[i] = accel_raw[i] * (0.244 / 1000);
	}
}
void read_mag(lsm9ds1_t *lsm9ds1, float *mag) {
	int16_t mag_raw[3];
	uint8_t buf[6];
	uint8_t reg = OUT_X_L_M;

	i2c_write_blocking(i2c_default, lsm9ds1->ADDR, &reg, 1, true);
	i2c_read_blocking(i2c_default, lsm9ds1->ADDR, buf, 6, false);

	mag_raw[0] = (int16_t)buf[1];
	mag_raw[0] = (mag_raw[0] * 256) + (int16_t)buf[0];
	mag_raw[1] = (int16_t)buf[3];
	mag_raw[1] = (mag_raw[1] * 256) + (int16_t)buf[2];
	mag_raw[2] = (int16_t)buf[5];
	mag_raw[2] = (mag_raw[2] * 256) + (int16_t)buf[4];

	for (int i = 0; i < 3; i++) {
		mag[i] = mag_raw[i] * (0.43 / 1000);
	}
}

