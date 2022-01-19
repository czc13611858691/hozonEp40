/*
 * led.c
 *
 * Created: 2021/9/24 15:15:36
 *  Author: Sunny
 */
#include "led.h"
#include "hc595.h"
#include <atmel_start.h>
#include "user_common.h"

uint8_t g_hc595_buffer[1] = {0xff};

//   x──────────────────┐
//   x      D7          │
//   x                  │
//   x                  │
//   x      D4          │
//   x                  │
//   x      D5          │
//   x                  │
//   x      D6          │
//   x                  │
//   x      D8          │
//   x                  │
//   x                  │
//  xx                  │
//  x       D9          │
//  x                   │
//  x                   │
// x        D11         │
// x                    │
// x        D10   blue  │
// x                   xx
// x             xxxxx
// x          xxxx
// x        xx
// x    xxxx
// x xxxx

typedef struct
{
	uint8_t byte;
	uint8_t bit;
} led_Dx_t;

#define LED_NUM (8)

const led_Dx_t g_led_Dx[LED_NUM] = {
	{0, 0}, /* LED8 D11 */
	{0, 1}, /* LED4 D9 */
	{0, 2}, /* LED7 D10 */
	{0, 3}, /* LED6 D7 */
	{0, 4}, /* LED1 D4 */
	{0, 5}, /* LED2 D5 */
	{0, 6}, /* LED5 D6 */
	{0, 7}, /* LED3 D8  */
};

// const led_Dx_t g_led_Dx[LED_NUM] = {
// 	{0, 0}, /* LED8 D11 */
// 	{0, 1}, /* LED4 D9 */
// 	{0, 2}, /* LED7 D10 */
// 	{0, 3}, /* LED1 D4 */
// 	{0, 4}, /* LED2 D5 */
// 	{0, 5}, /* LED5 D6 */
// 	{0, 6}, /* LED3 D8 */
// 	{0, 7}, /* LED6 D7  */
// };

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
	/* 亮度控制，目前20KHz，最大1199,默认%45亮度 */
	// TCA0.SINGLE.CMP2 = 540;

	// LED_ON_N_set_level(0);
	led_set_level(led_d10, 0);
	// led_set_level(led_d10, 0);
	// led_set_level(led_d9, 0);
	// led_set_level(led_d8, 0);
	// led_set_level(led_d7, 0);
	// led_set_level(led_d5, 0);
	// led_set_level(led_d6, 0);
	// led_set_level(led_d4, 0);
	HC595_Write(g_hc595_buffer, HC595_NUM, HC595_OUT_PIN);
}