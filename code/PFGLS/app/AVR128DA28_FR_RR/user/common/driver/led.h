#ifndef LED_H_
#define LED_H_

#include <stdint.h>

typedef enum
{
    led_d11,
    led_d9,
    led_d10,
    led_d4,
    led_d5,
    led_d6,
    led_d8,
    led_d7,
} led_Dx_e;

typedef struct
{
	uint8_t byte;
	uint8_t bit;
} led_Dx_t;


void led_set_level(led_Dx_e Dx, uint8_t level);
void led_update_buf_task(void);
void led_init(void);

#endif /* LED_H_ */