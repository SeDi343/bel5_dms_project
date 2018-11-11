/*
 * main.c
 *
 *  Created on: 28.09.2017
 *      Author: Markus Lechner
 */

/*********************************************************************LIBS*/
#include "board.h"

#include "int_mcu_state.h"
#include "../usb_hid/usb/inc/usb_hid.h"
/**********************************************************************/
int main(void) {
	int iRet = 0;

	/* Generic Initialization */
	SystemCoreClockUpdate();
	Chip_GPIO_Init(LPC_GPIO);

	mcu_config_LED_pins();
	mcu_enable_pwr_LED();

	iRet = usb_config_hid_device();
	if(iRet != LPC_OK) {
		return iRet;
	}

	while(1){
		//__WFI();
	}

	return 0;
}

/* EOF */
