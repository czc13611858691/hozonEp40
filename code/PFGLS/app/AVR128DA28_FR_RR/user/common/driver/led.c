#include "led.h"
#include "hc595.h"
#include <atmel_start.h>
#include "bit_operation.h"
#include "target.h"

void led_set_level(led_Dx_e Dx, uint8_t level)
{
	uint8_t byte = 0;
	uint8_t bit = 0;
	byte = g_led_Dx[Dx].byte;
	bit = g_led_Dx[Dx].bit;
	if (level == 1)
	{
		bitSet(g_hc595_buffer[byte], bit);
	}
	else
	{
		bitClear(g_hc595_buffer[byte], bit);
	}
}

void led_update_buf_task(void)
{
	HC595_Write(g_hc595_buffer, HC595_NUM, HC595_OUT_PIN);
}

void led_init(void)
{
	HC595_Write(g_hc595_buffer, HC595_NUM, HC595_OUT_PIN);
}
