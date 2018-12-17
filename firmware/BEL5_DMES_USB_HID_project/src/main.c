/*
 * main.c
 *
 *
 *
 *
 *
 *
 */

/*********************************************************************LIBS*/
#include "board.h"
#include "chip.h"
//#include "gpio.h"

#include "int_mcu_state.h"
#include "../usb_hid/usb/inc/usb_hid.h"
#include "SEGGER_RTT.h"

#include <string.h>
#include <stdio.h>

//#include <Arduino.h>
//#include <MD_AD9833.h>
/**********************************************************************/

#define POW_2_28 268435456
#define F_MCLK 25000000		// clk freq of AD9833 is 25MHz

#define BUFSIZE 64
#define DDS_BUF 5 // 5*8 bytes are sent
#define FREQ0_WRITE_REG		0x4000
#define FREQ1_WRITE_REG		0x8000  // not really needed in our setup

#define PHASE_WRITE_CMD		0xC000		// Setup for Phase write


#define LEN_SIG_TYPE 1
#define LEN_FREQ 3
#define LEN_PHASE 2
#define LEN_ANSWER 0

#define ACK 1
#define NACK 2

// evaluate type of commands from GUI
typedef enum {
	DEV_STAT = 0x01, // not needed
	SIG_TYPE = 0x02,
	SIG_FREQ = 0x03,
	SIG_VALS = 0x04, // not needed
	SIG_PHAS = 0x05
} CommandCode;

// see Arduino library, AD9833.h
typedef enum {
	SINE = 0x2000, TRIANGLE = 0x2002, SQUARE = 0x2028, HALF_SQUARE = 0x2020
} WaveformType;

//typedef enum {
//	REG0, REG1, SAME_AS_REG0
//} Registers;

/******************************************************************GLOBALS*/

static SPI_CFG_T ConfigStruct;
static SPI_DELAY_CONFIG_T DelayConfigStruct;
static SPI_DATA_SETUP_T XfSetup;
uint8_t data_ptr[BUFSIZE]; // receive commands from GUI

static uint16_t TxBuf[DDS_BUF];

uint8_t command_code;
uint16_t signal_type;
// uint16_t freq;
uint32_t freq;
uint8_t phase;
double phase_conversion_factor = 6.283185307/4096;

/***************************************************************+FUNCTIONS*/

void evaluate_command_code() {
	switch (data_ptr[0]) {
	case SIG_TYPE:
		SEGGER_RTT_printf(0, "received sig info\n");
		set_sig_type();
		break;
	case SIG_FREQ:
		SEGGER_RTT_printf(0, "received freq info\n");
		set_frequency();
		break;
	case SIG_PHAS:
		SEGGER_RTT_printf(0, "received phase info\n");
		set_phase();
		break;
	default:
		// send NACK or/and do nothing
		SEGGER_RTT_printf(0, "received invalid command code from GUI\n");
	}

	//send_DDS_data();
}

void set_sig_type() {
	switch (data_ptr[3]) {
	case 1:
		signal_type = SINE;
		break;
	case 2:
		signal_type = TRIANGLE;
		break;
	case 3:
		signal_type = SQUARE;
		break;
	default:
		signal_type = SINE;
	}
	//Chip_SPI_RWFrames_Blocking(LPC_SPI0, waveform);
	//Chip_SPI_WriteFrames_Blocking(LPC_SPI0, &XfSetup);
	send_DDS_data();
}

void set_frequency()
{
	freq = (data_ptr[3] << 16) | (data_ptr[4] << 8) | (data_ptr[5]);
	freq = freq/10;
	send_DDS_data();
}

void set_phase() {
	phase = (data_ptr[3] << 8) | (data_ptr[4]);
	//Chip_SPI_RWFrames_Blocking(LPC_SPI0, phase);
	//Chip_SPI_WriteFrames_Blocking(LPC_SPI0, &XfSetup);
	send_DDS_data();
}

void send_DDS_data() {

	// prepare frequency information
	uint32_t DDS_freq = (uint32_t)(freq * (POW_2_28 / (double) F_MCLK));
	uint16_t MSB = (DDS_freq & 0xFFFC000) >> 14;
	uint16_t LSB = DDS_freq & 0x3FFF;

	LSB |= FREQ0_WRITE_REG;
	MSB |= FREQ0_WRITE_REG;

	// prepare phase information
	uint16_t DDS_phase = (uint16_t)((double)phase * phase_conversion_factor);
	DDS_phase |= PHASE_WRITE_CMD;
	//DDS_phase |= 0x000C000;

	TxBuf[0] = 0x2100;
	TxBuf[1] = LSB;
	TxBuf[2] = MSB;
	TxBuf[3] = DDS_phase;
	TxBuf[4] = signal_type;

	//Chip_SPI_RWFrames_Blocking(LPC_SPI0, &XfSetup);
	Chip_SPI_WriteFrames_Blocking(LPC_SPI0, &XfSetup);
}

