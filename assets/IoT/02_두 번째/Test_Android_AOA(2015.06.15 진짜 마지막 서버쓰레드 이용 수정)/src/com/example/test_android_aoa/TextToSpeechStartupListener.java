package com.example.test_android_aoa;

import android.speech.tts.TextToSpeech;

//TTS 인스턴스 초기화 과정에서 일어나는 상황에서 이 인터페이스의 메소드들이 불리며,
//그저 이 인터페이스를 구현하여 적절하게 대처해주면 된다!!
public interface TextToSpeechStartupListener {
	// tts가 초기화되고 사용할 준비가 된 경우
	// @param tts 초기화된 인스턴스
	public void onSuccessfulInit(TextToSpeech tts);

	// 언어 데이터가 필요하다. 설치를 하려면 TestToSpeechInitializer.installLanguageData()를 호출해야 한다.
	public void onRequireLanguageData();

	// 앱이 이미 언어 데이터를 요청하고 결과를 기다리고 있는 중
	public void onWaitingForLanguageData();

	// 초기화가 실패했다.
	public void onFailedToInit();
}
