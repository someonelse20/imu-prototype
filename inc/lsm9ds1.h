#ifndef LSM9DS1_H
#define LSM9DS1_H

#include <stdint.h>

typedef struct {
	uint8_t ADDR;
	uint8_t MAG_ADDR;
} lsm9ds1_t;

uint8_t lsm_init(lsm9ds1_t *lsm9ds1);
void read_gyro(lsm9ds1_t *lsm9ds1, float *gyro);
void read_accel(lsm9ds1_t *lsm9ds1, float *accel);
void read_mag(lsm9ds1_t *lsm9ds1, float *mag);

#endif
