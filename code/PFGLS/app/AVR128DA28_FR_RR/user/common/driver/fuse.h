/**
 * @file fuse.h
 * @author 曹志成 (czc13611858691@gmail.com)
 * @brief 这个文件需要在Bootloader工程中被包含
 * @version 0.1
 * @date 2022-02-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
#include <avr/io.h>
FUSES = {
	.OSCCFG = CLKSEL_OSCHF_gc, // High frequency oscillator selected
	.SYSCFG0 = CRCSRC_NOCRC_gc | RSTPINCFG_GPIO_gc, // No CRC enabled, RST pin in GPIO mode
	.SYSCFG1 = SUT_64MS_gc, // Start-up time 64 ms
	.BOOTSIZE = 0x32, // BOOT size = 0x02 * 512 bytes = 1024 bytes，设置BOOT程序区域大小
	.CODESIZE = 0x00, // All remaining Flash used as App code
	.BODCFG = 0x44, // BOD ENABLE ,BOD 2.7V，睡眠唤醒多次测试失败的bug
};