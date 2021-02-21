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

	// ���� ���� ����
	public static final int MSG = 1;
	public static final int CONNECT_MSG = 2;
	public static final int FINISH_MSG = 3;
	public static final int SERVER_CONNECTED_TIMEOUT_MSG = 4; // ���� ������ 5�� �̳��� ���� �� �ߴ�.

	// FROM_SERVER
	public static final int OTP_FROM_SERVER = 5; // ��������� OTP ������ ����� ^^
	public static final int DOOR_LOCATION_FROM_SERVER = 6; // ������� ��ġ ������ ������Ƿ� �������� ������ �� �� �ִ�.
	public static final int PROXIMITY_ALERT_FROM_SERVER = 7; // ��������� �����溸�� ������ ī�� -��-..
	public static final int SUSPICIOUS_APPROACH_FROM_SERVER = 8; // ��������� ������ ������ �ִٰ� �˸���
	public static final int DEBUG_FROM_SERVER = 9; // �޽��� �Ľ� �߿� �� ���׵�


	public Handler mHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			StringBuilder s = new StringBuilder();
			switch(msg.what) {
			case MSG: // ������ ���� �����͸� �����Ͽ��� �� ����Ѵ�.
				s.append(debugTv.getText());
				s.append("** " + (String)msg.obj + "\n");
				debugTv.setText(s.toString());
				break;
			case CONNECT_MSG: // ����Ǿ��� �� ����Ѵ�.
				s.append(debugTv.getText());
				s.append("** " + (String)msg.obj + "\n");
				debugTv.setText(s.toString());

				connectBtn.setEnabled(false);
				ipChgBtn.setEnabled(false);
				finishBtn.setEnabled(true);
				sendBtn.setEnabled(true);
				otpBtn.setEnabled(true);
				break;
			case FINISH_MSG: // ������ �� ����Ѵ�.
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
			case DOOR_LOCATION_FROM_SERVER: // ����� ��ġ�� �˾Ҵ�!! ���� �����溸�� �����ؾ� �Ѵ�.

				// TODO : ���⼭ �����溸�� ��ϵȴ�
				mLocationReceiver = new LocationReceiver(ChattingNioClientActivity.this,
						Double.valueOf(getPre(ChattingNioClientActivity.PREFERENCES_LATITUDE)),
						Double.valueOf(getPre(ChattingNioClientActivity.PREFERENCES_LONGITUDE)),
						10);

				break;
			case PROXIMITY_ALERT_FROM_SERVER:
				Toast.makeText(ChattingNioClientActivity.this, (String)msg.obj, Toast.LENGTH_LONG).show();

				// ������� ���� ������� �����ߴ��� �˾Ƴ����� ��
				Thread t = new InputThread(clientThread, "is_entering;" + String.valueOf(mLocationReceiver.isEntering) + ";");
				t.start();

				break;
			case SUSPICIOUS_APPROACH_FROM_SERVER:
				Toast.makeText(ChattingNioClientActivity.this, (String)msg.obj, Toast.LENGTH_LONG).show();

				NotificationManager notificationManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);

				PendingIntent pi = PendingIntent.getActivity(ChattingNioClientActivity.this, 0, new Intent(), 0);

				Notification notification = new Notification(R.drawable.ic_launcher, "���� ������� ���� ����� �׷���� �ƴ� �� �ֽ��ϴ�.", System.currentTimeMillis());
				notification.setLatestEventInfo(ChattingNioClientActivity.this, "�������� �� �� �����ϴ�", "���� ������� ���� ����� �׷���� �ƴ� �� �ֽ��ϴ�.", pi);

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
	// public static final String PREFERENCES_NAME = "PREFERENCES_NAME"; // TODO : ���߿� ����� �̸��� ������ ������ �̸��� �ǵ��� �ؾ� ��. 1. ���� ���� 2.����� �̸� �䱸 3. ������ ����� �̸��� �� 4. ����� �̸��� ��ġ�� �ٽ� �䱸 5. ������ �̸��� �ְ� �� �̻� ��ġ�� ����

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

		// TODO : ���⼭ �̸� sharedpreferences�� ����� ���� IP�� ����â�� �����ش�.
		ipEt.setText(getPre(PREFERENCES_SERVER_IP));

		/****** Listener Init ******/
		initSetOnClickListener();

		connectBtn.setEnabled(true);
		ipChgBtn.setEnabled(true);
		finishBtn.setEnabled(false);
		sendBtn.setEnabled(false);
		otpBtn.setEnabled(false);

		// ��Ʈ��ũ ���¿� ��ó�ϴ� BroadcastReceiver ���
		IntentFilter intentfilter = new IntentFilter();
		intentfilter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
		intentfilter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
		networkStateReceiver = new NetworkStateReceiver();
		registerReceiver(networkStateReceiver, intentfilter);
	}

	private void initSetOnClickListener() {
		// ������ ���� �۾��� �����Ѵ�.
		connectBtn.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {

				NetworkStateReceiver ns = new NetworkStateReceiver();
				if(ns.getConnectivityStatus(v.getContext()) != 
						NetworkStateReceiver.WIFI_CONNECT) {
					Toast.makeText(v.getContext(), "wifi ��Ȳ������ �����մϴ�.\nwifi �� �� �ٽ� �õ����ּ���.", Toast.LENGTH_SHORT).show();
					return; // -END-
				}


				// TODO : ����ڰ� ���߿� IP�ּҸ� ������ ���� �־�� ��
				String prefIp = getPre(PREFERENCES_SERVER_IP);
				if("default".equals(prefIp)) {
					prefIp = ipEt.getText().toString(); // TODO : ���߿� UI���� ó���ؾ��� �κ�~!!
					putPre(PREFERENCES_SERVER_IP, prefIp);
				}

				if(clientThread == null) {
					clientThread = new NioChatterClient(ChattingNioClientActivity.this, prefIp);
					clientThread.start();
				} else {
					Toast.makeText(v.getContext(), "���񽺰� �̹�!!! ����ǰ� �ֽ��ϴ�.", Toast.LENGTH_SHORT).show();
				}
			}
		});

		// ������ ������ �����Ѵ�.
		finishBtn.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				if(clientThread != null) {
					if(clientThread.isAlive()) {
						clientThread.quit();
						clientThread = null;
						debugTv.setText("�����ư���� ������ ����Ǿ����ϴ�.");
						connectBtn.setEnabled(true);
						ipChgBtn.setEnabled(true);
						finishBtn.setEnabled(false);
						sendBtn.setEnabled(false);
						otpBtn.setEnabled(false);
						Toast.makeText(v.getContext(), "������ ����Ǿ����ϴ�.", Toast.LENGTH_SHORT).show();
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
					//�ȵ���̵� ���� �����忡�� �����͸� ������ �� ���� ������ �����带 �������.
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
				// OTP�� ��û�ؾ� ��
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
						// Ŭ���̾�Ʈ ������ ���� ��Ų��
						clientThread = new NioChatterClient(ChattingNioClientActivity.this, getPre(PREFERENCES_SERVER_IP));
						clientThread.start();

						debugTv.setText("** ���񽺰� ���۵Ǿ����ϴ�.\n");

						// ���� setEnabled
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

							debugTv.setText("** ���񽺸� �����մϴ�.\n");

							// ���� setEnabled
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

		// ��Ʈ��ũ ������ ����� ��, Wifi, Mobile, Nothing ���� �ľ�
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