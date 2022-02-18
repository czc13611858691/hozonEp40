#include "UDS.h"
#include "nvmctrl_basic.h"
#include "lin.h"
#include "lin_driver.h"
#include "rstctrl.h"

/* DID需要写入EEPROM的变量 */
const eeprom_did_data_t VINDataIdentifier_eeprom = {
    .start = 10,
    .len = 17,
};
const eeprom_did_data_t TesterSerialNumberDataIdentifier_eeprom = {
    .start = 30,
    .len = 10,
};
const eeprom_did_data_t ProgrammingDataDataIdentifier_eeprom = {
    .start = 40,
    .len = 4,
};
const eeprom_did_data_t ECUInstallationDateDataIdentifier_eeprom = {
    .start = 50,
    .len = 4,
};
uint8_t gVINDataIdentifier[17] = { 0 };
uint8_t gVINDataIdentifier_update_flg = 0;
uint8_t gTesterSerialNumberDataIdentifier[10] = { 0 };
uint8_t gTesterSerialNumberDataIdentifier_update_flg = 0;
uint8_t gProgrammingDataDataIdentifier[4] = { 0 };
uint8_t gProgrammingDataDataIdentifier_update_flg = 0;
uint8_t gECUInstallationDateDataIdentifier[4] = { 0 };
uint8_t gECUInstallationDateDataIdentifier_update_flg = 0;

uint8_t program_check_flg = 0;
uint8_t ecu_rst_flg = 0;
uint8_t program_request_flg = 0;

void UDS_read_DID_from_eeprom(void)
{
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
}

void UDS_flg_check_task(void)
{
    if (program_check_flg == 1)
    {
        program_check_flg = 0;
        FLASH_0_write_eeprom_byte(PRAOGRAM_CHECK_EEPROM_POS, 0x01);
        while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm))
            ;
    }

    if (ecu_rst_flg == 1)
    {
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