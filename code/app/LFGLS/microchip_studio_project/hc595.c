#include "hc595.h"

#include "atmel_start.h"

#define HC595_DS_SET LED_OUT_DAT_set_level(true)
#define HC595_DS_RST LED_OUT_DAT_set_level(false)
#define HC595_STCP_SET LED_CS_set_level(true)
#define HC595_STCP_RST LED_CS_set_level(false)
#define HC595_SHCP_SET LED_MCLK_set_level(true)
#define HC595_SHCP_RST LED_MCLK_set_level(false)

void HC595_Write(uint8_t *dataPtr, uint8_t icNum, uint8_t bitLen)
{
    uint8_t _dataArr[HC595_NUM] = {0};
    uint8_t arrIdx = 0;
    uint8_t bitIdx = 0;
    uint8_t bitVal = 0;
    for (uint8_t i = 0; i < icNum; i++)
    {
        _dataArr[i] = dataPtr[icNum - 1 - i];
    }

    for (uint8_t i = 0; i < bitLen; i++)
    {
        arrIdx = i / 8;
        bitIdx = i % 8;
        bitVal = _dataArr[arrIdx] & (0x01 << (7 - bitIdx));

        if (bitVal != 0)
        {
            HC595_DS_SET;
        }
        else
        {
            HC595_DS_RST;
        }
        HC595_SHCP_RST;
        HC595_SHCP_SET;
    }
    HC595_STCP_RST;
    HC595_STCP_SET;
}
