package com.example.test_android_aoa;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.speech.tts.TextToSpeech;
import android.util.Log;

//앱이 언어 데이터 설치를 요청할 때, 언어 데이터 설치의 상황의 상태를 추적하며 돕는다.
//데이터 설치 요구를 중복시키는 것을 피하고,언어 데이터 설치완료를 귀담아 듣는다.
//따라서 ~
//SharedPreferences를 이용하여 언어 데이터 설치 중복 요구를 피하기 위한 상태를 저장한다.
//앱은 반드시 SharedPreferences의 상태를 체크해야하고 static method를 사용한다.

//Manifest에 꼭 다음 추가시키기
//<receiver android:name=".LanguageDataInstallBroadcastReceiver" >
//<intent-filter>
// <action android:name="android.speech.tts.engine.TTS_DATA_INSTALLED" />
//</intent-filter>
//</receiver>

//또는 ~!!
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
		// 언어 데이터가 설치되면 안드로이드는 다음 인텐트를 뿌린다.
		// "android.speech.tts.engine.TTS_DATA_INSTALLED"
		if (intent.getAction().equals(TextToSpeech.Engine.ACTION_TTS_DATA_INSTALLED)) {
			Log.d(TAG, "language data preference: " + intent.getAction());
			// 설치를 했으므로 기다리는 상태가 아니므로 false
			setWaiting(context, false);
		}
	}

	// SharedPreferences를 이용하여 언어 데이터 설치 중복 요구를 피하기 위한 상태를 저장한다.
	public static void setWaiting(Context context, boolean waitingStatus) {
		SharedPreferences preferences;
		preferences = context.getSharedPreferences(PREFERENCES_NAME, Context.MODE_WORLD_WRITEABLE);
		Editor editor = preferences.edit();
		editor.putBoolean(WAITING_PREFERENCE_NAME, waitingStatus);
		editor.commit();
	}

	// 언어 데이터 설치 중복을 피하기 위해서 반드시 SharedPreferences를 체크하는
	// static method인 isWaiting()을 체크해야한다.
	public static boolean isWaiting(Context context) {
		SharedPreferences preferences;
		preferences = context.getSharedPreferences(PREFERENCES_NAME, Context.MODE_WORLD_READABLE);
		boolean waiting = preferences.getBoolean(WAITING_PREFERENCE_NAME, WAITING_DEFAULT);
		return waiting;
	}
}