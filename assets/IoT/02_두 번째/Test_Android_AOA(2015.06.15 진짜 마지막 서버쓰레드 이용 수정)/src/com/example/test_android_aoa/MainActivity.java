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
	// TODO 나중에 패키지명 바꿀 때 수정 바람
	private static final String ACTION_USB_PERMISSION = "com.example.test_android_aoa.action.USB_PERMISSION";

	// proximity_alert 때문에 접속한 사용자가 도어락에 근접했는지 판단하기 위해 어느 정도 시간을 둔 후 이놈이 실행된다.
	public boolean isProximityAlertTimeout; // // 너무 지연이 심각해서 어느 시간 후에 들어오는 것들은 무시하기 위한 플래그
	private Handler proximityAlertDecisionHandler = new Handler();
	private Runnable proximityAlertDecisionTask = new Runnable() {

		@Override
		public void run() {
			if(!Boolean.valueOf(getPre(PREFERENCES_IS_ENTERING))) { // "default" 걱정은 onCreate() 때 false로 초기화 했으니까 걱정 ㄴㄴ
				isProximityAlertTimeout = true; // 너무 지연이 심각해서 어느 시간 후에 들어오는 것들은 무시하기 위한 플래그

				if(serverThread != null) {
					if(serverThread.isAlive()) {
						// 현재 접속한 모든 클라이언트들에게 알 수 없는 이용자가 문을 열려한다고 알린다.
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

	// 네트워크 상황에 대처하는 것들
	NetworkStateReceiver networkStateReceiver;
	public ConnectivityManager cm = null;
	public WifiManager wifiManager = null;
	// 클라이언트와 상호작용할 서버
	private NioChatterServer serverThread = null;
	// 글자를 읽는 것을 다룸
	private TextToSpeechController mTextToSpeechController = null;
	// GPS 위치 정보를 다룸
	public LocationController mLocationController = null;
	// 백그라운드로 도어락과 이야기를 나누는 놈
	private DoorController mDoorController = null;


	public static final String PREFERENCES_IS_ENTERING = "PREFERENCES_IS_ENTERING";
	public static final String PREFERENCES_OTP = "PREFERENCES_OTP";
	public static final String PREFERENCES_LATITUDE = "PREFERENCES_LATITUDE";
	public static final String PREFERENCES_LONGITUDE = "PREFERENCES_LONGITUDE";
	// 서버에 접속한 클라이언트는 IP라는 이름으로 구분해서 각각 메세지를 저장한다
	public LinkedList<String> clientsName; // TODO : onDestroy() 에서 해당 저장된 메세지는 다 지운다
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

	// 처리할 메세지
	public static final int CONNECTED_DOOR = 0; // 도어락이 연결됨
	public static final int DISCONNECTED_DOOR = 1; // 도어락이 해제됨
	public static final int OPEN_OUTSIDE_FROM_DOOR = 2; // 밖에서 문이 열릴 때
	public static final int OPEN_INSIDE_FROM_DOOR = 3; // 안에서 문이 열릴 때
	public static final int FAIL_FROM_DOOR = 4; // 문열다가 실패 함. 넌 누구냐?
	public static final int START_INPUT_FROM_DOOR = 5; // 사용자가 문을 열기 위해서 시도 함
	public static final int SEND_OTP_USED_NUM_FROM_DOOR = 6; // OTP의 사용 횟수를 알아냈습니다 !!

	public static final int FINISH_REQUEST_FROM_LOCATION = 7; // 사용자가 위치 정보를 얻기 전에 취소 함
	public static final int GET_LOCATION_FROM_LOCATION = 8; // 도어락의 현재 위치를 알아냈다 !!

	public static final int ON_DONE_FROM_TTS = 9; // TTS가 해당 글을 다 읽음
	public static final int NOT_SUPPORTED_LANGUAGE_FROM_TTS = 10; // TTS를 전혀 쓸 수 가 없는 상황이다 ㅠ ..

	public static final int GET_IP_SERVER = 12;
	public static final int FINISH_SERVER = 13; // 서버 스레드에 이상 발생시 종료시킴
	public static final int DEBUG_SERVER = 14;

	public static final int DEBUG = 15; // 그냥 디버그
	// FROM_CLIENT
	public static final int IS_ENTERING_FROM_CLIENT = 16; // 도어락이 근접경보를 요구해서 사용자가 준 것 (1개)
	public static final int OUT_MSG_FROM_CLIENT = 17; // 사용자가 등록한 나갈 때 인사말 (1개)
	public static final int IN_MSG_FROM_CLIENT = 18; // 사용자가 등록한 들어올 때 인사말 (1개)
	public static final int REMOVE_MSG_FROM_CLIENT = 19; // 사용자가 등록한 인사말 모두 제거 (0개)
	public static final int REQUEST_OTP_FROM_CLIENT = 20; // 사용자가 OTP 요구 (0개)


	// 시스템에서 UsbManager 가져올꺼임
	public UsbManager mUsbManager;

	// 악세사리 퍼미션을 요청 할 때 쓰임 ~
	private PendingIntent mPermissionIntent;
	private boolean mPermissionRequestPending;

	/********** 카메라 *********/

	private Context mContext = this;
	private Camera mCamera;
	private CameraPreview mPreview;
	FrameLayout camera_preview;
	public static Directory dir = new Directory();

	// UI 노가다 ㅠ ㅠ..
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

	// 핸들러
	Handler mHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {

			InputThread inputThread;

			switch (msg.what) {
			case OPEN_OUTSIDE_FROM_DOOR:

				stateTv.setText("문이 밖에서 열렸습니다");
				if(registeredMessageSpeaking(PREFERENCES_IN_MSG)) // In Out 이 사용자 입장이랑 도어락 입장이랑 달라서 살짝 바껴보임
					break;
				else { // default 값
					if(mTextToSpeechController.hasTts())
						mTextToSpeechController.playScript("보고 싶었어요. 환영합니다.");
					break;
				}

			case OPEN_INSIDE_FROM_DOOR:
				stateTv.setText("문이 안에서 열렸습니다");
				if(registeredMessageSpeaking(PREFERENCES_OUT_MSG)) // In Out 이 사용자 입장이랑 도어락 입장이랑 달라서 살짝 바껴보임
					break;
				else { // default 값
					if(mTextToSpeechController.hasTts())
						mTextToSpeechController.playScript("잘 다녀오세요. 메르스 조심하세요.");
					break;
				}

			case FAIL_FROM_DOOR:
				stateTv.setText("사용자가 잘못된 비밀번호를 입력했습니다");
				if(mTextToSpeechController.hasTts())
					mTextToSpeechController.playScript("실패가 반복되면 신고하겠습니다");

				MytakePicture();
				break;

			case START_INPUT_FROM_DOOR:
				stateTv.setText("비밀번호 입력 시작");
				if(mTextToSpeechController.hasTts())
					mTextToSpeechController.playScript("비밀번호 입력 시작");

				MytakePicture();

				if(serverThread != null) {
					if(serverThread.isAlive()) {
						// 현재 접속한 모든 클라이언트들에게 도어락에 근접했는지 물어본다.
						String proximityAlert = "proximity_alert;"; // 도어락에 근접해 있는지 알려달라고 부탁
						inputThread = new InputThread(serverThread, proximityAlert);
						inputThread.start();
					}
				}

				isProximityAlertTimeout = false;
				proximityAlertDecisionHandler.postDelayed(proximityAlertDecisionTask, 10000);

				break;

			case SEND_OTP_USED_NUM_FROM_DOOR:
				stateTv.setText("OTP의 사용 횟수를 알아냈습니다 (" + (String)msg.obj + ")");

				mDoorController.mOtpUsedNum = Integer.parseInt((String)msg.obj);

				String otp = mDoorController.mOtp;
				for(int i = 0; i < mDoorController.mOtpUsedNum; i++) {
					otp = mDoorController.md5.encrypt(otp);
				}
				byte[] digest = otp.getBytes();
				if(digest.length < 4) {
					Toast.makeText(MainActivity.this, "해쉬한 함수의 길이가 너무 짧습니다 !!", Toast.LENGTH_SHORT).show();
					return;
				}

				byte otpTmp[] = new byte[4];
				// 아두이노에게 진짜 OTP를 전송해준다 ^^ . . .
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

				// 구한 OTP를 저장한다.
				putPre(PREFERENCES_OTP, new String(otpTmp));
				otpTv.setText(getPre(PREFERENCES_OTP));

				if(serverThread != null) {
					if(serverThread.isAlive()) {
						// 현재 접속한 모든 클라이언트들에게 바뀐 OTP 정보를 알려준다.
						String otpChg = "otp;" + getPre(PREFERENCES_OTP) + ";";
						inputThread = new InputThread(serverThread, otpChg);
						inputThread.start();
					}
				}

				break;

			case GET_LOCATION_FROM_LOCATION:

				// (String)msg.obj = "필요한 위치 정보를 얻었습니다.\n더 이상의 위치 정보는 수집하지 않습니다."
				Toast.makeText(MainActivity.this, (String)msg.obj, Toast.LENGTH_SHORT).show();

				if(mLocationController.hasLocation()) {
					putPre(PREFERENCES_LATITUDE, String.valueOf(mLocationController.mLocation.getLatitude()));
					putPre(PREFERENCES_LONGITUDE, String.valueOf(mLocationController.mLocation.getLongitude()));
				}

				if(serverThread != null) {
					if(serverThread.isAlive()) {
						// 현재 접속한 모든 클라이언트들에게 도어락 위치를 알려준다.
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
					Toast.makeText(MainActivity.this, "도어락 서비스가 끝났습니다.", Toast.LENGTH_SHORT).show();
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
							Toast.makeText(MainActivity.this, "서버가 종료되었습니다", Toast.LENGTH_SHORT).show();
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
			case IS_ENTERING_FROM_CLIENT: // 도어락이 근접경보를 요구해서 사용자가 준 것 (1개)
				// no-op NioChatterServer가 SheardPreferences로 넣어 줌
				break;
			case OUT_MSG_FROM_CLIENT: // 사용자가 등록한 나갈 때 인사말 (1개)
				// no-op NioChatterServer가 SheardPreferences로 넣어 줌
				break;
			case IN_MSG_FROM_CLIENT: // 사용자가 등록한 들어올 때 인사말 (1개)
				// no-op NioChatterServer가 SheardPreferences로 넣어 줌
				break;
			case REMOVE_MSG_FROM_CLIENT: // 사용자가 등록한 인사말 모두 제거 (0개)
				// no-op NioChatterServer가 SheardPreferences에서 제거
				break;
			case REQUEST_OTP_FROM_CLIENT: // 사용자가 OTP 요구 (0개)

				if(serverThread != null) {
					if(serverThread.isAlive()) {
						// 현재 접속한 모든 클라이언트들에게 OTP 정보를 알려준다.
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

	// 브로드캐스트리시버
	private final BroadcastReceiver mDoorReceiver = new DoorReceiver();
	class DoorReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {

			String action = intent.getAction(); // 무슨 액션이냐?

			if(ACTION_USB_PERMISSION.equals(action)) {

				synchronized (this) {

					UsbAccessory accessory = (UsbAccessory)intent.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
					boolean hasPermission = intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false);
					if(hasPermission) {
						// 도어락 쓰레드 시작 !!!
						if(mDoorController == null) {
							mDoorController = new DoorController(mHandler, mUsbManager, accessory);
							mDoorController.start();
						}
					}
					else {
						Log.d(TAG, "permission denied for accessory " + accessory);
						Toast.makeText(MainActivity.this, "permission denied for accessory ", Toast.LENGTH_SHORT).show();
					}

					// 해당 퍼미션 요구는 처리 했으니깐 false ~!!
					mPermissionRequestPending = false;
				}
			}
			else if(UsbManager.ACTION_USB_ACCESSORY_DETACHED .equals(action)) {
				//if((accessory != null) && accessory.equals(mUsbAccessory)) //accessory 얻는 코드가 더 있음
				synchronized (this) {
					if(mDoorController != null) {
						if(mDoorController.isAlive()) { mDoorController.quit(); }
						mDoorController = null;
					}
					Toast.makeText(MainActivity.this, "도어락과 연결이 해제되었습니다.", Toast.LENGTH_SHORT).show();
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
		// TODO : 도어락에 근접한 사람이 있는지 판단은 비밀번호 입력 시작시에 물어볼 때
		// 근접한 사람이 있어야 TRUE가 된다 ~!!
		putPre(PREFERENCES_IS_ENTERING, "false");
		isProximityAlertTimeout = true;

		// 위젯 setEnabled
		startBtn.setEnabled(true);
		finishBtn.setEnabled(false);
		// 네트워크 상태에 대처하는 BroadcastReceiver 등록
		IntentFilter intentfilter = new IntentFilter();
		intentfilter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
		intentfilter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
		networkStateReceiver = new NetworkStateReceiver();
		registerReceiver(networkStateReceiver, intentfilter);

		// 카메라
		camera_preview = (FrameLayout) findViewById(R.id.camera_preview);
		setCamera();

		// 클라이언트가 남긴 메세지 ~!!
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
			mPreview.check = true; // flag 변수

		long time = System.currentTimeMillis();
		SimpleDateFormat dayTime = new SimpleDateFormat(
				" 년도 : yyyy년\n 날짜 : MM월dd일\n 시각 : a hh시mm분");
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
		// 카메라 인스턴스 생성
		if (checkCameraHardware(mContext)) {
			mCamera = getCameraInstance();

			// 프리뷰창을 생성하고 액티비티의 레이아웃으로 지정합니다
			mPreview = new CameraPreview(this, mCamera);

			camera_preview.addView(mPreview); // preview 화면이 되게함.


		} else {
			Toast.makeText(mContext, "no camera on this device!",
					Toast.LENGTH_SHORT).show();
		}

	}
	/** 카메라 하드웨어 지원 여부 확인 */
	@SuppressLint("NewApi")
	private boolean checkCameraHardware(Context context) {
		if (context.getPackageManager().hasSystemFeature(
				PackageManager.FEATURE_CAMERA)) {
			// 카메라가 최소한 한개 있는 경우 처리
			Log.i("SETX",
					"Number of available camera : "
							+ Camera.getNumberOfCameras());
			return true;
		} else {
			// 카메라가 전혀 없는 경우
			Toast.makeText(mContext, "No camera found!", Toast.LENGTH_SHORT)
			.show();
			return false;
		}
	}
	/** 카메라 인스턴스를 안전하게 획득합니다 */
	@SuppressLint("NewApi")
	public static Camera getCameraInstance() {
		Camera c = null;
		try {
			c = Camera.open(0);
		} catch (Exception e) {
			// 사용중이거나 사용 불가능 한 경우
		}
		return c;
	}

	// GPS를 얻어오기 위해 사용자에게서 퍼미션 얻기 위한 과정중 일부
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
				// 도어락 쓰레드 시작 !!!
				if(mDoorController == null) {
					mDoorController = new DoorController(mHandler, mUsbManager, accessory);
					mDoorController.start();
				}
			}
			else { // 악세사리가 있긴한데 퍼미션을 못 얻는다면.. 퍼미션 요청
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
		// ㄷㄷ ㅠㅠ 딜레이 되서 실행 되는건데 나 죽고나면 뭐할려고 ??
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

	// 서버 시작 버튼
	public void onStratServer(View v) {
		NetworkStateReceiver ns = new NetworkStateReceiver();
		if(ns.getConnectivityStatus(v.getContext()) != 
				NetworkStateReceiver.WIFI_CONNECT) {
			Toast.makeText(v.getContext(), "wifi 상황에서만 시작합니다.\nwifi 켠 후 다시 시도해주세요.", Toast.LENGTH_SHORT).show();
			return; // -END-
		}

		if(serverThread == null) {
			// 서버소켓채널은 스레드로 작동시킨다.
			serverThread = new NioChatterServer(MainActivity.this);
			serverThread.start();
			debugTv.setText("** 서버가 시작되었습니다.\n");

			// 위젯 setEnabled
			startBtn.setEnabled(false);
			finishBtn.setEnabled(true);
		} else {
			Toast.makeText(v.getContext(), "서버가 이미!!! 실행되고 있습니다.", Toast.LENGTH_SHORT).show();
		}
	}

	// 서버 종료 버튼
	public void onFinshServer(View v) {
		if(serverThread != null) {
			if(serverThread.isAlive()) {
				serverThread.quit();
				serverThread = null;
				debugTv.setText("** 서버를 종료합니다.\n");

				// 위젯 setEnabled
				startBtn.setEnabled(true);
				finishBtn.setEnabled(false);

				Toast.makeText(v.getContext(), "서버를 종료합니다.", Toast.LENGTH_SHORT).show();
			}
		}
	}
	// TODO : 비밀번호 변경 더미 인덱스 나중에 제거할 것
	byte mPwIndex = 0x0;
	// 더미용 코드
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
				Toast.makeText(MainActivity.this, "변경 완료", Toast.LENGTH_SHORT).show();
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
						// 서버소켓채널은 스레드로 작동시킨다.
						serverThread = new NioChatterServer(MainActivity.this);
						serverThread.start();
						debugTv.setText("** 서버가 시작되었습니다.\n");

						// 위젯 setEnabled
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
							debugTv.setText("** 서버를 종료합니다.\n");

							// 위젯 setEnabled
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

		// 네트워크 정보가 변경될 때, Wifi, Mobile, Nothing 상태 파악
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