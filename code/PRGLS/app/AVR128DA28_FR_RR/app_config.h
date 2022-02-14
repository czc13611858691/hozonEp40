#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#pragma once

extern int erase_flag;
extern int flash_flag;
extern int jump_flag;
extern int bin_size_cnt;
extern uint16_t update_cnt;
extern flash_adr_t page_addr;
extern uint8_t program_buffer[512];
extern uint8_t history_blcok_count;
extern uint8_t ecu_rst_flg;
extern uint8_t program_request_flg;

typedef struct {
	uint8_t start;
	uint8_t len;
}eeprom_did_data_t;
extern const eeprom_did_data_t VINDataIdentifier_eeprom;
extern uint8_t gVINDataIdentifier[17];
extern uint8_t gVINDataIdentifier_update_flg;
extern const eeprom_did_data_t TesterSerialNumberDataIdentifier_eeprom;
extern uint8_t gTesterSerialNumberDataIdentifier[10];
extern uint8_t gTesterSerialNumberDataIdentifier_update_flg;
extern const eeprom_did_data_t ProgrammingDataDataIdentifier_eeprom;
extern uint8_t gProgrammingDataDataIdentifier[4];
extern uint8_t gProgrammingDataDataIdentifier_update_flg;
extern const eeprom_did_data_t ECUInstallationDateDataIdentifier_eeprom;
extern uint8_t gECUInstallationDateDataIdentifier[4];
extern uint8_t gECUInstallationDateDataIdentifier_update_flg;

#endif /* APP_CONFIG_H_ */