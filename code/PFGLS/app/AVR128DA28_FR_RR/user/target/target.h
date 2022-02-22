#pragma once

#include <stddef.h>
#include <stdint.h>
#include "led.h"
#include "touch_press_motor.h"
#include "atmel_start_pins.h"

// #define USER_BOOTLOADER
#define APP_MASK_EXCEL_DEF 0x460DB0A7
#define LIN_USART_X USART0
#define LIN_USART_ISR_VECT USART0_RXC_vect
#define ADC_RES_0_CHANNEL ADC_MUXPOS_AIN5_gc
#define ADC_RES_1_CHANNEL ADC_MUXPOS_AIN7_gc
#define LIN_TX_ID 0x21
#define LIN_NAD 0x60
#define LED_NUM (8)
#define HC595_NUM (1)
#define HC595_OUT_PIN (8)
#define PRESS_DELTA_D1 2
#define PRESS_DELTA_D2 2
#define PRESS_DELTA_D3 2
#define LIN_TX_SET_DIR PA0_set_dir
#define LIN_TX_SET_LEVEL PA0_set_level
#define DUTY_MAX 1199

extern uint8_t g_hc595_buffer[1];
extern led_Dx_t g_led_Dx[LED_NUM];
extern state_obj_t g_btn_a_state;
extern state_obj_t g_btn_b_state;
extern state_obj_t g_btn_self_state;

void target_soft_timer_init(void);
size_t USART0_Read(uint8_t * rDATA);
size_t USART0_Send(uint8_t * tDATA);
void soft_timer_touch_press_motor_task(void);
void lin_go_to_sleep(void);
