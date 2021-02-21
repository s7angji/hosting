package com.example.chattingnioclient;

import android.app.Activity;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;


public class ChattingNioClientActivity extends Activity {

	private static final int NOTIFICATION_ID = 8888;

	// 소켓 연결 관련
	public static final int MSG = 1;
	public static final int CONNECT_MSG = 2;
	public static final int FINISH_MSG = 3;
	public static final int SERVER_CONNECTED_TIMEOUT_MSG = 4; // 서버 연결을 5초 이내에 하지 못 했다.

	// FROM_SERVER
	public static final int OTP_FROM_SERVER = 5; // 도어락에서 OTP 정보를 얻었다 ^^
	public static final int DOOR_LOCATION_FROM_SERVER = 6; // 도어락의 위치 정보를 얻었으므로 근접정보 세팅을 할 수 있다.
	public static final int PROXIMITY_ALERT_FROM_SERVER = 7; // 도어락에서 근접경보를 보내라 카네 -ㅅ-..
	public static final int SUSPICIOUS_APPROACH_FROM_SERVER = 8; // 도어락에서 수상한 접근이 있다고 알리네
	public static final int DEBUG_FROM_SERVER = 9; // 메시지 파싱 중에 찍어볼 사항들


	public Handler mHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			StringBuilder s = new StringBuilder();
			switch(msg.what) {
			case MSG: // 서버로 부터 데이터를 수신하였을 때 사용한다.
				s.append(debugTv.getText());
				s.append("** " + (String)msg.obj + "\n");
				debugTv.setText(s.toString());
				break;
			case CONNECT_MSG: // 연결되었을 때 사용한다.
				s.append(debugTv.getText());
				s.append("** " + (String)msg.obj + "\n");
				debugTv.setText(s.toString());

				connectBtn.setEnabled(false);
				ipChgBtn.setEnabled(false);
				finishBtn.setEnabled(true);
				sendBtn.setEnabled(true);
				otpBtn.setEnabled(true);
				break;
			case FINISH_MSG: // 종료할 때 사용한다.
				s.append("** " + (String)msg.obj + "\n");
				debugTv.setText(s.toString());

				if(clientThread != null) {
					if(clientThread.isAlive()) {
						clientThread.quit();
						clientThread = null;
					}
				}
				else
					clientThread = null;

				connectBtn.setEnabled(true);
				ipChgBtn.setEnabled(true);
				finishBtn.setEnabled(false);
				sendBtn.setEnabled(false);
				otpBtn.setEnabled(false);
				break;
			case SERVER_CONNECTED_TIMEOUT_MSG:
				s.append("** " + (String)msg.obj + "\n");
				debugTv.setText(s.toString());

				if(clientThread != null) {
					if(clientThread.isAlive()) {
						clientThread.quit();
						clientThread = null;
					}
				}
				else
					clientThread = null;

				connectBtn.setEnabled(true);
				ipChgBtn.setEnabled(true);
				finishBtn.setEnabled(false);
				sendBtn.setEnabled(false);
				otpBtn.setEnabled(false);
				break;
			case OTP_FROM_SERVER:
				otpTv.setText(getPre(ChattingNioClientActivity.PREFERENCES_OTP));
				break;
			case DOOR_LOCATION_FROM_SERVER: // 도어락 위치를 알았다!! 따라서 근접경보를 셋팅해야 한다.

				// TODO : 여기서 근접경보가 등록된다
				mLocationReceiver = new LocationReceiver(ChattingNioClientActivity.this,
						Double.valueOf(getPre(ChattingNioClientActivity.PREFERENCES_LATITUDE)),
						Double.valueOf(getPre(ChattingNioClientActivity.PREFERENCES_LONGITUDE)),
						10);

				break;
			case PROXIMITY_ALERT_FROM_SERVER:
				Toast.makeText(ChattingNioClientActivity.this, (String)msg.obj, Toast.LENGTH_LONG).show();

				// 도어락이 지금 도어락의 근접했는지 알아내고자 함
				Thread t = new InputThread(clientThread, "is_entering;" + String.valueOf(mLocationReceiver.isEntering) + ";");
				t.start();

				break;
			case SUSPICIOUS_APPROACH_FROM_SERVER:
				Toast.makeText(ChattingNioClientActivity.this, (String)msg.obj, Toast.LENGTH_LONG).show();

				NotificationManager notificationManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);

