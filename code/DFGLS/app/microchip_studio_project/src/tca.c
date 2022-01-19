/**
 * \file
 *
 * \brief TCA related functionality implementation.
 *
 (c) 2020 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms,you may use this software and
    any derivatives exclusively with Microchip products.It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/**
 * \addtogroup doc_driver_tca
 *
 * \section doc_driver_tca_rev Revision History
 * - v0.0.0.1 Initial Commit
 *
 *@{
 */
#include <tca.h>

/**
 * \brief Initialize tca interface
 *
 * \return Initialization status.
 */
int8_t TIMER_1_init()
{

	TCA0.SPLIT.CTRLD = 1 << TCA_SPLIT_SPLITM_bp /* Split Mode : enabled */;

	TCA0.SPLIT.CTRLB = 1 << TCA_SPLIT_HCMP0EN_bp    /* High Compare 0 Enable: enabled */
	                   | 0 << TCA_SPLIT_HCMP1EN_bp  /* High Compare 1 Enable: disabled */
	                   | 0 << TCA_SPLIT_HCMP2EN_bp  /* High Compare 2 Enable: disabled */
	                   | 0 << TCA_SPLIT_LCMP0EN_bp  /* Low Compare 0 Enable: disabled */
	                   | 0 << TCA_SPLIT_LCMP1EN_bp  /* Low Compare 1 Enable: disabled */
	                   | 0 << TCA_SPLIT_LCMP2EN_bp; /* Low Compare 2 Enable: disabled */

	TCA0.SPLIT.CTRLC = 1 << TCA_SPLIT_HCMP0OV_bp    /* High Compare 0 Output Value: enabled */
	                   | 0 << TCA_SPLIT_HCMP1OV_bp  /* High Compare 1 Output Value: disabled */
	                   | 0 << TCA_SPLIT_HCMP2OV_bp  /* High Compare 2 Output Value: disabled */
	                   | 0 << TCA_SPLIT_LCMP0OV_bp  /* Low Compare 0 Output Value: disabled */
	                   | 0 << TCA_SPLIT_LCMP1OV_bp  /* Low Compare 1 Output Value: disabled */
	                   | 0 << TCA_SPLIT_LCMP2OV_bp; /* Low Compare 2 Output Value: disabled */

	TCA0.SPLIT.HCMP0 = 0x4f; /* Compare value of channel 0: 0x4f */

	// TCA0.SPLIT.HCMP1 = 0x0; /* Compare value of channel 1: 0x0 */

	// TCA0.SPLIT.HCMP2 = 0x0; /* Compare value of channel 2: 0x0 */

	// TCA0.SPLIT.HCNT = 0x0; /*  High-byte Timer Counter Register: 0x0 */

	TCA0.SPLIT.HPER = 0x4a; /*  High-byte Period Register: 0x4a */

	// TCA0.SPLIT.INTCTRL = 0 << TCA_SPLIT_HUNF_bp /* High Underflow Interrupt Enable: disabled */
	//		 | 0 << TCA_SPLIT_LCMP0_bp /* Low Compare 0 Interrupt Enable: disabled */
	//		 | 0 << TCA_SPLIT_LCMP1_bp /* Low Compare 1 Interrupt Enable: disabled */
	//		 | 0 << TCA_SPLIT_LCMP2_bp /* Low Compare 2 Interrupt Enable: disabled */
	//		 | 0 << TCA_SPLIT_LUNF_bp; /* Low Underflow Interrupt Enable: disabled */

	// TCA0.SPLIT.LCMP0 = 0x0; /* Compare value Channel 0: 0x0 */

	// TCA0.SPLIT.LCMP1 = 0x0; /* Compare value Channel 1: 0x0 */

	// TCA0.SPLIT.LCMP2 = 0x0; /* Compare value Channel 2: 0x0 */

	// TCA0.SPLIT.LCNT = 0x0; /* Low-byte Timer Counter Register: 0x0 */

	// TCA0.SPLIT.LPER = 0xff; /*  Low-byte Timer Period Register: 0xff */

	// TCA0.SPLIT.DBGCTRL = 0 << TCA_SPLIT_DBGRUN_bp; /* Debug Run: disabled */

	TCA0.SPLIT.CTRLA = TCA_SPLIT_CLKSEL_DIV16_gc /* System Clock / 16 */
	                   | 1 << TCA_SPLIT_ENABLE_bp /* Module Enable: enabled */;

	return 0;
}
