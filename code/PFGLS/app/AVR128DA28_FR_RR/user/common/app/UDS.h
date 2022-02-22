#pragma once

#include <stdint.h>

#define PROGRAM_REQUEST_EEPROM_POS 0
#define APPLICATION_VALID_EEPROM_POS 1
#define PRAOGRAM_CHECK_EEPROM_POS 2

typedef struct {
	uint8_t start;
	uint8_t len;
}eeprom_did_data_t;

extern uint8_t ecu_rst_flg;
extern uint8_t program_request_flg;
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

extern uint32_t SECURITY_ACCESS_SEED;
extern uint8_t g_calculate_key_flg;
extern uint32_t diag_device_send_key;

void UDS_read_DID_from_eeprom(void);
void UDS_flg_check_task(void);