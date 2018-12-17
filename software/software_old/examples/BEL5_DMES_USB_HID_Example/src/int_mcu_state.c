/*
 * int_mcu_state.c
 *
 *  Created on: 08.01.2018
 *      Author: Markus Lechner
 */

/*********************************************************************LIBS*/
#include "board.h"
#include "lpc_types.h"
#include "int_mcu_state.h"
/**********************************************************************DEFINES*/
/**********************************************************************CONSTANTS*/
/* Green LED for MCU Power indication */
static uint8_t mcu_LED_pwr_pin = 7;
static uint8_t mcu_LED_pwr_port = 1;
/* Red LED for MCU Error indication */
static uint8_t mcu_LED_err_pin = 8;
static uint8_t mcu_LED_err_port = 1;
/**********************************************************************FUNCTIONS_CONFIG*/
void mcu_config_LED_pins() {
	/* Enable Digital Pin Function */
	Chip_IOCON_PinMuxSet(LPC_IOCON, mcu_LED_pwr_port, mcu_LED_pwr_pin,
			(IOCON_MODE_INACT | IOCON_DIGMODE_EN));

	Chip_IOCON_PinMuxSet(LPC_IOCON, mcu_LED_err_port, mcu_LED_err_pin,
			(IOCON_MODE_INACT | IOCON_DIGMODE_EN));

	Chip_GPIO_SetPinDIROutput(LPC_GPIO, mcu_LED_pwr_port, mcu_LED_pwr_pin);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, mcu_LED_err_port, mcu_LED_err_pin);

	Chip_GPIO_SetPinState(LPC_GPIO, mcu_LED_pwr_port, mcu_LED_pwr_pin, false);
	Chip_GPIO_SetPinState(LPC_GPIO, mcu_LED_err_port, mcu_LED_err_pin, false);
}

void mcu_enable_pwr_LED() {
	Chip_GPIO_SetPinState(LPC_GPIO, mcu_LED_pwr_port, mcu_LED_pwr_pin, true);
}

void mcu_disable_pwr_LED() {
	Chip_GPIO_SetPinState(LPC_GPIO, mcu_LED_pwr_port, mcu_LED_pwr_pin, false);
}

void mcu_enable_err_LED() {
	Chip_GPIO_SetPinState(LPC_GPIO, mcu_LED_err_port, mcu_LED_err_pin, true);
}

void mcu_disable_err_LED() {
	Chip_GPIO_SetPinState(LPC_GPIO, mcu_LED_err_port, mcu_LED_err_pin, false);
}

/* EOF */
