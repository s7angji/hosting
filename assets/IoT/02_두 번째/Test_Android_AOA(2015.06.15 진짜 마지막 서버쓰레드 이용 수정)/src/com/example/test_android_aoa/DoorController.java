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

	// OTP ��� Ƚ��
	public int mOtpUsedNum;
	public String mOtp = "�ƹ����̳� ^^";
	Encrypt md5 = new Encrypt();

	public class Encrypt {
		// ���ڿ��� �޾ƿ� �ؽ��Ͽ� ������
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
				sendHandlerMessage("NoSuchAlgorithmException �߻���", MainActivity.DEBUG);
			}
			return encData; 
		}
	}

	// Thread ���Ḧ ���� flag
	private boolean running = false;

	// ��������� ���� ��ɾ�
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

	// �ܺ� ��ġ�� ����ó�� �д´�. �ȵ���̵嵵 �ᱹ �������� Ŀ���̴ϱ� ~!
	private ParcelFileDescriptor mFileDescriptor;
	private FileInputStream mInputStream;
	private FileOutputStream mOutputStream;

	// ������
	public DoorController(Handler hostHandler, UsbManager usbManager, UsbAccessory usbAccessory) {
		// �Ű������� �Ѿ� �� �ڵ鷯
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

		// �Ƶ��̳뿡�� �ʱ� OTP ���� �־�� �Ѵ� !!
		// ���� OTP�� ���� �� �� �̿��ߴ��� �˾Ƴ���
		sendCommand(CMD_REQUEST_OTP_USED_NUM, (byte)0, (byte)0);
		// �Ƶ��̳뿡���� ������ ���� 'OTP ��� Ƚ��'�� �Ѱ��ش�
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

	// SendMsgToDoorThread���� �̿��Ѵ�. �� �ʱ⿡ OTP ��� Ƚ���� �˾Ƴ��� ���ؼ� ^^
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
		byte[] buffer = new byte[16384]; // �ִ� ���� ������
		int i;

		while (running && (ret >= 0)) {
			try {
				ret = mInputStream.read(buffer);
			} catch (IOException e) {
				// ���� �и� ����ڰ� ������� �ڴ�� �������� ���� -_-
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