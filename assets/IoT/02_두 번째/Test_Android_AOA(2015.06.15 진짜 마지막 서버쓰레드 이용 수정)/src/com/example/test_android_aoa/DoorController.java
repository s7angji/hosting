package com.example.test_android_aoa;

import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbManager;
import android.os.Handler;
import android.os.Message;
import android.os.ParcelFileDescriptor;
import android.util.Log;

public class DoorController extends Thread {
	static final String TAG = "DoorController.java";

	// OTP 사용 횟수
	public int mOtpUsedNum;
	public String mOtp = "아무값이나 ^^";
	Encrypt md5 = new Encrypt();

	public class Encrypt {
		// 문자열을 받아와 해쉬하여 돌려줌
		public String encrypt(String str){
			String encData = "";

			try {
				MessageDigest md5 = MessageDigest.getInstance("MD5");
				byte[] bytes = str.getBytes();
				md5.update(bytes);
				byte[] digest = md5.digest();

				for(int i = 0; i < digest.length; i++) {
					encData += Integer.toHexString(digest[i] & 0xff);
				}
			} catch (NoSuchAlgorithmException e) {
				e.printStackTrace();
				sendHandlerMessage("NoSuchAlgorithmException 발생함", MainActivity.DEBUG);
			}
			return encData; 
		}
	}

	// Thread 종료를 위한 flag
	private boolean running = false;

	// 도어락에게 내릴 명령어
	public static final byte CMD_CHG_PW = 0x7;
	public static final byte CMD_CHG_OPT = 0x8;
	public static final byte CMD_REQUEST_OTP_USED_NUM = 0x9;

	private static final int STATE_DEBUG = 0x0;
	private static final int STATE_START_INPUT = 0x1;
	private static final int STATE_OPEN_OUTSIDE = 0x2;
	private static final int STATE_OPEN_INSIDE = 0x3;
	private static final int STATE_FAIL = 0x4;
	private static final int STATE_SEND_OTP_USED_NUM = 0x5;

	protected Handler hostHandler;

	public UsbManager mUsbManager;
	public UsbAccessory mUsbAccessory;

	// 외부 장치도 파일처럼 읽는다. 안드로이드도 결국 리눅스가 커널이니깐 ~!
	private ParcelFileDescriptor mFileDescriptor;
	private FileInputStream mInputStream;
	private FileOutputStream mOutputStream;

	// 생성자
	public DoorController(Handler hostHandler, UsbManager usbManager, UsbAccessory usbAccessory) {
		// 매개변수로 넘어 온 핸들러
		this.hostHandler = hostHandler;
		this.mUsbManager = usbManager;

		// OPEN ACCESSORY!!!
		mFileDescriptor = mUsbManager.openAccessory(usbAccessory);
		if(mFileDescriptor != null) {

			this.mUsbAccessory = usbAccessory;

			FileDescriptor fd = mFileDescriptor.getFileDescriptor();
			mInputStream = new FileInputStream(fd);
			mOutputStream = new FileOutputStream(fd);

			Log.d(TAG, "accessory opened");
			sendHandlerMessage("accessory opened", MainActivity.CONNECTED_DOOR);

			running = true;
		}
		else {
			Log.d(TAG, "accessory open fail");
			sendHandlerMessage("accessory open fail", MainActivity.DISCONNECTED_DOOR);
			quit();
		}

		// 아두이노에게 초기 OTP 값을 주어야 한다 !!
		// 따라서 OTP를 지금 몇 번 이용했는지 알아낸다
		sendCommand(CMD_REQUEST_OTP_USED_NUM, (byte)0, (byte)0);
		// 아두이노에서는 다음과 같이 'OTP 사용 횟수'를 넘겨준다
		//else if(msg0 == CMD_REQUEST_OTP_USED_NUM) {
		// 	sendStateToAndroid(STATE_SEND_OTP_USED_NUM, (uint16_t)EEPROM.read(5));
		//}
	}

