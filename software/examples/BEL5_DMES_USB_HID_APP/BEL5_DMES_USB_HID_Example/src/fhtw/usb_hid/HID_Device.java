/**
 * 
 */
package fhtw.usb_hid;



import java.util.List;

import purejavahidapi.DeviceRemovalListener;
import purejavahidapi.HidDevice;
import purejavahidapi.HidDeviceInfo;
import purejavahidapi.InputReportListener;
import purejavahidapi.PureJavaHidApi;

/**
 * @author Markus Lechner
 *
 */
public class HID_Device implements Runnable {
	private final short HID_PRODUCT_ID = 0x0081;
	private final String HID_SERIAL_NR = ("OpenLab-Multimeter");

	private boolean hid_dev_opened = false;
	private HidDevice hid_device = null;
	private HidDeviceInfo hidDevInfo = null;
	private Thread th_HID_device = null;


	/**
	 * 
	 */
	public HID_Device() {
		th_HID_device = new Thread(this);
		th_HID_device.setName("HID_Device");
		th_HID_device.setDaemon(true);		
	}

	public void start_HID_Device() {
		th_HID_device.start();
	}


	@Override
	public void run() {
		List<HidDeviceInfo> hidDevList = null;

		while(true) {
			try {
				if(!hid_dev_opened && (hid_device == null)) {
					hidDevList = PureJavaHidApi.enumerateDevices();

					for (HidDeviceInfo hid_info : hidDevList) {
						if (HID_SERIAL_NR.equals(hid_info.getSerialNumberString()) && (HID_PRODUCT_ID == hid_info.getProductId())) {
							hidDevInfo = hid_info;
							System.out.println("USB-HID device found! Product ID: " + Integer.toHexString(hid_info.getProductId()));
							break;
						}			
					}

					if(hidDevInfo == null) {					
						Thread.sleep(100);
					} else {
						hid_device = PureJavaHidApi.openDevice(hidDevInfo);

						hid_device.setDeviceRemovalListener(new DeviceRemovalListener() {
							@Override
							public void onDeviceRemoval(HidDevice source) {
								hid_device = null;
								hidDevInfo = null;
								hid_dev_opened = false;
								System.out.println("Connection to USB-HID device lost!");
							}
						});

						hid_device.setInputReportListener(new InputReportListener() {
							@Override
							public void onInputReport(HidDevice source, byte Id, byte[] data, int len) {
								String recData = new String(data);
								
								System.out.println("Received packet at ID: " + Id + ", Packet size: " + 
										len + ", Content: " + recData);
							} 
						});

						hid_dev_opened = true;
					}

				} else {
					Thread.sleep(10);
				}
			} catch (Throwable  e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}

	public boolean transmitPacket() {
		//String hid_data = new String("SignalGenerator");
		
		// hardcoded signal information to test the setup $[signal-type]:[frequency]:[phase]#
		String hid_data = new String("$sin:2000:0#"); 

		if(hid_device == null) {
			return false;
		}

		hid_device.setOutputReport((byte)0, hid_data.getBytes(), hid_data.getBytes().length);
		return true;
	}
}

/* EOF */
