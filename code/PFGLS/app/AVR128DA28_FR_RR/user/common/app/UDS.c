#include "UDS.h"
#include "nvmctrl_basic.h"
#include "lin.h"
#include "lin_driver.h"
#include "rstctrl.h"
#include "target.h"
#include "lin_commontl_api.h"

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
uint32_t SECURITY_ACCESS_SEED = 0x11223344;
uint8_t g_calculate_key_flg = 0;
uint32_t diag_device_send_key = 0;

uint32_t canculate_security_access_bcm(uint32_t seed, uint32_t APP_MASK);

void UDS_read_DID_from_eeprom(void)
{
#if 0
    for (uint8_t i = 0;i < VINDataIdentifier_eeprom.len;i++)
    {
        gVINDataIdentifier[i] = FLASH_0_read_eeprom_byte(VINDataIdentifier_eeprom.start + i);
    }
#endif
    for (uint8_t i = 0;i < TesterSerialNumberDataIdentifier_eeprom.len;i++)
    {
        gTesterSerialNumberDataIdentifier[i] = FLASH_0_read_eeprom_byte(TesterSerialNumberDataIdentifier_eeprom.start + i);
    }
    for (uint8_t i = 0;i < ProgrammingDataDataIdentifier_eeprom.len;i++)
    {
        gProgrammingDataDataIdentifier[i] = FLASH_0_read_eeprom_byte(ProgrammingDataDataIdentifier_eeprom.start + i);
    }
#if 0
    for (uint8_t i = 0;i < ECUInstallationDateDataIdentifier_eeprom.len;i++)
    {
        gECUInstallationDateDataIdentifier[i] = FLASH_0_read_eeprom_byte(ECUInstallationDateDataIdentifier_eeprom.start + i);
    }
#endif
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

    if (g_calculate_key_flg == 1)
    {
        diag_device_send_key = canculate_security_access_bcm(SECURITY_ACCESS_SEED, APP_MASK_EXCEL_DEF);
    }
}

uint32_t canculate_security_access_bcm(uint32_t seed, uint32_t APP_MASK)
{
    uint32_t tmpseed = seed;
    uint32_t key_1 = tmpseed ^ APP_MASK;
    uint32_t seed_2 = tmpseed;
    seed_2 = (seed_2 & 0x55555555) << 1 ^ (seed_2 & 0xAAAAAAAA) >> 1;
    seed_2 = (seed_2 ^ 0x33333333) << 2 ^ (seed_2 ^ 0xCCCCCCCC) >> 2;
    seed_2 = (seed_2 & 0x0F0F0F0F) << 4 ^ (seed_2 & 0xF0F0F0F0) >> 4;
    seed_2 = (seed_2 ^ 0x00FF00FF) << 8 ^ (seed_2 ^ 0xFF00FF00) >> 8;
    seed_2 = (seed_2 & 0x0000FFFF) << 16 ^ (seed_2 & 0xFFFF0000) >> 16;
    uint32_t key_2 = seed_2;
    uint32_t key = key_1 + key_2;
    return key;
}