	private int composeInt(byte hi, byte lo) {
		int val = (int) hi & 0xff;
		val *= 256;
		val += (int) lo & 0xff;
		return val;
	}

	// SendMsgToDoorThread에서 이용한다. 또 초기에 OTP 사용 횟수를 알아내기 위해서 ^^
	public void sendCommand(byte command, byte target, byte value) {
		byte[] buffer = new byte[3];
		//if (value > 127)
		//	value = 127;

		buffer[0] = command;
		buffer[1] = target;
		buffer[2] = (byte) value;
		if (mOutputStream != null && buffer[1] != -1) {
			try {
				mOutputStream.write(buffer);
			} catch (IOException e) {
				Log.e(TAG, "write failed", e);
			}
		}
	}

	public void sendHandlerMessage(String msg, int what) {
		Message m = new Message();
		m.what = what;
		m.obj = msg;
		hostHandler.sendMessage(m);
	}

	@Override
	public void run() {

		int ret = 0;
		byte[] buffer = new byte[16384]; // 최대 버퍼 사이즈
		int i;

		while (running && (ret >= 0)) {
			try {
				ret = mInputStream.read(buffer);
			} catch (IOException e) {
				// 지금 분명 사용자가 도어락을 멋대로 뽑은거임 히밤 -_-
				break;
			}

			i = 0;
			while (i < ret) {
				int len = ret - i;

				switch (buffer[i]) {

				case STATE_DEBUG:
					if (len >= 3) {
						sendHandlerMessage(new Integer(composeInt(buffer[i + 1], buffer[i + 2])).toString(),
								MainActivity.DEBUG);
					}
					i += 3;
					break;

				case STATE_START_INPUT:
					if (len >= 3) {
						sendHandlerMessage(new Integer(composeInt(buffer[i + 1], buffer[i + 2])).toString(),
								MainActivity.START_INPUT_FROM_DOOR);
					}
					i += 3;
					break;

				case STATE_OPEN_OUTSIDE:
					if (len >= 3) {
						sendHandlerMessage(new Integer(composeInt(buffer[i + 1], buffer[i + 2])).toString(),
								MainActivity.OPEN_OUTSIDE_FROM_DOOR);
					}
					i += 3;
					break;

				case STATE_OPEN_INSIDE:
					if (len >= 3) {
						sendHandlerMessage(new Integer(composeInt(buffer[i + 1], buffer[i + 2])).toString(),
								MainActivity.OPEN_INSIDE_FROM_DOOR);
					}
					i += 3;
					break;

				case STATE_FAIL:
					if (len >= 3) {
						sendHandlerMessage(new Integer(composeInt(buffer[i + 1], buffer[i + 2])).toString(),
								MainActivity.FAIL_FROM_DOOR);
					}
					i += 3;
					break;
				case STATE_SEND_OTP_USED_NUM:
					if (len >= 3) {
						sendHandlerMessage(new Integer(composeInt(buffer[i + 1], buffer[i + 2])).toString(),
								MainActivity.SEND_OTP_USED_NUM_FROM_DOOR);
					}
					i += 3;
					break;
				default:
					Log.d(TAG, "unknown msg: " + buffer[i]);
					i = len;
					break;
				}
			}
		}

		sendHandlerMessage("Please connect the accessory (DETACHED)", MainActivity.DISCONNECTED_DOOR);
		quit();
	}

	public void quit() {

		// sendHandlerMessage("Please connect the accessory (Thread quit())", MainActivity.DISCONNECTED_DOOR);

		try {
			if (mFileDescriptor != null) {
				mFileDescriptor.close();
			}
		}
		catch (IOException e) {
			Log.e(TAG, "Error closing file", e);
		}
		finally {
			mFileDescriptor = null;
			mUsbAccessory = null;
		}

		running = false;
		interrupt();
	}
}