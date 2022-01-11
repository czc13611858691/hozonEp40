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

uint8_t g_hc595_buffer[2] = {0xD7, 0xff};

//                              x x xxxxx
//     xxx x x  x   x   x  x x          x
//     x                                x
//     x                                x
//     x                                x
//     x     D19          D18           x
//     x                                x
//     x								x
//     x     D14          D15          x
//     x                               x
//    xx                               x
//    x      D12          D13          x
//    x                                x
//   x                                 x
//   x       D11          D4           x
//  xx                                 x
//  x                                  x
// xx        D17          D16          x
// x                                   x
// x                                   x
// x         D10          D9           x
// x                                   x
//  x                                  x
//  x                                  x
//   x                                 x
//   x       D8           D6           x
//    x      D7 BLUE      D5 BLUE      xx
//    x                                 x
//    x                                 x
//    xx                                x
//     xx                              xx
//       xx                            x
//        xx                           x
//          xxx                        x
//            xxxx                     x
//                xxxxx               xx
//                     xxxxxxx       xx
//                            xxxxxxx

typedef struct
{
	uint8_t byte;
	uint8_t bit;
} led_Dx_t;

#define LED_NUM (16)

const led_Dx_t g_led_Dx[LED_NUM] = {
	{0, 7}, /* LED7 D16 */
	{0, 6}, /* LED5 D9 */
	{0, 5}, /* LED1 D5 */
	{0, 4}, /* LED2 D6 */
	{0, 3}, /* LED3 D7 */
	{0, 2}, /* LED4 D8 */
	{0, 1}, /* LED6 D10 */
	{0, 0}, /* LED8 D17 */

	{1, 7}, /* LED14 D15 */
	{1, 6}, /* LED12 D13 */
	{1, 5}, /* LED9 D4 */
	{1, 4}, /* LED10 D11 */
	{1, 3}, /* LED11 D12 */
	{1, 2}, /* LED13 D14 */
	{1, 1}, /* LED16 D19 */
	{1, 0}, /* LED15 D18 */
};

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
	/* 亮度控制，目前20KHz，最大0x4a,默认%45亮度 */
	// TCA0.SPLIT.HCMP0 = 33;

	LED_ON_N_set_level(0);
	HC595_Write(g_hc595_buffer, HC595_NUM, HC595_OUT_PIN);
}