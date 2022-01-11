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
 ********************* MAIN **********************
 ************************************************/
int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	Enable_global_interrupt();

	led_init();

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
