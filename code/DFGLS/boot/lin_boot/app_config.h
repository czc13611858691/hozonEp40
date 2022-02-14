/*
 * app_config.h
 *
 * Created: 2021/10/27 16:12:10
 *  Author: JasonZhu
 */


#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#define SECURITY_SEED_LEN 4
#define FLASH_PAGE_SIZE 512

#define SESSION_DEFAULT     1U
#define SESSION_PROGRAM     2U
#define SESSION_EXTEND      3U

extern uint8_t security_seed[SECURITY_SEED_LEN];
extern int erase_flag;
extern int flash_flag;
extern int reset_flag;
extern uint32_t bin_size_cnt;
extern uint16_t update_cnt;
extern flash_adr_t page_addr;
extern uint8_t program_buffer[512];
extern uint8_t history_blcok_count;
extern uint8_t session_mode;
extern uint8_t security_checked;  //安全授权
extern uint8_t download_request_flag;
extern uint16_t crc16_code;
extern uint32_t check_program_size;
extern uint8_t  check_program_flag;
extern uint32_t update_mem_size;
extern uint32_t update_mem_addr;
extern uint8_t ecu_rst_flg;
extern uint8_t g_sessionStatus;
extern uint8_t program_check_flg;

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
