#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#pragma once

#include "nvmctrl_basic.h"

extern int erase_flag;
extern int flash_flag;
extern int jump_flag;
extern int bin_size_cnt;
extern uint16_t update_cnt;
extern flash_adr_t page_addr;
extern uint8_t program_buffer[512];
extern uint8_t history_blcok_count;

#endif /* APP_CONFIG_H_ */