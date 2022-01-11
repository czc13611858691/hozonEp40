/**
 * @file adc.c
 * @author 曹志成
 * @brief 获取霍尔传感器的值，在软件定时器中10ms开启一次转换，转换完成触发中断，中断完成切换通道
 * @date 2021-10-20
 */
#include "adc.h"

adc_result_t ADC_0_measurement[2];

adc_0_channel_t adc_channel = ADC_MUXPOS_AIN5_gc;

void ADC_0_adc_handler_cb_copy(void)
{
    if (adc_channel == ADC_MUXPOS_AIN5_gc)
    {
        adc_channel = ADC_MUXPOS_AIN7_gc;
        ADC_0_measurement[0] = ADC_0_get_conversion_result();
    }
    else if (adc_channel == ADC_MUXPOS_AIN7_gc)
    {
        adc_channel = ADC_MUXPOS_AIN5_gc;
        ADC_0_measurement[1] = ADC_0_get_conversion_result();
    }
}

void adc_init(void)
{
    ADC_0_register_callback(ADC_0_adc_handler_cb_copy);
}
