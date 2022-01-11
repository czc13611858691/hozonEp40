#include "soft_timer.h"
#include "lin_driver.h"
#include "lin1.h"
#include "lin_common_api.h"
#include "app_config.h"
#include "ccp.h"
#include "lin_commontl_api.h"

#include "adc.h"
#include "ep40_cfg.h"
#include "ep40_function_specification.h"
#include "led.h"
#include "touch_press_motor_cfg.h"
#include "rstctrl.h"

uint8_t eeprom_set_boot_flg = 0;
int erase_flag = 0;
int flash_flag = 0;
int jump_flag = 0;
uint16_t rb;

uint8_t program_buffer[512] = {0};
uint8_t history_blcok_count;
uint16_t update_cnt = 0;
int bin_size_cnt = 0;
;
flash_adr_t page_addr = 0x4800;
uint8_t ok;


/************************************************
 ********************* ADC **********************
 ************************************************/
int adc_res[2] = {0};

void soft_timer_adc_task(void)
{
	adc_res[0] = ADC_0_measurement[0] >> 4;
	adc_res[1] = ADC_0_measurement[1] >> 4;
	ADC_0_start_conversion(adc_channel);
	// printf(":%d,%d\r\n", adc_res[0], adc_res[1]);
}

/************************************************
 ********************* DAC **********************
 ************************************************/
extern uint8_t sin_data_out_flg;
extern uint16_t sin_data[53];

void sof_timer_motor_test_task(void)
{
	sin_data_out_flg = 1;
}

void soft_timer_touch_led_task(void)
{
	if ((get_sensor_state(0) & 0x80) != 0)
	{
		led_set_level(led_d4, true);
	}
	else
	{
		led_set_level(led_d4, false);
	}
	if ((get_sensor_state(1) & 0x80) != 0)
	{
		led_set_level(led_d9, true);
	}
	else
	{
		led_set_level(led_d9, false);
	}
	if ((get_sensor_state(2) & 0x80) != 0)
	{
		led_set_level(led_d10, true);
		led_set_level(led_d11, false);
	}
	else
	{
		led_set_level(led_d11, true);
		led_set_level(led_d10, false);
	}

	uint8_t slide_index = 0;
	uint8_t slide_pos = 0;

	if (get_scroller_state(slide_index) != 0)
	{
		slide_pos = get_scroller_position(slide_index);
		led_set_level(led_d8, true);
		if (slide_pos > 50)
		{
			led_set_level(led_d7, true);
		}
		else
		{
			led_set_level(led_d7, false);
		}

		if (slide_pos > 100)
		{
			led_set_level(led_d6, true);
		}
		else
		{
			led_set_level(led_d6, false);
		}

		if (slide_pos > 150)
		{
			led_set_level(led_d5, true);
		}
		else
		{
			led_set_level(led_d5, false);
		}
	}
	else
	{
		led_set_level(led_d8, false);
		led_set_level(led_d7, false);
		led_set_level(led_d6, false);
		led_set_level(led_d5, false);
	}
}

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	Enable_global_interrupt();

	// adc_init();
	led_init();

	/* 10ms-adc采样任务 */
	// soft_timer_create(10, soft_timer_adc_task);

	/* 初始化电机:设置dac初始化电平，使能tpa驱动芯片 */
	// DAC_0_set_output(sin_data[0]);
	// MOTOR_GATE_set_level(true);

	/* led灯光更新任务 */
	soft_timer_create(100, led_update_buf_task);

	// soft_timer_create(50, soft_timer_touch_led_task);

	// soft_timer_create(20, soft_timer_touch_press_motor_task);

	/* WINDOW LOCK按键软件定时器任务 */
	soft_timer_create(20, soft_timer_window_lock_task);

	/* WINDOW 提升软件定时器任务 */
	soft_timer_create(20, soft_timer_window_task);

	/* lin 接收信号变量更新任务 */
	soft_timer_create(10, soft_timer_lin_signal_update_task);

	/* 背光亮度控制更新任务 */
	soft_timer_create(100, backlight_task);

	l_sys_init();
	l_ifc_init(LI0);
	ld_init(LI0);

	// FLASH_0_write_eeprom_byte(0x01, 0x60);

	/* Replace with your application code */
	while (1)
	{
		soft_timer_run();
		touch_process();
		if (eeprom_set_boot_flg == 1)
		{
			eeprom_set_boot_flg = 0;
			FLASH_0_write_eeprom_byte(0x00, 0xaa);
			while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm))
				;
			RSTCTRL_reset();
		}
	}
}
