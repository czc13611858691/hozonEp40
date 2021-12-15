#include <atmel_start.h>
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
#include "rstctrl.h"
#include "nvmctrl_basic.h"

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
	//printf(":%d,%d\r\n", adc_res[0], adc_res[1]);
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

/************************************************
 ********************* TOUCH **********************
 ************************************************/
void soft_timer_touch_printf_task(void)
{
	int16_t temp_int_calc;
	uint8_t u8temp_output;
	volatile uint16_t count_bytes_out;
	//printf("delta:");
	for (count_bytes_out = 0u; count_bytes_out < DEF_NUM_CHANNELS; count_bytes_out++)
	{
		temp_int_calc = get_sensor_node_signal(count_bytes_out);
		temp_int_calc -= get_sensor_node_reference(count_bytes_out);
		//printf("%d,", temp_int_calc);
	}
	//printf("\r\n");
	//printf("state:");
	for (count_bytes_out = 0u; count_bytes_out < DEF_NUM_CHANNELS; count_bytes_out++)
	{
		/* State */
		u8temp_output = get_sensor_state(count_bytes_out) & 0x80;
		//printf("%d,", u8temp_output);
	}
	//printf("\r\n");
}

extern qtm_scroller_control_t qtm_scroller_control1;
void soft_timer_slide_printf_task(void)
{
	uint8_t u8temp_output;
	volatile uint16_t count_bytes_out;
	uint16_t u16temp_output;
	//printf("state:");
	for (count_bytes_out = 0u; count_bytes_out < qtm_scroller_control1.qtm_scroller_group_config->num_scrollers;
		 count_bytes_out++)
	{

		/* Slider State */
		u8temp_output = qtm_scroller_control1.qtm_scroller_data[count_bytes_out].scroller_status;
		//printf("%d,", u8temp_output);
	}
	//printf("\r\n");
	//printf("pos:");
	for (count_bytes_out = 0u; count_bytes_out < qtm_scroller_control1.qtm_scroller_group_config->num_scrollers;
		 count_bytes_out++)
	{

		/* filtered position */
		u16temp_output = qtm_scroller_control1.qtm_scroller_data[count_bytes_out].position;
		//printf("%d,", u16temp_output);
	}
	//printf("\r\n");
}

void soft_timer_touch_led_task(void)
{
	if ((get_sensor_state(0) & 0x80) != 0)
	{
		led_set_level(led_d6, true);
	}
	else
	{
		led_set_level(led_d6, false);
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
		led_set_level(led_d18, true);
	}
	else
	{
		led_set_level(led_d18, false);
	}
	if ((get_sensor_state(3) & 0x80) != 0)
	{
		led_set_level(led_d8, true);
	}
	else
	{
		led_set_level(led_d8, false);
	}
	if ((get_sensor_state(4) & 0x80) != 0)
	{
		led_set_level(led_d10, true);
	}
	else
	{
		led_set_level(led_d10, false);
	}
	if ((get_sensor_state(5) & 0x80) != 0)
	{
		led_set_level(led_d19, true);
	}
	else
	{
		led_set_level(led_d19, false);
	}

	uint8_t slide_index = 1;
	uint8_t slide_pos = 0;

	if (get_scroller_state(slide_index) != 0)
	{
		slide_pos = 255 - get_scroller_position(slide_index);
		led_set_level(led_d14, true);
		if (slide_pos > 50)
		{
			led_set_level(led_d12, true);
		}
		else
		{
			led_set_level(led_d12, false);
		}

		if (slide_pos > 100)
		{
			led_set_level(led_d11, true);
		}
		else
		{
			led_set_level(led_d11, false);
		}

		if (slide_pos > 150)
		{
			led_set_level(led_d17, true);
		}
		else
		{
			led_set_level(led_d17, false);
		}
	}
	else
	{
		led_set_level(led_d14, false);
		led_set_level(led_d12, false);
		led_set_level(led_d11, false);
		led_set_level(led_d17, false);
	}

	slide_index = 0;

	if (get_scroller_state(slide_index) != 0)
	{
		slide_pos = 255 - get_scroller_position(slide_index);
		led_set_level(led_d15, true);
		if (slide_pos > 50)
		{
			led_set_level(led_d13, true);
		}
		else
		{
			led_set_level(led_d13, false);
		}

		if (slide_pos > 100)
		{
			led_set_level(led_d4, true);
		}
		else
		{
			led_set_level(led_d4, false);
		}

		if (slide_pos > 150)
		{
			led_set_level(led_d16, true);
		}
		else
		{
			led_set_level(led_d16, false);
		}
	}
	else
	{
		led_set_level(led_d15, false);
		led_set_level(led_d13, false);
		led_set_level(led_d4, false);
		led_set_level(led_d16, false);
	}
}

