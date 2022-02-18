#ifndef __hc595_H_
#define __hc595_H_

#include <stdint.h>

#define HC595_NUM (2)
#define HC595_OUT_PIN (8)

void HC595_Write(uint8_t *dataPtr, uint8_t icNum, uint8_t bitLen);

#endif