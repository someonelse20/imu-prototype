#include "hardware/adc.h"
#include "pico/stdlib.h"
#include <pico/time.h>
#include <stdio.h>

#include "lsm9ds1.h"

float get_bat_volt() {
	const float conversion_factor = (3.3f / (1 << 12)) * 1.3125;
	uint16_t result = adc_read();
	// printf("Raw value: 0x%03x, Battery voltage: %f V\n", result, result * conversion_factor);
	return result * conversion_factor;
}

int main() {
	lsm9ds1_t imu;
	imu.ADDR = 0x6B;
	imu.MAG_ADDR= 0x1E;

	setup_default_uart();
	stdio_init_all();

	adc_init();

	adc_gpio_init(29);
	adc_select_input(3);

	sleep_ms(10);
	uint8_t val = lsm_init(&imu);
	sleep_ms(10);

	while (1) {
		float bat_volt = get_bat_volt();

		float gyro[3];
		float accel[3];
		float mag[3];

		read_gyro(&imu, gyro);
		read_accel(&imu, accel);
		read_mag(&imu, mag);

		printf("mag: ");
		for (int i = 0; i < 3; i++) {
			printf("%f, ", mag[i]);
		}
		printf("\n");
		/*
		printf("%i\n", val);
		*/

		sleep_ms(100);
	}
}