/************************************************
 ********************* LIN **********************
 ************************************************/
void soft_timer_lin_printf_task(void)
{
	// printf(":1");

	/* 0x10帧整体读的数据打印 */
	// for(uint8_t i=0;i<8;i++)
	// {
	// 	printf("0x%0x,",g_lin_frame[0].data[i]);
	// }

	/* 信号读写api测试 */
	// uint32_t backlight_status = 0;
	// uint32_t backlight_brightness = 0;
	// uint32_t ctrl_window_lock_sw_ind = 0;

	// backlight_status = lin_signal_rd(SIGNAL_BACKLIGHT_STATUS);
	// backlight_brightness = lin_signal_rd(SIGNAL_BACKLIGHT_BRIGHTNESS);
	// ctrl_window_lock_sw_ind = lin_signal_rd(SIGNAL_CTRL_WINDOW_LOCK_SW_IND);

	// printf("%d,%d,%d", (int)backlight_status, (int)backlight_brightness, (int)ctrl_window_lock_sw_ind);

	// printf("\r\n");
#if 0
	uint8_t signal1 = 0;
	uint8_t signal2 = 0;
	uint8_t signal3 = 0;

	signal1 = lin_signal_rd(DDCU_0X10_BACKLIGHT_BRIGHTNESS_FB);
	signal2 = lin_signal_rd(DDCU_0X10_CONTROL_WINDOW_LOCK_SW_LND);
	signal3 = lin_signal_rd(DDCU_0X10_BACKLIGHT_STATUS);

	g_lin_frame[1].data[4] = signal1;
	g_lin_frame[1].data[5] = signal2;
	g_lin_frame[1].data[6] = signal3;
#endif
}

/************************************************
 ********************* MAIN **********************
 ************************************************/
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

	/* 100ms 测试电机震动波形 */
	// soft_timer_create(100, sof_timer_motor_test_task);

	/* 100ms 触摸节点测试，触摸滑条测试 */
	// soft_timer_create(100, soft_timer_touch_printf_task);
	// soft_timer_create(100, soft_timer_slide_printf_task);

	/* 100ms lin测试任务 */
	// soft_timer_create(1000, soft_timer_lin_printf_task);

	//lin_signal_x_shot_wr(SIGNAL_FL_WINDOW_CTRL,1,2);

	// soft_timer_create(50, soft_timer_touch_led_task);

	/* REAR按键软件定时器任务 */
	soft_timer_create(20, soft_timer_rear_btn_task);

	/* WINDOW LOCK按键软件定时器任务 */
	soft_timer_create(20, soft_timer_window_lock_task);

	/* WINDOW 提升软件定时器任务 */
	soft_timer_create(20, soft_timer_window_task);

	/* led灯光更新任务 */
	soft_timer_create(50, led_update_buf_task);

	/* lin 接收信号变量更新任务 */
	soft_timer_create(10, soft_timer_lin_signal_update_task);

	/* 背光亮度控制更新任务 */
	soft_timer_create(100, backlight_task);

	l_sys_init();
	l_ifc_init(LI0);
	ld_init(LI0);

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
