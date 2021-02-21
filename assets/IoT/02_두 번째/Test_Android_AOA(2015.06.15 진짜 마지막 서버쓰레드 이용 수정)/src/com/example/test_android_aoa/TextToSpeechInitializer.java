package com.example.test_android_aoa;

import java.util.Locale;

import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.speech.tts.TextToSpeech;
import android.speech.tts.TextToSpeech.OnInitListener;
import android.speech.tts.TextToSpeech.OnUtteranceCompletedListener;
import android.util.Log;

//TTS 초기화를 맡는 클래스 초기화 상황에 따라 TextToSpeechStartupListener 인터페이스를 콜백해준다 !!
public class TextToSpeechInitializer {
	private static final String TAG = "TextToSpeechInitializer";

	private TextToSpeech tts;
	private TextToSpeechStartupListener callback;
	private Context context;

	// 생성자
	public TextToSpeechInitializer(Context context, final Locale loc, TextToSpeechStartupListener callback) {
		this.callback = callback;
		this.context = context;

		// 넘어온 지역에 맞는 TTS 인스턴스를 만든다.
		createTextToSpeech(loc);
	}

	// TTS 인스턴스 만들기
	private void createTextToSpeech(final Locale locale) {
		// 해당 지역을 지원하지 않을 경우 인스턴스가 만들어 지지 않는다.
		// 또 OnInitListener를 받아서 onInit()을 콜백하여 인스턴스 생성 성공여부를 알려준다.
		// 바로 딱 만들어주는 것도 아니고 까다로움 -ㅅ-;
		tts = new TextToSpeech(context, new OnInitListener() {
			@Override
			public void onInit(int status) {
				if (status == TextToSpeech.SUCCESS)
					setTextToSpeechSettings(locale);
				else {
					Log.e(TAG, "error creating text to speech");
					// TTS 인스턴스를 만들 수 없다 !
					// TextToSpeechStartupListener 인터페이스의 onFailedToInit() 콜백이 필요한 시점이다.
					callback.onFailedToInit();
				}
			}
		});
	}

	private void setTextToSpeechSettings(final Locale locale) {

		Locale defaultOrPassedIn = locale; // Locale.getDefault() 또는 Locale.KOREA가 들어왔을 꺼임

		if (locale == null) { // 그냥 null을 넘겼다면 Locale.getDefault()로 셋팅
			defaultOrPassedIn = Locale.getDefault();
		}
		// TTS가 넘어온 지역을 지원하는지 체크해야 함
		switch (tts.isLanguageAvailable(defaultOrPassedIn)) {
		case TextToSpeech.LANG_AVAILABLE: // TTS가 준비되었고, 언어를 지원한다.
		case TextToSpeech.LANG_COUNTRY_AVAILABLE: // TTS가 준비되었고, 언어와 지역을 지원한다.
		case TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE:// TTS가 준비되었고, 언어, 지역 그리고 그 변이형(variant)을 지원한다.
			Log.d(TAG, "SUPPORTED");
			tts.setLanguage(locale);
			// TextToSpeechStartupListener 인터페이스의 onSuccessfulInit() 콜백이 필요한 시점이다.
			callback.onSuccessfulInit(tts);
			break;
		case TextToSpeech.LANG_MISSING_DATA: // TTS가 준비되지 않았다. 하지만 언어 데이터를 받으면 사용가능하다.
			Log.d(TAG, "MISSING_DATA");
			// shared preference를 체크해서 언어 데이터 설치를 중복적으로 하지 않는다.
			if (LanguageDataInstallBroadcastReceiver.isWaiting(context)) {
				Log.d(TAG, "waiting for data...");
				// TextToSpeechStartupListener 인터페이스의 onWaitingForLanguageData() 콜백이 필요한 시점이다.
				callback.onWaitingForLanguageData();
			}
			else {
				Log.d(TAG, "require data...");
				// TextToSpeechStartupListener 인터페이스의 onRequireLanguageData() 콜백이 필요한 시점이다.
				// 사용자가 언어 데이터를 다운로드 받도록 해야 한다.
				callback.onRequireLanguageData();
			}
			break;
		case TextToSpeech.LANG_NOT_SUPPORTED: // TTS가 준비되지 않았다. 앱은 언어를 지원하지 않는다. 설치따위 상관없이 무조건 안 된다.
			Log.d(TAG, "NOT SUPPORTED");
			// TextToSpeechStartupListener 인터페이스의 onFailedToInit() 콜백이 필요한 시점이다.
			callback.onFailedToInit();
			break;
		}
	}

//	// tts 인스턴스에 리스너를 달아주는 메소드
//	public void setOnUtteranceCompleted(OnUtteranceCompletedListener whenDoneSpeaking) {
//		int result = tts.setOnUtteranceCompletedListener(whenDoneSpeaking);
//		if (result == TextToSpeech.ERROR) {
//			Log.e(TAG, "failed to add utterance listener");
//		}
//	}

	// 만약 다운로드를 기다리는 동안 대화상자를 봐여주는 helper method
	public void installLanguageData(Dialog ifAlreadyWaiting) {
		boolean alreadyDidThis = LanguageDataInstallBroadcastReceiver.isWaiting(context);
		if (alreadyDidThis) {
			// shared preference가 기다린다고 체크 됬으니까 전달 받은 대화상자를 보여준다.
			ifAlreadyWaiting.show();
		} else {
			// 아직 shared preference가 기다린다고 체크 안됬으니깐 언어 데이터 설치를 진행한다.
			installLanguageData();
		}
	}

	// 언어 데이터 설치 >_<~!!!
	public void installLanguageData() {
		// shared preference에다가 언어 데이터 설치를 기다린다고 체크~!
		LanguageDataInstallBroadcastReceiver.setWaiting(context, true);

		Intent installIntent = new Intent();
		installIntent.setAction(TextToSpeech.Engine.ACTION_INSTALL_TTS_DATA);
		context.startActivity(installIntent);

		// 설치가 완료되면 "android.speech.tts.engine.TTS_DATA_INSTALLED" 인텐트가
		// 뿌려질것이며, LanguageDataInstallBroadcastReceiver가 받을것이다 ~!!
	}
}