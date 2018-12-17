/**
 * 
 */
package fhtw.requester;

import fhtw.usb_hid.HID_Device;

/**
 * @author Markus Lechner
 *
 */
public class HID_Requester implements Runnable {
	
	private Thread th_HID_req = null;
	
	public HID_Requester() {
		th_HID_req = new Thread(this);
		th_HID_req.setName("HID_Requester");
		th_HID_req.setDaemon(false);	
	}
	
	public void start_HID_Requester() {
		th_HID_req.start();
	}

	@Override
	public void run() {
		HID_Device hid_dev = new HID_Device();
		hid_dev.start_HID_Device();
		
		while(true) {
			hid_dev.transmitPacket();
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}		
	}
}
