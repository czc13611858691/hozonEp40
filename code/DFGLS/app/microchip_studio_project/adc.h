#ifndef __adc_H_ 
#define __adc_H_

#include <stdint.h>
#include <atmel_start.h>

adc_0_channel_t adc_channel;
adc_result_t ADC_0_measurement[2];
void adc_init(void);

#endif