#include "adc.h"
#include "target.h"

adc_result_t ADC_0_measurement[2];

adc_0_channel_t adc_channel = ADC_RES_0_CHANNEL;

void ADC_0_adc_handler_cb_copy(void)
{
    if (adc_channel == ADC_RES_0_CHANNEL)
    {
        adc_channel = ADC_RES_1_CHANNEL;
        ADC_0_measurement[0] = ADC_0_get_conversion_result();
    }
    else if (adc_channel == ADC_RES_1_CHANNEL)
    {
        adc_channel = ADC_RES_0_CHANNEL;
        ADC_0_measurement[1] = ADC_0_get_conversion_result();
    }
}

void adc_init(void)
{
    ADC_0_register_callback(ADC_0_adc_handler_cb_copy);
}
