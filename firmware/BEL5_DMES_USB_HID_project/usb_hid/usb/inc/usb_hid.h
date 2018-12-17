/*
 * hid_main.h
 *
 *  Created on: 08.11.2017
 *      Author: Markus Lechner
 */

#ifndef USB_HID_USB_INC_USB_HID_H_
#define USB_HID_USB_INC_USB_HID_H_
/*********************************************************************FUNCTION PROTOTYPE*/
int usb_config_hid_device(void);
void my_usb_hid_receive(uint8_t *data_ptr);
bool my_usb_hid_transmit(uint8_t *data_ptr);
/**********************************************************************/
#endif /* USB_HID_USB_INC_USB_HID_H_ */

/* EOF */
