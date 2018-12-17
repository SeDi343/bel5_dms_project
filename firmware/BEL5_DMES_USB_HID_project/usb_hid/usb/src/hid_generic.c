/*
 * @brief HID generic example's callabck routines
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
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
/*********************************************************************LIBS*/
#include <string.h>
#include "board.h"
#include "hid_generic.h"
#include "usbd_rom_api.h"
/*********************************************************************DEFINES*/
/**********************************************************************TYPEDEFS*/
typedef struct {
	USBD_HANDLE_T hUsb; /*!< Handle to USB stack. */
	uint8_t report_out_buf[MAX_REPORT_SIZE]; /*!< Last report data  */
	_Bool tx_out_busy; /*!< Flag indicating if a report is pending in endpoint queue. */
	_Bool rec_wait;
	uint8_t endptr;
} HID_CTRL_T;
/*********************************************************************GLOBALS-STATIC*/
static HID_CTRL_T g_HID_ctrl; /* Singleton instance of HID control */
static uint8_t *loopback_report;
/****************************************************************************EXTERN*/
extern const uint8_t HID_ReportDescriptor[];
extern const uint16_t HID_ReportDescSize;
/**********************************************************************PRIVATE-FUNCTIONS*/
/*  HID get report callback function. */
static ErrorCode_t HID_GetReport(USBD_HANDLE_T hHid, USB_SETUP_PACKET *pSetup,
		uint8_t * *pBuffer, uint16_t *plength) {
	/* ReportID = SetupPacket.wValue.WB.L; */
	switch (pSetup->wValue.WB.H) {
	case HID_REPORT_INPUT:
		*pBuffer[0] = *loopback_report;
		*plength = 1;
		break;

	case HID_REPORT_OUTPUT:
		return ERR_USBD_STALL; /* Not Supported */

	case HID_REPORT_FEATURE:
		return ERR_USBD_STALL; /* Not Supported */
	}
	return LPC_OK;
}

/* HID set report callback function. */
static ErrorCode_t HID_SetReport(USBD_HANDLE_T hHid, USB_SETUP_PACKET *pSetup,
		uint8_t * *pBuffer, uint16_t length) {
	/* we will reuse standard EP0Buf */
	if (length == 0) {
		return LPC_OK;
	}

	/* ReportID = SetupPacket.wValue.WB.L; */
	switch (pSetup->wValue.WB.H) {
	case HID_REPORT_INPUT:
		return ERR_USBD_STALL; /* Not Supported */

	case HID_REPORT_OUTPUT:
		*loopback_report = **pBuffer;
		break;

	case HID_REPORT_FEATURE:
		return ERR_USBD_STALL; /* Not Supported */
	}
	return LPC_OK;
}

/* HID Interrupt endpoint event handler. */
static ErrorCode_t HID_Ep_Hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event) {
	USB_HID_CTRL_T *pHidCtrl = (USB_HID_CTRL_T *) data;

	switch (event) {
	case USB_EVT_IN:
		/* USB_EVT_IN occurs when HW completes sending IN packet */
		g_HID_ctrl.tx_out_busy = false;
		break;

	case USB_EVT_OUT:
		/* Read the new report received. */

		g_HID_ctrl.endptr = pHidCtrl->epout_adr;

		g_HID_ctrl.rec_wait = true;

		//USBD_API->hw->ReadEP(hUsb, pHidCtrl->epout_adr, loopback_report);

		/* Avoid time consuming operations in the ISR! This is only for demonstration purposes!!!*/
		//hid_transmit_data();
		break;
	}
	return LPC_OK;
}

/**********************************************************************PUBLIC-FUNCTIONS*/
/* HID init routine */
ErrorCode_t usb_hid_init(USBD_HANDLE_T hUsb,
		USB_INTERFACE_DESCRIPTOR *pIntfDesc, uint32_t *mem_base,
		uint32_t *mem_size) {
	USBD_HID_INIT_PARAM_T hid_param;
	USB_HID_REPORT_T reports_data[1];
	ErrorCode_t ret = LPC_OK;

	if ((pIntfDesc == 0)
			|| (pIntfDesc->bInterfaceClass != USB_DEVICE_CLASS_HUMAN_INTERFACE)) {
		return ERR_FAILED;
	}

	/* Init HID params */
	memset((void *) &hid_param, 0, sizeof(USBD_HID_INIT_PARAM_T));

	hid_param.max_reports = 1;
	hid_param.mem_base = *mem_base;
	hid_param.mem_size = *mem_size;
	hid_param.intf_desc = (uint8_t *) pIntfDesc;
	/* user defined functions */
	hid_param.HID_GetReport = HID_GetReport;
	hid_param.HID_SetReport = HID_SetReport;
	hid_param.HID_EpIn_Hdlr = HID_Ep_Hdlr;
	hid_param.HID_EpOut_Hdlr = HID_Ep_Hdlr;

	/* Init reports_data */
	reports_data[0].len = HID_ReportDescSize;
	reports_data[0].idle_time = 0;
	reports_data[0].desc = (uint8_t *) &HID_ReportDescriptor[0];
	hid_param.report_data = reports_data;

	ret = USBD_API->hid->init(hUsb, &hid_param);

	/* allocate USB accessible memory space for report data */
	loopback_report = (uint8_t *) hid_param.mem_base;
	hid_param.mem_base += 4;
	hid_param.mem_size += 4;
	/* update memory variables */
	*mem_base = hid_param.mem_base;
	*mem_size = hid_param.mem_size;

	/* store stack handle for later use. */
	g_HID_ctrl.hUsb = hUsb;
	return ret;
}

_Bool hid_transmit_data() {
	char hid_data[] = "Received_Packet";

	/* Check device is configured before sending report. */
	if (USB_IsConfigured(g_HID_ctrl.hUsb)) {
		if (!g_HID_ctrl.tx_out_busy) {
			memcpy(&g_HID_ctrl.report_out_buf[0], hid_data, sizeof(hid_data));

			/* Send report data */
			g_HID_ctrl.tx_out_busy = true;
			USBD_API->hw->WriteEP(g_HID_ctrl.hUsb, HID_EP_IN,
					&g_HID_ctrl.report_out_buf[0], MAX_REPORT_SIZE);
			return true;
		} else {
			return false;
		}
	} else {
		/* Reset busy flag if we get disconnected. */
		g_HID_ctrl.tx_out_busy = false;
		return false;
	}
}

void my_rec_data(uint8_t *data_ptr) {

	// wait unitl I receive sth.
	while (!g_HID_ctrl.rec_wait) {;}
	g_HID_ctrl.rec_wait = false;
	USBD_API->hw->ReadEP(g_HID_ctrl.hUsb, g_HID_ctrl.endptr, data_ptr);
}

bool my_tx_data(uint8_t *data_ptr) {
	//char hid_data[] = "Received_Packet";

	/* Check device is configured before sending report. */
	if (USB_IsConfigured(g_HID_ctrl.hUsb)) {
		if (!g_HID_ctrl.tx_out_busy) {
			memcpy(&g_HID_ctrl.report_out_buf[0], data_ptr, MAX_REPORT_SIZE /*sizeof(data_ptr)*/);

			/* Send report data */
			g_HID_ctrl.tx_out_busy = true;
			USBD_API->hw->WriteEP(g_HID_ctrl.hUsb, HID_EP_IN,
					&g_HID_ctrl.report_out_buf[0], MAX_REPORT_SIZE);
			return true;
		} else {
			return false;
		}
	} else {
		/* Reset busy flag if we get disconnected. */
		g_HID_ctrl.tx_out_busy = false;
		return false;
	}
}

/* EOF */
