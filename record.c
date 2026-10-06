#include "hardware/adc.h"
#include "kin_math.h"
#include "kin_types.h"
#include "pico/stdlib.h"
#include <pico/time.h>
#include <pico/types.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "lsm9ds1.h"
#include "ssd1306.h"
#include "kin_imu.h"

void render_imu(struct render_area *frame_area, float *val);

float get_bat_volt();

char* concat(const char *s1, const char *s2);

const float TARGET_DT_S = 0.11;

int main() {
	// Init IMU type.
	lsm9ds1_t lsm9ds1;
	lsm9ds1.ADDR = 0x6B;
	lsm9ds1.MAG_ADDR= 0x1E;

	setup_default_uart();
	stdio_init_all();

	// Init ADC.
	adc_init();

	adc_gpio_init(29);
	adc_select_input(3);

	// Init IMU.
	sleep_ms(10);
	uint8_t val = lsm_init(&lsm9ds1);
	sleep_ms(10);

	// Init OLED.
	SSD1306_init();

	// Initialize render area for entire frame (SSD1306_WIDTH pixels by SSD1306_NUM_PAGES pages)
	struct render_area frame_area = {
		start_col : 0,
		end_col : SSD1306_WIDTH - 1,
		start_page : 0,
		end_page : SSD1306_NUM_PAGES - 1
	};

	calc_render_area_buflen(&frame_area);

	// zero the entire display
	uint8_t buf[SSD1306_BUF_LEN];
	memset(buf, 0, SSD1306_BUF_LEN);
	render(buf, &frame_area);

	// Init kinetic.
	imu_t imu;

	while (1) {
		absolute_time_t timestamp = get_absolute_time();
		float bat_volt = get_bat_volt();

		float gyro[3];
		float accel[3];
		float mag[3];

		read_gyro(&lsm9ds1, gyro);
		read_accel(&lsm9ds1, accel);
		read_mag(&lsm9ds1, mag);

		printf("%f", absolute_time_diff_us(timestamp, get_absolute_time()) / 1000.f);
		printf("%f,%f,%f,", gyro[0], gyro[1], gyro[2]);
		printf("%f,%f,%f,", accel[0], accel[1], accel[2]);
		printf("%f,%f,%f\n", mag[0], mag[1], mag[2]);

		render_imu(&frame_area, accel);


		// Calculate timestamp.
		uint64_t duration_us = absolute_time_diff_us(timestamp, get_absolute_time());

		// printf("%lld\n", duration_us);

		int sleep_time = (TARGET_DT_S * 1000000) - duration_us;
		// printf("%i\n", sleep_time);
		sleep_us(sleep_time);

		/*
		   sleep_ms(10);
		   printf("%f\n", (double)absolute_time_diff_us(timestamp, get_absolute_time()) / 1000000);

		   absolute_time_t test_timestamp = get_absolute_time();
		   printf("%f\n", (double)absolute_time_diff_us(test_timestamp, get_absolute_time()) / 1000000);
		 */
	}
}

void render_imu(struct render_area *frame_area, float *val) {
	uint8_t buf[SSD1306_BUF_LEN];
	memset(buf, 0, SSD1306_BUF_LEN);

	char val_x_str[20];
	char val_y_str[20];
	char val_z_str[20];
	snprintf(val_x_str, 20, "%f", val[0]);
	snprintf(val_y_str, 20, "%f", val[1]);
	snprintf(val_z_str, 20, "%f", val[2]);

	char *text[] = {
		"x ",
		"y ",
		"z ",
	};

	text[0] = concat(text[0], val_x_str);
	text[1] = concat(text[1], val_y_str);
	text[2] = concat(text[2], val_z_str);

	// printf("%s\n ", text[0]);

	int y = 0;
	for (uint i = 0 ; i < count_of(text); i++) {
		WriteString(buf, SSD1306_WIDTH / 8, y, text[i]);
		y+=8;
	}
	render(buf, frame_area);

	free(text[0]);
	free(text[1]);
	free(text[2]);
}

float get_bat_volt() {
	const float conversion_factor = (3.3f / (1 << 12)) * 1.3125;
	uint16_t result = adc_read();
	// printf("Raw value: 0x%03x, Battery voltage: %f V\n", result, result * conversion_factor);
	return result * conversion_factor;
}


char* concat(const char *s1, const char *s2) {
	char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
	// in real code you would check for errors in malloc here
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

