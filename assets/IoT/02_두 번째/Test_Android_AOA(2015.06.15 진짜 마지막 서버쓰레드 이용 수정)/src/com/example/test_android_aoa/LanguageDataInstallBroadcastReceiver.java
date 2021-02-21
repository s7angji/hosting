package com.example.test_android_aoa;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.speech.tts.TextToSpeech;
import android.util.Log;

//���� ��� ������ ��ġ�� ��û�� ��, ��� ������ ��ġ�� ��Ȳ�� ���¸� �����ϸ� ���´�.
//������ ��ġ �䱸�� �ߺ���Ű�� ���� ���ϰ�,��� ������ ��ġ�ϷḦ �ʹ�� ��´�.
//���� ~
//SharedPreferences�� �̿��Ͽ� ��� ������ ��ġ �ߺ� �䱸�� ���ϱ� ���� ���¸� �����Ѵ�.
//���� �ݵ�� SharedPreferences�� ���¸� üũ�ؾ��ϰ� static method�� ����Ѵ�.

//Manifest�� �� ���� �߰���Ű��
//<receiver android:name=".LanguageDataInstallBroadcastReceiver" >
//<intent-filter>
// <action android:name="android.speech.tts.engine.TTS_DATA_INSTALLED" />
//</intent-filter>
//</receiver>

//�Ǵ� ~!!
//public void register(Context context) {
//		Log.d(TAG, "registered a receiver!!");
//		context.registerReceiver(this, new IntentFilter(TextToSpeech.Engine.ACTION_TTS_DATA_INSTALLED));
//}

//public void unregister(Context context) {
//		context.unregisterReceiver(this);
//}

public class LanguageDataInstallBroadcastReceiver extends BroadcastReceiver {
	private static final String TAG = "LanguageDataInstallBroadcastReceiver";

	private static final String PREFERENCES_NAME = "installedLanguageData";
	private static final String WAITING_PREFERENCE_NAME = "WAITING_PREFERENCE_NAME";
	private static final Boolean WAITING_DEFAULT = false;

	@Override
	public void onReceive(Context context, Intent intent) {
		// ��� �����Ͱ� ��ġ�Ǹ� �ȵ���̵�� ���� ����Ʈ�� �Ѹ���.
		// "android.speech.tts.engine.TTS_DATA_INSTALLED"
		if (intent.getAction().equals(TextToSpeech.Engine.ACTION_TTS_DATA_INSTALLED)) {
			Log.d(TAG, "language data preference: " + intent.getAction());
			// ��ġ�� �����Ƿ� ��ٸ��� ���°� �ƴϹǷ� false
			setWaiting(context, false);
		}
	}

	// SharedPreferences�� �̿��Ͽ� ��� ������ ��ġ �ߺ� �䱸�� ���ϱ� ���� ���¸� �����Ѵ�.
	public static void setWaiting(Context context, boolean waitingStatus) {
		SharedPreferences preferences;
		preferences = context.getSharedPreferences(PREFERENCES_NAME, Context.MODE_WORLD_WRITEABLE);
		Editor editor = preferences.edit();
		editor.putBoolean(WAITING_PREFERENCE_NAME, waitingStatus);
		editor.commit();
	}

	// ��� ������ ��ġ �ߺ��� ���ϱ� ���ؼ� �ݵ�� SharedPreferences�� üũ�ϴ�
	// static method�� isWaiting()�� üũ�ؾ��Ѵ�.
	public static boolean isWaiting(Context context) {
		SharedPreferences preferences;
		preferences = context.getSharedPreferences(PREFERENCES_NAME, Context.MODE_WORLD_READABLE);
		boolean waiting = preferences.getBoolean(WAITING_PREFERENCE_NAME, WAITING_DEFAULT);
		return waiting;
	}
}