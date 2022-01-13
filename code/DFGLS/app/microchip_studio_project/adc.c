#include "adc.h"

adc_result_t ADC_0_measurement[2];

adc_0_channel_t adc_channel = ADC_MUXPOS_AIN14_gc;

void ADC_0_adc_handler_cb_copy(void)
{
    if (adc_channel == ADC_MUXPOS_AIN14_gc)
    {
        adc_channel = ADC_MUXPOS_AIN15_gc;
        ADC_0_measurement[0] = ADC_0_get_conversion_result();
    }
    else if (adc_channel == ADC_MUXPOS_AIN15_gc)
    {
        adc_channel = ADC_MUXPOS_AIN14_gc;
        ADC_0_measurement[1] = ADC_0_get_conversion_result();
    }
}

void adc_init(void)
{
    ADC_0_register_callback(ADC_0_adc_handler_cb_copy);
}
