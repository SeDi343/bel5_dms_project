/**
 * 
 */
package fhtw.signal_generator;

import fhtw.requester.HID_Requester;

/**
 * @author Markus Lechner
 *
 */
public class Main {
	

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		HID_Requester hid_Requester = new HID_Requester();
		hid_Requester.start_HID_Requester();
		System.out.println("USB-HID example is running...");
	}
}

/* EOF */