#include "target.h"
#include "atmel_start.h"
#include "led.h"
#include "lin_commontl_api.h"
#include "UDS.h"
#include "soft_timer.h"

#ifdef USER_BOOTLOADER
#include "fuse.h"
#endif


int main(void)
{
	atmel_start_init();

	Enable_global_interrupt();

	led_init();

	target_soft_timer_init();

	l_sys_init();
	l_ifc_init(LI0);
	ld_init(LI0);

	UDS_read_DID_from_eeprom();

	while (1)
	{
		soft_timer_run();
		touch_process();
		UDS_flg_check_task();
	}
}
