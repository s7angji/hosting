package com.example.test_android_aoa;

import java.io.FileNotFoundException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Iterator;
import java.util.LinkedList;



import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.hardware.Camera;
import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.MediaStore.Images.Media;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {
	static final String TAG = "DoorlookActivity.java";

	Button photo;
	// TODO ���߿� ��Ű���� �ٲ� �� ���� �ٶ�
	private static final String ACTION_USB_PERMISSION = "com.example.test_android_aoa.action.USB_PERMISSION";

	// proximity_alert ������ ������ ����ڰ� ������� �����ߴ��� �Ǵ��ϱ� ���� ��� ���� �ð��� �� �� �̳��� ����ȴ�.
	public boolean isProximityAlertTimeout; // // �ʹ� ������ �ɰ��ؼ� ��� �ð� �Ŀ� ������ �͵��� �����ϱ� ���� �÷���
	private Handler proximityAlertDecisionHandler = new Handler();
	private Runnable proximityAlertDecisionTask = new Runnable() {

		@Override
		public void run() {
			if(!Boolean.valueOf(getPre(PREFERENCES_IS_ENTERING))) { // "default" ������ onCreate() �� false�� �ʱ�ȭ �����ϱ� ���� ����
				isProximityAlertTimeout = true; // �ʹ� ������ �ɰ��ؼ� ��� �ð� �Ŀ� ������ �͵��� �����ϱ� ���� �÷���

				if(serverThread != null) {
					if(serverThread.isAlive()) {
						// ���� ������ ��� Ŭ���̾�Ʈ�鿡�� �� �� ���� �̿��ڰ� ���� �����Ѵٰ� �˸���.
						String suspiciousApproach = "suspicious_approach;";
						InputThread inputThread = new InputThread(serverThread, suspiciousApproach);
						inputThread.start();
					}
				}
			}
			else
				putPre(PREFERENCES_IS_ENTERING, "false");
		}

	};

	// ��Ʈ��ũ ��Ȳ�� ��ó�ϴ� �͵�
	NetworkStateReceiver networkStateReceiver;
	public ConnectivityManager cm = null;
	public WifiManager wifiManager = null;
	// Ŭ���̾�Ʈ�� ��ȣ�ۿ��� ����
	private NioChatterServer serverThread = null;
	// ���ڸ� �д� ���� �ٷ�
	private TextToSpeechController mTextToSpeechController = null;
	// GPS ��ġ ������ �ٷ�
	public LocationController mLocationController = null;
	// ��׶���� ������� �̾߱⸦ ������ ��
	private DoorController mDoorController = null;


	public static final String PREFERENCES_IS_ENTERING = "PREFERENCES_IS_ENTERING";
	public static final String PREFERENCES_OTP = "PREFERENCES_OTP";
	public static final String PREFERENCES_LATITUDE = "PREFERENCES_LATITUDE";
	public static final String PREFERENCES_LONGITUDE = "PREFERENCES_LONGITUDE";
	// ������ ������ Ŭ���̾�Ʈ�� IP��� �̸����� �����ؼ� ���� �޼����� �����Ѵ�
	public LinkedList<String> clientsName; // TODO : onDestroy() ���� �ش� ����� �޼����� �� �����
	public static final String PREFERENCES_OUT_MSG = "PREFERENCES_OUT_MSG";
	public static final String PREFERENCES_IN_MSG = "PREFERENCES_IN_MSG";

	public String getPre(String key) {
		synchronized (this) {
			SharedPreferences prefs = getSharedPreferences(key, MODE_PRIVATE);
			return prefs.getString(key, "default");	
		}
	}

	public void putPre(String key, String input) {
		synchronized (this) {
			SharedPreferences.Editor prefs = getSharedPreferences(key, MODE_PRIVATE).edit();
			prefs.putString(key, input);
			prefs.commit();
		}
	}

	public void removePre(String key) {
		synchronized (this) {
			SharedPreferences.Editor prefs = getSharedPreferences(key, MODE_PRIVATE).edit();
			prefs.remove(key);
			prefs.commit();
		}
	}

	private boolean registeredMessageSpeaking(String direction) {

		switch(direction) {
		case PREFERENCES_OUT_MSG:
			if(clientsName.isEmpty()) {
				return false;
			}
			else {
				if(mTextToSpeechController.hasTts()) {
					Iterator<String> i = clientsName.iterator();
					LinkedList<String> removeTmp = new LinkedList<String>();
					while(i.hasNext()) {
						String ip = i.next();
						String outMsg = getPre(ip + direction);
						if(outMsg.equals("default"))
							return false;
						mTextToSpeechController.playScript(outMsg);
						removePre(ip + direction);

						if(getPre(ip + PREFERENCES_IN_MSG).equals("default"))
							removeTmp.add(ip);
					}

					i = removeTmp.iterator();
					while(i.hasNext())
						clientsName.remove(i.next());

					return true;
				}
				else 
					return false;
			}
		case PREFERENCES_IN_MSG:
			if(clientsName.isEmpty()) {
				return false;
			}
			else {
				if(mTextToSpeechController.hasTts()) {
					Iterator<String> i = clientsName.iterator();
					LinkedList<String> removeTmp = new LinkedList<String>();
					while(i.hasNext()) {
						String ip = i.next();
						String inMsg = getPre(ip + direction);
						if(inMsg.equals("default"))
							return false;
						mTextToSpeechController.playScript(inMsg);
						removePre(ip + direction);

						if(getPre(ip + PREFERENCES_OUT_MSG).equals("default"))
							removeTmp.add(ip);
					}

					i = removeTmp.iterator();
					while(i.hasNext())
						clientsName.remove(i.next());

					return true;
				}
				else 
					return false;
			}
		default:
			return false;
		}

	}

	// ó���� �޼���
	public static final int CONNECTED_DOOR = 0; // ������� �����
	public static final int DISCONNECTED_DOOR = 1; // ������� ������
	public static final int OPEN_OUTSIDE_FROM_DOOR = 2; // �ۿ��� ���� ���� ��
	public static final int OPEN_INSIDE_FROM_DOOR = 3; // �ȿ��� ���� ���� ��
	public static final int FAIL_FROM_DOOR = 4; // �����ٰ� ���� ��. �� ������?
	public static final int START_INPUT_FROM_DOOR = 5; // ����ڰ� ���� ���� ���ؼ� �õ� ��
	public static final int SEND_OTP_USED_NUM_FROM_DOOR = 6; // OTP�� ��� Ƚ���� �˾Ƴ½��ϴ� !!

	public static final int FINISH_REQUEST_FROM_LOCATION = 7; // ����ڰ� ��ġ ������ ��� ���� ��� ��
	public static final int GET_LOCATION_FROM_LOCATION = 8; // ������� ���� ��ġ�� �˾Ƴ´� !!

	public static final int ON_DONE_FROM_TTS = 9; // TTS�� �ش� ���� �� ����
	public static final int NOT_SUPPORTED_LANGUAGE_FROM_TTS = 10; // TTS�� ���� �� �� �� ���� ��Ȳ�̴� �� ..

	public static final int GET_IP_SERVER = 12;
	public static final int FINISH_SERVER = 13; // ���� �����忡 �̻� �߻��� �����Ŵ
	public static final int DEBUG_SERVER = 14;

	public static final int DEBUG = 15; // �׳� �����
	// FROM_CLIENT
	public static final int IS_ENTERING_FROM_CLIENT = 16; // ������� �����溸�� �䱸�ؼ� ����ڰ� �� �� (1��)
	public static final int OUT_MSG_FROM_CLIENT = 17; // ����ڰ� ����� ���� �� �λ縻 (1��)
	public static final int IN_MSG_FROM_CLIENT = 18; // ����ڰ� ����� ���� �� �λ縻 (1��)
	public static final int REMOVE_MSG_FROM_CLIENT = 19; // ����ڰ� ����� �λ縻 ��� ���� (0��)
	public static final int REQUEST_OTP_FROM_CLIENT = 20; // ����ڰ� OTP �䱸 (0��)


	// �ý��ۿ��� UsbManager �����ò���
	public UsbManager mUsbManager;

	// �Ǽ��縮 �۹̼��� ��û �� �� ���� ~
	private PendingIntent mPermissionIntent;
	private boolean mPermissionRequestPending;

	/********** ī�޶� *********/

	private Context mContext = this;
	private Camera mCamera;
	private CameraPreview mPreview;
	FrameLayout camera_preview;
	public static Directory dir = new Directory();

	// UI �밡�� �� ��..
	ImageView num_btn1;
	ImageView num_btn2;
	ImageView num_btn3;
	ImageView num_btn4;
	ImageView num_btn5;
	ImageView num_btn6;
	ImageView num_btn7;
	ImageView num_btn8;
	ImageView num_btn9;
	ImageView num_btn0;
	ImageView num_cancel;
	ImageView num_ok;


	private TextView stateTv;
	private TextView otpTv;
	private TextView ipTv;
	private TextView debugTv;
	private Button startBtn;
	private Button finishBtn;

	// �ڵ鷯
	Handler mHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {

			InputThread inputThread;

			switch (msg.what) {
			case OPEN_OUTSIDE_FROM_DOOR:

				stateTv.setText("���� �ۿ��� ���Ƚ��ϴ�");
				if(registeredMessageSpeaking(PREFERENCES_IN_MSG)) // In Out �� ����� �����̶� ����� �����̶� �޶� ��¦ �ٲ�����
					break;
				else { // default ��
					if(mTextToSpeechController.hasTts())
						mTextToSpeechController.playScript("���� �;����. ȯ���մϴ�.");
					break;
				}

			case OPEN_INSIDE_FROM_DOOR:
				stateTv.setText("���� �ȿ��� ���Ƚ��ϴ�");
				if(registeredMessageSpeaking(PREFERENCES_OUT_MSG)) // In Out �� ����� �����̶� ����� �����̶� �޶� ��¦ �ٲ�����
					break;
				else { // default ��
					if(mTextToSpeechController.hasTts())
						mTextToSpeechController.playScript("�� �ٳ������. �޸��� �����ϼ���.");
					break;
				}

			case FAIL_FROM_DOOR:
				stateTv.setText("����ڰ� �߸��� ��й�ȣ�� �Է��߽��ϴ�");
				if(mTextToSpeechController.hasTts())
					mTextToSpeechController.playScript("���а� �ݺ��Ǹ� �Ű��ϰڽ��ϴ�");

				MytakePicture();
				break;

			case START_INPUT_FROM_DOOR:
				stateTv.setText("��й�ȣ �Է� ����");
				if(mTextToSpeechController.hasTts())
					mTextToSpeechController.playScript("��й�ȣ �Է� ����");

				MytakePicture();

				if(serverThread != null) {
					if(serverThread.isAlive()) {
						// ���� ������ ��� Ŭ���̾�Ʈ�鿡�� ������� �����ߴ��� �����.
						String proximityAlert = "proximity_alert;"; // ������� ������ �ִ��� �˷��޶�� ��Ź
						inputThread = new InputThread(serverThread, proximityAlert);
						inputThread.start();
					}
				}

				isProximityAlertTimeout = false;
				proximityAlertDecisionHandler.postDelayed(proximityAlertDecisionTask, 10000);

				break;

			case SEND_OTP_USED_NUM_FROM_DOOR:
				stateTv.setText("OTP�� ��� Ƚ���� �˾Ƴ½��ϴ� (" + (String)msg.obj + ")");

				mDoorController.mOtpUsedNum = Integer.parseInt((String)msg.obj);

				String otp = mDoorController.mOtp;
				for(int i = 0; i < mDoorController.mOtpUsedNum; i++) {
					otp = mDoorController.md5.encrypt(otp);
				}
				byte[] digest = otp.getBytes();
				if(digest.length < 4) {
					Toast.makeText(MainActivity.this, "�ؽ��� �Լ��� ���̰� �ʹ� ª���ϴ� !!", Toast.LENGTH_SHORT).show();
					return;
				}

				byte otpTmp[] = new byte[4];
				// �Ƶ��̳뿡�� ��¥ OTP�� �������ش� ^^ . . .
				for(byte otpIndex = 0; otpIndex < 4; otpIndex++) {
					byte command = DoorController.CMD_CHG_OPT;
					byte target = otpIndex; // index
					byte value;
					if(('0' <= digest[otpIndex]) && (digest[otpIndex] <= '9')) {
						value = digest[otpIndex];

						otpTmp[otpIndex] = value;
					}
					else{
						value = digest[otpIndex];
						value %= 100;
						value %= 10;
						value += '0';

						otpTmp[otpIndex] = value;
					}

					SendCmdToDoorThread t = new SendCmdToDoorThread(mDoorController, command, target, value);
					t.start();
				}

				// ���� OTP�� �����Ѵ�.
				putPre(PREFERENCES_OTP, new String(otpTmp));
				otpTv.setText(getPre(PREFERENCES_OTP));

				if(serverThread != null) {
					if(serverThread.isAlive()) {
						// ���� ������ ��� Ŭ���̾�Ʈ�鿡�� �ٲ� OTP ������ �˷��ش�.
						String otpChg = "otp;" + getPre(PREFERENCES_OTP) + ";";
						inputThread = new InputThread(serverThread, otpChg);
						inputThread.start();
					}
				}

				break;

			case GET_LOCATION_FROM_LOCATION:

				// (String)msg.obj = "�ʿ��� ��ġ ������ ������ϴ�.\n�� �̻��� ��ġ ������ �������� �ʽ��ϴ�."
				Toast.makeText(MainActivity.this, (String)msg.obj, Toast.LENGTH_SHORT).show();

				if(mLocationController.hasLocation()) {
					putPre(PREFERENCES_LATITUDE, String.valueOf(mLocationController.mLocation.getLatitude()));
					putPre(PREFERENCES_LONGITUDE, String.valueOf(mLocationController.mLocation.getLongitude()));
				}

				if(serverThread != null) {
					if(serverThread.isAlive()) {
						// ���� ������ ��� Ŭ���̾�Ʈ�鿡�� ����� ��ġ�� �˷��ش�.
						String location = "door_location;" + getPre(PREFERENCES_LATITUDE) + ";"
								+ getPre(PREFERENCES_LONGITUDE) + ";";
						inputThread = new InputThread(serverThread, location);
						inputThread.start();
					}
				}
				break;

			case FINISH_REQUEST_FROM_LOCATION:
				Toast.makeText(MainActivity.this, (String)msg.obj, Toast.LENGTH_SHORT).show();
				finish();
				break;

			case ON_DONE_FROM_TTS:
				// no-op
				// stateTv.setText((String)msg.obj);
				break;

			case NOT_SUPPORTED_LANGUAGE_FROM_TTS:
				stateTv.setText((String)msg.obj);
				Toast.makeText(MainActivity.this, (String)msg.obj, Toast.LENGTH_SHORT).show();
				finish();
				break;

			case DISCONNECTED_DOOR:
				synchronized (this) {
					if(mDoorController != null) {
						if(mDoorController.isAlive()) { mDoorController.quit(); }
						mDoorController = null;
					}
					stateTv.setText((String)msg.obj);
					Toast.makeText(MainActivity.this, "����� ���񽺰� �������ϴ�.", Toast.LENGTH_SHORT).show();
					finish();
				}
				break;
			case GET_IP_SERVER:
				ipTv.setText((String)msg.obj);
				break;
			case FINISH_SERVER:
				synchronized(this) {
					if(serverThread != null) {
						if(serverThread.isAlive()) {
							serverThread.quit();
							serverThread = null;

							debugTv.setText((String)msg.obj);
							Toast.makeText(MainActivity.this, "������ ����Ǿ����ϴ�", Toast.LENGTH_SHORT).show();
						}
					}
				}
				break;
			case DEBUG_SERVER:
				StringBuilder s = new StringBuilder();
				s.append(debugTv.getText());
				s.append((String) msg.obj + "\n");
				debugTv.setText(s.toString());

				break;
			case IS_ENTERING_FROM_CLIENT: // ������� �����溸�� �䱸�ؼ� ����ڰ� �� �� (1��)
				// no-op NioChatterServer�� SheardPreferences�� �־� ��
				break;
			case OUT_MSG_FROM_CLIENT: // ����ڰ� ����� ���� �� �λ縻 (1��)
				// no-op NioChatterServer�� SheardPreferences�� �־� ��
				break;
			case IN_MSG_FROM_CLIENT: // ����ڰ� ����� ���� �� �λ縻 (1��)
				// no-op NioChatterServer�� SheardPreferences�� �־� ��
				break;
			case REMOVE_MSG_FROM_CLIENT: // ����ڰ� ����� �λ縻 ��� ���� (0��)
				// no-op NioChatterServer�� SheardPreferences���� ����
				break;
			case REQUEST_OTP_FROM_CLIENT: // ����ڰ� OTP �䱸 (0��)

				if(serverThread != null) {
					if(serverThread.isAlive()) {
						// ���� ������ ��� Ŭ���̾�Ʈ�鿡�� OTP ������ �˷��ش�.
						String otpRequest = "otp;" + getPre(PREFERENCES_OTP) + ";";
						inputThread = new InputThread(serverThread, otpRequest);
						inputThread.start();
					}
				}
				break;

			case CONNECTED_DOOR:
			case DEBUG:
				stateTv.setText((String)msg.obj);
				break;
			default:
				break;
			}
			super.handleMessage(msg);
		}
	};

	// ��ε�ĳ��Ʈ���ù�
	private final BroadcastReceiver mDoorReceiver = new DoorReceiver();
	class DoorReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {

			String action = intent.getAction(); // ���� �׼��̳�?

			if(ACTION_USB_PERMISSION.equals(action)) {

				synchronized (this) {

					UsbAccessory accessory = (UsbAccessory)intent.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
					boolean hasPermission = intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false);
					if(hasPermission) {
						// ����� ������ ���� !!!
						if(mDoorController == null) {
							mDoorController = new DoorController(mHandler, mUsbManager, accessory);
							mDoorController.start();
						}
					}
					else {
						Log.d(TAG, "permission denied for accessory " + accessory);
						Toast.makeText(MainActivity.this, "permission denied for accessory ", Toast.LENGTH_SHORT).show();
					}

					// �ش� �۹̼� �䱸�� ó�� �����ϱ� false ~!!
					mPermissionRequestPending = false;
				}
			}
			else if(UsbManager.ACTION_USB_ACCESSORY_DETACHED .equals(action)) {
				//if((accessory != null) && accessory.equals(mUsbAccessory)) //accessory ��� �ڵ尡 �� ����
				synchronized (this) {
					if(mDoorController != null) {
						if(mDoorController.isAlive()) { mDoorController.quit(); }
						mDoorController = null;
					}
					Toast.makeText(MainActivity.this, "������� ������ �����Ǿ����ϴ�.", Toast.LENGTH_SHORT).show();
					finish();
				}
			}
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.activity_main);

		/****** Widget Instance Reference ******/

		num_btn1 = (ImageView) findViewById(R.id.num1);
		num_btn2 = (ImageView) findViewById(R.id.num2);
		num_btn3 = (ImageView) findViewById(R.id.num3);
		num_btn4 = (ImageView) findViewById(R.id.num4);
		num_btn5 = (ImageView) findViewById(R.id.num5);
		num_btn6 = (ImageView) findViewById(R.id.num6);
		num_btn7 = (ImageView) findViewById(R.id.num7);
		num_btn8 = (ImageView) findViewById(R.id.num8);
		num_btn9 = (ImageView) findViewById(R.id.num9);
		num_btn0 = (ImageView) findViewById(R.id.num0);
		num_cancel = (ImageView) findViewById(R.id.num11);
		num_ok = (ImageView) findViewById(R.id.num10);

		photo = (Button) findViewById(R.id.captureList);


		stateTv = (TextView)findViewById(R.id.stateTv);
		otpTv = (TextView)findViewById(R.id.otpTv);
		ipTv = (TextView)findViewById(R.id.ipTv);
		debugTv = (TextView)findViewById(R.id.debugTv);
		startBtn = (Button)findViewById(R.id.startBtn);
		finishBtn = (Button)findViewById(R.id.finishBtn);

		/****** AOA Init ******/
		mUsbManager = (UsbManager)getSystemService(Context.USB_SERVICE);

		mPermissionIntent = PendingIntent.getBroadcast(this, 0, new Intent(ACTION_USB_PERMISSION), 0);
		IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
		filter.addAction(UsbManager.ACTION_USB_ACCESSORY_DETACHED);
		registerReceiver(mDoorReceiver, filter);

		if (mDoorController == null) {
			stateTv.setText("Please connect the accessory ..");
		}

		/****** Location Init ******/
		mLocationController = new LocationController(this);

		/****** TextToSpeech Init ******/
		mTextToSpeechController = new TextToSpeechController(this);

		/****** Server Init ******/
		// TODO : ������� ������ ����� �ִ��� �Ǵ��� ��й�ȣ �Է� ���۽ÿ� ��� ��
		// ������ ����� �־�� TRUE�� �ȴ� ~!!
		putPre(PREFERENCES_IS_ENTERING, "false");
		isProximityAlertTimeout = true;

		// ���� setEnabled
		startBtn.setEnabled(true);
		finishBtn.setEnabled(false);
		// ��Ʈ��ũ ���¿� ��ó�ϴ� BroadcastReceiver ���
		IntentFilter intentfilter = new IntentFilter();
		intentfilter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
		intentfilter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
		networkStateReceiver = new NetworkStateReceiver();
		registerReceiver(networkStateReceiver, intentfilter);

		// ī�޶�
		camera_preview = (FrameLayout) findViewById(R.id.camera_preview);
		setCamera();

		// Ŭ���̾�Ʈ�� ���� �޼��� ~!!
		clientsName = new LinkedList<String>();

		photo.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {

				Intent i = new Intent(MainActivity.this, view_Photo.class);
				startActivity(i);

			}
		});
	}



	public void MytakePicture() {

		if(mPreview.check == false)
			mPreview.check = true; // flag ����

		long time = System.currentTimeMillis();
		SimpleDateFormat dayTime = new SimpleDateFormat(
				" �⵵ : yyyy��\n ��¥ : MM��dd��\n �ð� : a hh��mm��");
		String primary_name = dayTime.format(new Date(time));
		System.out.println(primary_name);
		String sortout = "SmartDoorLock";

		ContentValues mContentValues = new ContentValues(3);
		mContentValues.put(Media.DISPLAY_NAME, primary_name);
		mContentValues.put(Media.DESCRIPTION, sortout);

		Uri imageFileUri = getContentResolver().insert(
				Media.EXTERNAL_CONTENT_URI, mContentValues);

		try {
			dir.fileUri = getContentResolver().openOutputStream(
					imageFileUri);
			Toast.makeText(MainActivity.this, "take picture",
					Toast.LENGTH_LONG).show();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

	public void setCamera() {

		mContext = this;
		// ī�޶� �ν��Ͻ� ����
		if (checkCameraHardware(mContext)) {
			mCamera = getCameraInstance();

			// ������â�� �����ϰ� ��Ƽ��Ƽ�� ���̾ƿ����� �����մϴ�
			mPreview = new CameraPreview(this, mCamera);

			camera_preview.addView(mPreview); // preview ȭ���� �ǰ���.


		} else {
			Toast.makeText(mContext, "no camera on this device!",
					Toast.LENGTH_SHORT).show();
		}

	}
	/** ī�޶� �ϵ���� ���� ���� Ȯ�� */
	@SuppressLint("NewApi")
	private boolean checkCameraHardware(Context context) {
		if (context.getPackageManager().hasSystemFeature(
				PackageManager.FEATURE_CAMERA)) {
			// ī�޶� �ּ��� �Ѱ� �ִ� ��� ó��
			Log.i("SETX",
					"Number of available camera : "
							+ Camera.getNumberOfCameras());
			return true;
		} else {
			// ī�޶� ���� ���� ���
			Toast.makeText(mContext, "No camera found!", Toast.LENGTH_SHORT)
			.show();
			return false;
		}
	}
	/** ī�޶� �ν��Ͻ��� �����ϰ� ȹ���մϴ� */
	@SuppressLint("NewApi")
	public static Camera getCameraInstance() {
		Camera c = null;
		try {
			c = Camera.open(0);
		} catch (Exception e) {
			// ������̰ų� ��� �Ұ��� �� ���
		}
		return c;
	}

	// GPS�� ������ ���� ����ڿ��Լ� �۹̼� ��� ���� ������ �Ϻ�
	public static final int REQUEST_GPS_AT_ON_ACTIVITY_RESULT = 44;
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		switch(requestCode) {
		case REQUEST_GPS_AT_ON_ACTIVITY_RESULT:
			if(!mLocationController.requestLocation(false)) {
				Toast.makeText(this, "GPS Not Enabled.", Toast.LENGTH_SHORT).show();
				finish();
			}
			break;
		default:
			Toast.makeText(this, "I don't know.. requestCode", Toast.LENGTH_SHORT).show();
			break;
		}
	}

	@Override
	public void onResume() {
		super.onResume();

		UsbAccessory[] accessories = mUsbManager.getAccessoryList();
		UsbAccessory accessory = (accessories == null ? null : accessories[0]);
		if (accessory != null) {
			if(mUsbManager.hasPermission(accessory)) {
				// ����� ������ ���� !!!
				if(mDoorController == null) {
					mDoorController = new DoorController(mHandler, mUsbManager, accessory);
					mDoorController.start();
				}
			}
			else { // �Ǽ��縮�� �ֱ��ѵ� �۹̼��� �� ��´ٸ�.. �۹̼� ��û
				synchronized (mDoorReceiver) {
					if (!mPermissionRequestPending) {
						mUsbManager.requestPermission(accessory, mPermissionIntent);

						mPermissionRequestPending = true;
					}
				}
			}
		}
		else {
			Log.d(TAG, "Accessory is null");
		}
	}

	@Override
	public void onPause() {
		super.onPause();
	}

	@Override
	public void onDestroy() {
		// ���� �Ф� ������ �Ǽ� ���� �Ǵ°ǵ� �� �װ��� ���ҷ��� ??
		isProximityAlertTimeout = true;
		proximityAlertDecisionHandler.removeCallbacks(proximityAlertDecisionTask);

		if(mDoorController != null) {
			if(mDoorController.isAlive()) { mDoorController.quit(); }
			mDoorController = null;
		}
		unregisterReceiver(mDoorReceiver);

		mLocationController.release();
		mTextToSpeechController.release();

		removePre(PREFERENCES_IS_ENTERING);
		removePre(PREFERENCES_LATITUDE);
		removePre(PREFERENCES_LONGITUDE);
		removePre(PREFERENCES_OTP);
		if(!clientsName.isEmpty()) {
			Iterator<String> i = clientsName.iterator();
			while(i.hasNext()) {
				String ip = i.next();
				removePre(ip + PREFERENCES_OUT_MSG);
				removePre(ip + PREFERENCES_IN_MSG);
				clientsName.remove(ip);
			}
		}

		synchronized(this) {
			if(serverThread != null) {
				if(serverThread.isAlive()) {
					serverThread.quit();
					serverThread = null;
				}
				else
					serverThread = null;
			}
			unregisterReceiver(networkStateReceiver);	
		}

		super.onDestroy();
	}

	// ���� ���� ��ư
	public void onStratServer(View v) {
		NetworkStateReceiver ns = new NetworkStateReceiver();
		if(ns.getConnectivityStatus(v.getContext()) != 
				NetworkStateReceiver.WIFI_CONNECT) {
			Toast.makeText(v.getContext(), "wifi ��Ȳ������ �����մϴ�.\nwifi �� �� �ٽ� �õ����ּ���.", Toast.LENGTH_SHORT).show();
			return; // -END-
		}

		if(serverThread == null) {
			// ��������ä���� ������� �۵���Ų��.
			serverThread = new NioChatterServer(MainActivity.this);
			serverThread.start();
			debugTv.setText("** ������ ���۵Ǿ����ϴ�.\n");

			// ���� setEnabled
			startBtn.setEnabled(false);
			finishBtn.setEnabled(true);
		} else {
			Toast.makeText(v.getContext(), "������ �̹�!!! ����ǰ� �ֽ��ϴ�.", Toast.LENGTH_SHORT).show();
		}
	}

	// ���� ���� ��ư
	public void onFinshServer(View v) {
		if(serverThread != null) {
			if(serverThread.isAlive()) {
				serverThread.quit();
				serverThread = null;
				debugTv.setText("** ������ �����մϴ�.\n");

				// ���� setEnabled
				startBtn.setEnabled(true);
				finishBtn.setEnabled(false);

				Toast.makeText(v.getContext(), "������ �����մϴ�.", Toast.LENGTH_SHORT).show();
			}
		}
	}
	// TODO : ��й�ȣ ���� ���� �ε��� ���߿� ������ ��
	byte mPwIndex = 0x0;
	// ���̿� �ڵ�
	static int getPassNum=0;
	public void onChangePW(View v) {
		byte num = 0;
		switch(v.getId()) {
		case R.id.num1:
			num = '1';
			break;
		case R.id.num2:
			num = '2';
			break;
		case R.id.num3:
			num = '3';
			break;
		case R.id.num4:
			num = '4';
			break;
		case R.id.num5:
			num = '5';
			break;
		case R.id.num6:
			num = '6';
			break;
		case R.id.num7:
			num = '7';
			break;
		case R.id.num8:
			num = '8';
			break;
		case R.id.num9:
			num = '9';
			break;
		case R.id.num0:
			num = '0';
			break;
		default:
			Toast.makeText(this, "onChangePW Error", Toast.LENGTH_SHORT).show();
			return;
		}

		if(mDoorController != null) {
			byte command = DoorController.CMD_CHG_PW;
			byte target = mPwIndex++; // index
			if(mPwIndex == 0x4)
				mPwIndex = 0x0;
			byte value = num;
			SendCmdToDoorThread t = new SendCmdToDoorThread(mDoorController, command, target, value);
			t.start();
			getPassNum++;
			if(getPassNum==4){
				Toast.makeText(MainActivity.this, "���� �Ϸ�", Toast.LENGTH_SHORT).show();
				getPassNum=0;
			}
		}
	}

	class NetworkStateReceiver extends BroadcastReceiver {
		private static final int WIFI_CONNECT = 1;
		private static final int MOBILE_CONNECT = 2;
		private static final int NOTHING = 3;

		@Override
		public void onReceive(Context context, Intent intent) {

			String IntentAction = intent.getAction();

			if(IntentAction.equals(ConnectivityManager.CONNECTIVITY_ACTION)
					|| IntentAction.equals(WifiManager.WIFI_STATE_CHANGED_ACTION)) {

				switch(getConnectivityStatus(context)) {

				case WIFI_CONNECT:
					if(serverThread == null) {
						// ��������ä���� ������� �۵���Ų��.
						serverThread = new NioChatterServer(MainActivity.this);
						serverThread.start();
						debugTv.setText("** ������ ���۵Ǿ����ϴ�.\n");

						// ���� setEnabled
						startBtn.setEnabled(false);
						finishBtn.setEnabled(true);
					} else {
						return;
					}
					break;

				case MOBILE_CONNECT:
				case NOTHING:
					if(serverThread != null) {
						if(serverThread.isAlive()) {
							serverThread.quit();
							serverThread = null;
							debugTv.setText("** ������ �����մϴ�.\n");

							// ���� setEnabled
							startBtn.setEnabled(true);
							finishBtn.setEnabled(false);
						}
						else
							serverThread = null;
					}
					break;

				default:
					Toast.makeText(context, "I don't know", Toast.LENGTH_SHORT).show();
				}
			}
		}

		// ��Ʈ��ũ ������ ����� ��, Wifi, Mobile, Nothing ���� �ľ�
		public  int getConnectivityStatus(Context context) {
			ConnectivityManager cm = (ConnectivityManager)context.getSystemService(Context.CONNECTIVITY_SERVICE);
			NetworkInfo activeNetwork = cm.getActiveNetworkInfo();

			if(activeNetwork == null)
				return NOTHING;

			switch(activeNetwork.getType()){
			case ConnectivityManager.TYPE_WIFI:
				if(activeNetwork.getState() == NetworkInfo.State.CONNECTED)
					return WIFI_CONNECT;
				break;
			case ConnectivityManager.TYPE_MOBILE:
				if(activeNetwork.getState() == NetworkInfo.State.CONNECTED)
					return MOBILE_CONNECT;
				break;
			}
			return NOTHING;
		}
	}
}