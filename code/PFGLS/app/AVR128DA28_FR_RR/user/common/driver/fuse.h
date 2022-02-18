#pragma once
#include <avr/io.h>
FUSES = {
	.OSCCFG = CLKSEL_OSCHF_gc, // High frequency oscillator selected
	.SYSCFG0 = CRCSRC_NOCRC_gc | RSTPINCFG_GPIO_gc, // No CRC enabled, RST pin in GPIO mode
	.SYSCFG1 = SUT_64MS_gc, // Start-up time 64 ms
	.BOOTSIZE = 0x32, // BOOT size = 0x02 * 512 bytes = 1024 bytes
	.CODESIZE = 0x00, // All remaining Flash used as App code
	.BODCFG = 0x44, // BOD ENABLE ,BOD 2.7V
};