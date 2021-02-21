package com.example.test_android_aoa;

import java.util.Locale;

import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.speech.tts.TextToSpeech;
import android.speech.tts.TextToSpeech.OnInitListener;
import android.speech.tts.TextToSpeech.OnUtteranceCompletedListener;
import android.util.Log;

//TTS �ʱ�ȭ�� �ô� Ŭ���� �ʱ�ȭ ��Ȳ�� ���� TextToSpeechStartupListener �������̽��� �ݹ����ش� !!
public class TextToSpeechInitializer {
	private static final String TAG = "TextToSpeechInitializer";

	private TextToSpeech tts;
	private TextToSpeechStartupListener callback;
	private Context context;

	// ������
	public TextToSpeechInitializer(Context context, final Locale loc, TextToSpeechStartupListener callback) {
		this.callback = callback;
		this.context = context;

		// �Ѿ�� ������ �´� TTS �ν��Ͻ��� �����.
		createTextToSpeech(loc);
	}

	// TTS �ν��Ͻ� �����
	private void createTextToSpeech(final Locale locale) {
		// �ش� ������ �������� ���� ��� �ν��Ͻ��� ����� ���� �ʴ´�.
		// �� OnInitListener�� �޾Ƽ� onInit()�� �ݹ��Ͽ� �ν��Ͻ� ���� �������θ� �˷��ش�.
		// �ٷ� �� ������ִ� �͵� �ƴϰ� ��ٷο� -��-;
		tts = new TextToSpeech(context, new OnInitListener() {
			@Override
			public void onInit(int status) {
				if (status == TextToSpeech.SUCCESS)
					setTextToSpeechSettings(locale);
				else {
					Log.e(TAG, "error creating text to speech");
					// TTS �ν��Ͻ��� ���� �� ���� !
					// TextToSpeechStartupListener �������̽��� onFailedToInit() �ݹ��� �ʿ��� �����̴�.
					callback.onFailedToInit();
				}
			}
		});
	}

	private void setTextToSpeechSettings(final Locale locale) {

		Locale defaultOrPassedIn = locale; // Locale.getDefault() �Ǵ� Locale.KOREA�� ������ ����

		if (locale == null) { // �׳� null�� �Ѱ�ٸ� Locale.getDefault()�� ����
			defaultOrPassedIn = Locale.getDefault();
		}
		// TTS�� �Ѿ�� ������ �����ϴ��� üũ�ؾ� ��
		switch (tts.isLanguageAvailable(defaultOrPassedIn)) {
		case TextToSpeech.LANG_AVAILABLE: // TTS�� �غ�Ǿ���, �� �����Ѵ�.
		case TextToSpeech.LANG_COUNTRY_AVAILABLE: // TTS�� �غ�Ǿ���, ���� ������ �����Ѵ�.
		case TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE:// TTS�� �غ�Ǿ���, ���, ���� �׸��� �� ������(variant)�� �����Ѵ�.
			Log.d(TAG, "SUPPORTED");
			tts.setLanguage(locale);
			// TextToSpeechStartupListener �������̽��� onSuccessfulInit() �ݹ��� �ʿ��� �����̴�.
			callback.onSuccessfulInit(tts);
			break;
		case TextToSpeech.LANG_MISSING_DATA: // TTS�� �غ���� �ʾҴ�. ������ ��� �����͸� ������ ��밡���ϴ�.
			Log.d(TAG, "MISSING_DATA");
			// shared preference�� üũ�ؼ� ��� ������ ��ġ�� �ߺ������� ���� �ʴ´�.
			if (LanguageDataInstallBroadcastReceiver.isWaiting(context)) {
				Log.d(TAG, "waiting for data...");
				// TextToSpeechStartupListener �������̽��� onWaitingForLanguageData() �ݹ��� �ʿ��� �����̴�.
				callback.onWaitingForLanguageData();
			}
			else {
				Log.d(TAG, "require data...");
				// TextToSpeechStartupListener �������̽��� onRequireLanguageData() �ݹ��� �ʿ��� �����̴�.
				// ����ڰ� ��� �����͸� �ٿ�ε� �޵��� �ؾ� �Ѵ�.
				callback.onRequireLanguageData();
			}
			break;
		case TextToSpeech.LANG_NOT_SUPPORTED: // TTS�� �غ���� �ʾҴ�. ���� �� �������� �ʴ´�. ��ġ���� ������� ������ �� �ȴ�.
			Log.d(TAG, "NOT SUPPORTED");
			// TextToSpeechStartupListener �������̽��� onFailedToInit() �ݹ��� �ʿ��� �����̴�.
			callback.onFailedToInit();
			break;
		}
	}

//	// tts �ν��Ͻ��� �����ʸ� �޾��ִ� �޼ҵ�
//	public void setOnUtteranceCompleted(OnUtteranceCompletedListener whenDoneSpeaking) {
//		int result = tts.setOnUtteranceCompletedListener(whenDoneSpeaking);
//		if (result == TextToSpeech.ERROR) {
//			Log.e(TAG, "failed to add utterance listener");
//		}
//	}

	// ���� �ٿ�ε带 ��ٸ��� ���� ��ȭ���ڸ� �����ִ� helper method
	public void installLanguageData(Dialog ifAlreadyWaiting) {
		boolean alreadyDidThis = LanguageDataInstallBroadcastReceiver.isWaiting(context);
		if (alreadyDidThis) {
			// shared preference�� ��ٸ��ٰ� üũ �����ϱ� ���� ���� ��ȭ���ڸ� �����ش�.
			ifAlreadyWaiting.show();
		} else {
			// ���� shared preference�� ��ٸ��ٰ� üũ �ȉ����ϱ� ��� ������ ��ġ�� �����Ѵ�.
			installLanguageData();
		}
	}

	// ��� ������ ��ġ >_<~!!!
	public void installLanguageData() {
		// shared preference���ٰ� ��� ������ ��ġ�� ��ٸ��ٰ� üũ~!
		LanguageDataInstallBroadcastReceiver.setWaiting(context, true);

		Intent installIntent = new Intent();
		installIntent.setAction(TextToSpeech.Engine.ACTION_INSTALL_TTS_DATA);
		context.startActivity(installIntent);

		// ��ġ�� �Ϸ�Ǹ� "android.speech.tts.engine.TTS_DATA_INSTALLED" ����Ʈ��
		// �ѷ������̸�, LanguageDataInstallBroadcastReceiver�� �������̴� ~!!
	}
}