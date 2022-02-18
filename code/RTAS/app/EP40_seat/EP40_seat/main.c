#include <atmel_start.h>
#include "./usr/hc595d/hc595.h"
#include "lin1.h"
#include "lin_common_api.h"
#include "app_config.h"
#include "lin_commontl_api.h"
#include "./usr/os_t/soft_timer.h"
#include "rstctrl.h"
#include "ep40_function_specification.h"


#define PROGRAM_REQUEST_EEPROM_POS 0
#define APPLICATION_VALID_EEPROM_POS 1
#define PRAOGRAM_CHECK_EEPROM_POS 2

const eeprom_did_data_t VINDataIdentifier_eeprom = {
	.start = 10,
	.len = 17,
};

uint8_t gVINDataIdentifier[17] = { 0 };
uint8_t gVINDataIdentifier_update_flg = 0;

const eeprom_did_data_t TesterSerialNumberDataIdentifier_eeprom = {
	.start = 30,
	.len = 10,
};

uint8_t gTesterSerialNumberDataIdentifier[10] = { 0 };
uint8_t gTesterSerialNumberDataIdentifier_update_flg = 0;

const eeprom_did_data_t ProgrammingDataDataIdentifier_eeprom = {
	.start = 40,
	.len = 4,
};

uint8_t gProgrammingDataDataIdentifier[4] = { 0 };
uint8_t gProgrammingDataDataIdentifier_update_flg = 0;

const eeprom_did_data_t ECUInstallationDateDataIdentifier_eeprom = {
	.start = 50,
	.len = 4,
};

uint8_t gECUInstallationDateDataIdentifier[4] = { 0 };
uint8_t gECUInstallationDateDataIdentifier_update_flg = 0;

uint8_t program_check_flg = 0;
int erase_flag = 0;
int flash_flag = 0;
int jump_flag = 0;
uint16_t rb;

uint8_t program_buffer[512] = { 0 };
uint8_t history_blcok_count;
uint16_t update_cnt = 0;
int bin_size_cnt = 0;
flash_adr_t page_addr = 0x6400;
uint8_t ok;
uint8_t ecu_rst_flg = 0;
uint8_t program_request_flg = 0;

uint8_t led_data[2] = {0x00,0x00};
int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	l_sys_init();
	l_ifc_init(LI0);
	ld_init(LI0);
	Enable_global_interrupt();

	/* 从EEPROM中读取数据 */
	for (uint8_t i = 0;i < VINDataIdentifier_eeprom.len;i++)
	{
		gVINDataIdentifier[i] = FLASH_0_read_eeprom_byte(VINDataIdentifier_eeprom.start + i);
	}
	for (uint8_t i = 0;i < TesterSerialNumberDataIdentifier_eeprom.len;i++)
	{
		gTesterSerialNumberDataIdentifier[i] = FLASH_0_read_eeprom_byte(TesterSerialNumberDataIdentifier_eeprom.start + i);
	}
	for (uint8_t i = 0;i < ProgrammingDataDataIdentifier_eeprom.len;i++)
	{
		gProgrammingDataDataIdentifier[i] = FLASH_0_read_eeprom_byte(ProgrammingDataDataIdentifier_eeprom.start + i);
	}
	for (uint8_t i = 0;i < ECUInstallationDateDataIdentifier_eeprom.len;i++)
	{
		gECUInstallationDateDataIdentifier[i] = FLASH_0_read_eeprom_byte(ECUInstallationDateDataIdentifier_eeprom.start + i);
	}

	/* Replace with your application code */

	// test PWM IO control 
	// BAT_CTR1_set_level(1);
	// BAT_CTR2_set_level(1);
	// TCA0.SPLIT.HCMP1 = 29;
	TCA0.SPLIT.LCMP2 = 20;
	
	// test led backlight
	HC595_Write(led_data,2,16);
	//HC595_Write(led_data,1,8);
	soft_timer_create(20, btn_process_task);
	soft_timer_create(10, ep40_integrate_all_signal);
	soft_timer_create(100,backlight_task);
	while (1) {
		soft_timer_run();
		touch_process();
		if (program_check_flg == 1)
		{
			program_check_flg = 0;
			FLASH_0_write_eeprom_byte(PRAOGRAM_CHECK_EEPROM_POS, 0x01);
			while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm))
				;
		}

		if (ecu_rst_flg == 1)
		{
			/* 切换到编程会话的两个步骤，置位EEPROM编程请求变量，同时复位 */
			if (program_request_flg == 1)
			{
				FLASH_0_write_eeprom_byte(PROGRAM_REQUEST_EEPROM_POS, 1);
				while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm))
					;
			}

			if (ld_raw_tx_status(0) == LD_QUEUE_EMPTY)
			{
				if (LIN_DRV_GetCurrentNodeState(0) == LIN_NODE_STATE_IDLE)
				{
					RSTCTRL_reset();
				}
			}
		}

		if (gVINDataIdentifier_update_flg == 1)
		{
			gVINDataIdentifier_update_flg = 0;
			for (uint8_t i = 0;i < VINDataIdentifier_eeprom.len;i++)
			{
				FLASH_0_write_eeprom_byte(VINDataIdentifier_eeprom.start + i, gVINDataIdentifier[i]);
			}
		}

		if (gTesterSerialNumberDataIdentifier_update_flg == 1)
		{
			gTesterSerialNumberDataIdentifier_update_flg = 0;
			for (uint8_t i = 0;i < TesterSerialNumberDataIdentifier_eeprom.len;i++)
			{
				FLASH_0_write_eeprom_byte(TesterSerialNumberDataIdentifier_eeprom.start + i, gTesterSerialNumberDataIdentifier[i]);
			}
		}

		if (gProgrammingDataDataIdentifier_update_flg == 1)
		{
			gProgrammingDataDataIdentifier_update_flg = 0;

			for (uint8_t i = 0;i < ProgrammingDataDataIdentifier_eeprom.len;i++)
			{
				FLASH_0_write_eeprom_byte(ProgrammingDataDataIdentifier_eeprom.start + i, gProgrammingDataDataIdentifier[i]);
			}
		}

		if (gECUInstallationDateDataIdentifier_update_flg == 1)
		{
			gECUInstallationDateDataIdentifier_update_flg = 0;
			for (uint8_t i = 0;i < ECUInstallationDateDataIdentifier_eeprom.len;i++)
			{
				FLASH_0_write_eeprom_byte(ECUInstallationDateDataIdentifier_eeprom.start + i, gECUInstallationDateDataIdentifier[i]);
			}
		}
	}
}