				PendingIntent pi = PendingIntent.getActivity(ChattingNioClientActivity.this, 0, new Intent(), 0);

				Notification notification = new Notification(R.drawable.ic_launcher, "지금 도어락을 여는 사람이 그룹원이 아닐 수 있습니다.", System.currentTimeMillis());
				notification.setLatestEventInfo(ChattingNioClientActivity.this, "누구인지 알 수 없습니다", "지금 도어락을 여는 사람이 그룹원이 아닐 수 있습니다.", pi);

				notification.defaults |= Notification.DEFAULT_SOUND;
				notification.flags |= Notification.FLAG_AUTO_CANCEL;

				notificationManager.notify(NOTIFICATION_ID, notification);

				break;
			case DEBUG_FROM_SERVER:
				Toast.makeText(ChattingNioClientActivity.this, (String)msg.obj, Toast.LENGTH_LONG).show();
				break;
			default:
				break;

			}
			super.handleMessage(msg);
		}
	};

	private NioChatterClient clientThread = null;

	NetworkStateReceiver networkStateReceiver;
	public ConnectivityManager cm = null;
	public WifiManager wifiManager = null;

	LocationReceiver mLocationReceiver;

	private Button connectBtn;
	private Button finishBtn;
	private Button sendBtn;
	private Button ipChgBtn;
	private TextView debugTv;
	private TextView otpTv;
	private Button otpBtn;
	private EditText ipEt;
	private EditText outMsgEt;
	private EditText inMsgEt;

	public static final String PREFERENCES_SERVER_IP = "PREFERENCES_SERVER_IP";
	public static final String PREFERENCES_OTP = "PREFERENCES_OTP";
	public static final String PREFERENCES_LATITUDE = "PREFERENCES_LATITUDE";
	public static final String PREFERENCES_LONGITUDE = "PREFERENCES_LONGITUDE";
	// public static final String PREFERENCES_NAME = "PREFERENCES_NAME"; // TODO : 나중에 사용자 이름은 서버가 지정한 이름이 되도록 해야 함. 1. 서버 연결 2.사용자 이름 요구 3. 서버가 사용자 이름을 줌 4. 사용자 이름이 겹치면 다시 요구 5. 서버가 이름을 주고 더 이상 겹치지 않음

	public String getPre(String key) {
		SharedPreferences prefs = getSharedPreferences(key, MODE_PRIVATE);
		return prefs.getString(key, "default");
	}

	public void putPre(String key, String input) {
		SharedPreferences.Editor prefs = getSharedPreferences(key, MODE_PRIVATE).edit();
		prefs.putString(key, input);
		prefs.commit();
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		if(clientThread != null) {
			if(clientThread.isAlive()) {
				clientThread.quit();
				clientThread = null;
			}
		}
		if(mLocationReceiver != null) {
			mLocationReceiver.release();
			mLocationReceiver = null;
		}
		unregisterReceiver(networkStateReceiver);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.activity_chatting_nio_client);

		/****** Widget Instance Reference ******/
		ipChgBtn = (Button)findViewById(R.id.ipChgBtn);
		ipEt = (EditText)findViewById(R.id.ipEt);

		outMsgEt = (EditText)findViewById(R.id.outMsgEt);
		inMsgEt = (EditText)findViewById(R.id.inMsgEt);
		sendBtn = (Button)findViewById(R.id.sendBtn);

		connectBtn = (Button)findViewById(R.id.connectBtn);
		finishBtn = (Button)findViewById(R.id.finishBtn);

		otpBtn = (Button)findViewById(R.id.otpBtn);
		otpTv = (TextView)findViewById(R.id.otpTv);

		debugTv = (TextView)findViewById(R.id.debugTv);

		// TODO : 여기서 미리 sharedpreferences에 저장된 서버 IP를 편집창에 보여준다.
		ipEt.setText(getPre(PREFERENCES_SERVER_IP));

		/****** Listener Init ******/
		initSetOnClickListener();

		connectBtn.setEnabled(true);
		ipChgBtn.setEnabled(true);
		finishBtn.setEnabled(false);
		sendBtn.setEnabled(false);
		otpBtn.setEnabled(false);

		// 네트워크 상태에 대처하는 BroadcastReceiver 등록
		IntentFilter intentfilter = new IntentFilter();
		intentfilter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
		intentfilter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
		networkStateReceiver = new NetworkStateReceiver();
		registerReceiver(networkStateReceiver, intentfilter);
	}

	private void initSetOnClickListener() {
		// 서버와 연결 작업을 수행한다.
		connectBtn.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {

				NetworkStateReceiver ns = new NetworkStateReceiver();
				if(ns.getConnectivityStatus(v.getContext()) != 
						NetworkStateReceiver.WIFI_CONNECT) {
					Toast.makeText(v.getContext(), "wifi 상황에서만 연결합니다.\nwifi 켠 후 다시 시도해주세요.", Toast.LENGTH_SHORT).show();
					return; // -END-
				}


				// TODO : 사용자가 나중에 IP주소를 변경할 수도 있어야 함
				String prefIp = getPre(PREFERENCES_SERVER_IP);
				if("default".equals(prefIp)) {
					prefIp = ipEt.getText().toString(); // TODO : 나중에 UI에서 처리해야할 부분~!!
					putPre(PREFERENCES_SERVER_IP, prefIp);
				}

				if(clientThread == null) {
					clientThread = new NioChatterClient(ChattingNioClientActivity.this, prefIp);
					clientThread.start();
				} else {
					Toast.makeText(v.getContext(), "서비스가 이미!!! 실행되고 있습니다.", Toast.LENGTH_SHORT).show();
				}
			}
		});

		// 서버와 접속을 종료한다.
		finishBtn.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				if(clientThread != null) {
					if(clientThread.isAlive()) {
						clientThread.quit();
						clientThread = null;
						debugTv.setText("종료버튼으로 접속이 종료되었습니다.");
						connectBtn.setEnabled(true);
						ipChgBtn.setEnabled(true);
						finishBtn.setEnabled(false);
						sendBtn.setEnabled(false);
						otpBtn.setEnabled(false);
						Toast.makeText(v.getContext(), "접속이 종료되었습니다.", Toast.LENGTH_SHORT).show();
					}
				}
			}
		});

		sendBtn.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				StringBuilder sb = new StringBuilder();
				if(!outMsgEt.getText().toString().equals("")) {
					sb.append("out_msg;");
					sb.append(outMsgEt.getText().toString());
					sb.append(";");
				}
				if(!inMsgEt.getText().toString().equals("")) {
					sb.append("in_msg;");
					sb.append(inMsgEt.getText().toString());
					sb.append(";");
				}
				if(!sb.toString().equals("")) {
					//안드로이드 메인 스레드에서 데이터를 전송할 수 없기 때문에 스레드를 만들었다.
					Thread t = new InputThread(clientThread, sb.toString());
					t.start();
				}
				else {
					Thread t = new InputThread(clientThread, "remove_msg;");
					t.start();
				}
			}
		});

		ipChgBtn.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				String prefIp = ipEt.getText().toString();
				putPre(PREFERENCES_SERVER_IP, prefIp);
			}
		});

		otpBtn.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				// OTP를 요청해야 함
				Thread t = new InputThread(clientThread, "request_otp;");
				t.start();
			}
		});
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
					if(clientThread == null) {
						// 클라이언트 쓰레드 시작 시킨다
						clientThread = new NioChatterClient(ChattingNioClientActivity.this, getPre(PREFERENCES_SERVER_IP));
						clientThread.start();

						debugTv.setText("** 서비스가 시작되었습니다.\n");

						// 위젯 setEnabled
						connectBtn.setEnabled(false);
						ipChgBtn.setEnabled(false);
						sendBtn.setEnabled(true);
						finishBtn.setEnabled(true);
						otpBtn.setEnabled(true);
					} else {
						return;
					}
					break;

				case MOBILE_CONNECT:
				case NOTHING:
					if(clientThread != null) {
						if(clientThread.isAlive()) {
							clientThread.quit();
							clientThread = null;

							debugTv.setText("** 서비스를 종료합니다.\n");

							// 위젯 setEnabled
							connectBtn.setEnabled(true);
							ipChgBtn.setEnabled(true);
							sendBtn.setEnabled(false);
							finishBtn.setEnabled(false);
							otpBtn.setEnabled(false);
						}
						else
							clientThread = null;
					}
					break;

				default:
					Toast.makeText(context, "I don't know", Toast.LENGTH_SHORT).show();
				}
			}
		}

		// 네트워크 정보가 변경될 때, Wifi, Mobile, Nothing 상태 파악
		public int getConnectivityStatus(Context context) {
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