/*void apply_signal(WaveformType waveType, float frequencyInHz, float phaseInDeg = 0.0, Registers freqReg)
 * {
	// prepare data for sending

	// 1 byte cmd, then always 5 for signal gen., length (depending on cmd),

	// send data over SPI
	XfSetup.Length = DDS_BUF;
	XfSetup.pTx = TxBuf;
	XfSetup.RxCnt = XfSetup.TxCnt = 0;
	XfSetup.DataSize = 16;  //  sending 5 * 16 bit = HEX values
	TxBuf[0] = 0;
	TxBuf[1] = 1;
	XfSetup.TxCnt = 0;

	Chip_SPI_WriteFrames_Blocking(LPC_SPI0, &XfSetup);
}*/

// see also https://community.nxp.com/thread/460774
void init_SPI() {
	// set up SPI to communicate with AD9833
	//Chip_SPI_Init();

	/* Enable the clock to the Switch Matrix */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
	/*
	 * Initialize SPI0 pins connect
	 * SCK0: PINASSIGN3[15:8]: Select P0.0
	 * MOSI0: PINASSIGN3[23:16]: Select P0.16
	 * MISO0: PINASSIGN3[31:24] : Select P0.10
	 * SSEL0: PINASSIGN4[7:0]: Select P0.9
	 */

	// assign SPI pins here
	Chip_SWM_MovablePinAssign(SWM_SPI0_SCK_IO, 3);
	Chip_SWM_MovablePinAssign(SWM_SPI0_MOSI_IO, 4);
	Chip_SWM_MovablePinAssign(SWM_SPI0_MISO_IO, 10);
	Chip_SWM_MovablePinAssign(SWM_SPI0_SSELSN_0_IO, 5);

	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 3,
			(IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 4,
			(IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 10,
			(IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 5,
			(IOCON_MODE_INACT | IOCON_DIGMODE_EN));

	ConfigStruct.Mode = SPI_MODE_MASTER;
	ConfigStruct.ClkDiv = Chip_SPI_CalClkRateDivider(LPC_SPI0, 100000);
	ConfigStruct.ClockMode = SPI_CLOCK_CPHA0_CPOL0;
	ConfigStruct.DataOrder = SPI_DATA_MSB_FIRST;
	ConfigStruct.SSELPol = SPI_CFG_SPOL0_LO;

	Chip_SPI_Init(LPC_SPI0);
	Chip_SPI_SetConfig(LPC_SPI0, &ConfigStruct);

	DelayConfigStruct.FrameDelay = 0;
	DelayConfigStruct.PostDelay = 0;
	DelayConfigStruct.PreDelay = 0;
	DelayConfigStruct.TransferDelay = 0;
	Chip_SPI_DelayConfig(LPC_SPI0, &DelayConfigStruct);

	Chip_SPI_Enable(LPC_SPI0);

	XfSetup.Length = DDS_BUF;
	XfSetup.pTx = TxBuf;
	XfSetup.RxCnt = XfSetup.TxCnt = 0;
	//XfSetup.DataSize = 8;
	XfSetup.DataSize = 16; // 5*16 bit as HEX vals
	//TxBuf[0] = 0;
	//TxBuf[1] = 1;
	TxBuf[0] = 0x2100;
	TxBuf[1] = freq;
	TxBuf[2] = freq;
	TxBuf[3] = phase;
	TxBuf[4] = signal_type;

	XfSetup.pRx = NULL;

	Chip_SPI_WriteFrames_Blocking(LPC_SPI0, &XfSetup);

	/* Disable the clock to the Switch Matrix to save power */
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);
	//

}

int main(void) {
	int iRet = 0;
	//uint16_t tuning_word = 1;

	/* Generic Initialization */
	SystemCoreClockUpdate();
	Chip_GPIO_Init(LPC_GPIO);

	mcu_config_LED_pins();
	mcu_enable_pwr_LED();
	//mcu_enable_err_LED();

	// create phase accumulator (if working with internal DAC)
	// create sin LUT (if working with internal DAC)

	iRet = usb_config_hid_device();
	if (iRet != LPC_OK) {
		return iRet;
	}

	//uint32_t PLL_rate = Chip_Clock_GetSystemPLLOutClockRate();

	init_SPI();

	// init DAC
	Chip_DAC_Init(LPC_DAC);

	while (1) {
		//__WFI();
		//Board_UARTGetChar();
		// calls from driver lib /usb_hid/usb/inc/usb_hid.h

		my_usb_hid_receive(data_ptr);
		// debugging with JLink RTT viewer
		//SEGGER_RTT_printf(0, "received: %s\n", data_ptr);

		evaluate_command_code();
		// send ACK or NACK

		//my_usb_hid_transmit(data_ptr);
		//SEGGER_RTT_printf(0, "transmitted: %s\n\n", data_ptr);

		// params: WaveformType waveType, float frequencyInHz, float phaseInDeg = 0.0,  Registers freqReg
		//apply_signal(SINE_WAVE, 2000, float phaseInDeg = 0.0);

	}

	return 0;
}

/* EOF */
