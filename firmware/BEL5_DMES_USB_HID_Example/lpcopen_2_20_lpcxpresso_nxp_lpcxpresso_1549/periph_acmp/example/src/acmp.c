/*
 * @brief Analog Comparator example.
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Example uses ACMP #3 */
#define CMP_INDEX 3

#if defined(BOARD_KEIL_MCB1500)
/* On the KEIL MDB1500 use ACMP_I1 (PIO0_27) as the signal input.  Connect a
   jumper wire between the potentiometer (POT_MOT1 pin 3) and PIO0_27/ACMP_I1
   (MOTOR2-pin 17). */
#define INPUT_SELECTED      ACMP_POSIN_ACMP_I1
#define ANALOG_INPUT_PORT   0
#define ANALOG_INPUT_BIT    27
#define ANALOG_FIXED_PIN    SWM_FIXED_ACMP_I1

#elif defined(BOARD_NXP_LPCXPRESSO_1549)
/* The NXP LPCXpresso 1549 uses ADC1_1 as the input. This is already connected
   to the potentiometer. */
#define INPUT_SELECTED      ACMP_POSIN_ADCIN_1
#define ANALOG_INPUT_PORT   0
#define ANALOG_INPUT_BIT    9
#define ANALOG_FIXED_PIN    SWM_FIXED_ADC1_1

#else
#error "No positive input selected"
#endif

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* ACMP Pin mux function - note that SystemInit() may already setup your
   pin muxing at system startup */
static void Init_ACMP_PinMux(void)
{
	/* Disables pullups/pulldowns and disable digital mode */
	Chip_IOCON_PinMuxSet(LPC_IOCON, ANALOG_INPUT_PORT, ANALOG_INPUT_BIT, 
		(IOCON_MODE_INACT | IOCON_ADMODE_EN));
	Chip_SWM_EnableFixedPin(ANALOG_FIXED_PIN);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Analog comparator interrupt handler sub-routine
 * @return	Nothing
 */
void ACMP3_IRQHandler(void)
{
	/* Clear the interrupt */
	Chip_ACMP_ClearIntFlag(LPC_CMP, CMP_INDEX);
}

/**
 * @brief	Main program body
 * @return	Does not return
 */
int main(void)
{
	/* initialize the board */
	SystemCoreClockUpdate();
	Board_Init();
	Board_LED_Set(0, false);

	/* initialize the ACMP */
	Chip_ACMP_Init(LPC_CMP);

	/* Setup board specific ACMP pin muxing */
	Init_ACMP_PinMux();

	/* Positive and negative references, no hysteresis */
	Chip_ACMP_SetupACMPRefs(LPC_CMP, CMP_INDEX, INPUT_SELECTED, ACMP_NEGIN_VREF_DIV, ACMP_HYS_NONE);
	/* Edge triggered interrupt on both edges*/
	Chip_ACMP_SetupACMPInt(LPC_CMP, CMP_INDEX, false, false, ACMP_EDGESEL_BOTH);
	Chip_ACMP_EnableCompInt(LPC_CMP, CMP_INDEX);
	Chip_ACMP_SetupVoltLadder(LPC_CMP, CMP_INDEX, 0x15, false);
	Chip_ACMP_EnableVoltLadder(LPC_CMP, CMP_INDEX);
	/* Setup Comparator Filter register to bypass filter and use SysClk without any division */
	Chip_ACMP_SetCompFiltReg(LPC_CMP, CMP_INDEX, ACMP_SMODE_0, ACMP_CLKDIV_1);
	/* Enable Comparator */
	Chip_ACMP_EnableComp(LPC_CMP, CMP_INDEX);

	/* Enable the Interrupt for the compare output */
	NVIC_EnableIRQ(CMP3_IRQ);

	while (1) {
		/* Enter low power mode until interrupt */
		__WFI();

		if (Chip_ACMP_GetCompStatus(LPC_CMP, CMP_INDEX)) {
			Board_LED_Set(0, true);
		}
		else {
			Board_LED_Set(0, false);
		}
	}

	return 0;
}
