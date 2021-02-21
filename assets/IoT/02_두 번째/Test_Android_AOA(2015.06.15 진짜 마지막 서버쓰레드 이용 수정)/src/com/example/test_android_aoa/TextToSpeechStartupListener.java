package com.example.test_android_aoa;

import android.speech.tts.TextToSpeech;

//TTS �ν��Ͻ� �ʱ�ȭ �������� �Ͼ�� ��Ȳ���� �� �������̽��� �޼ҵ���� �Ҹ���,
//���� �� �������̽��� �����Ͽ� �����ϰ� ��ó���ָ� �ȴ�!!
public interface TextToSpeechStartupListener {
	// tts�� �ʱ�ȭ�ǰ� ����� �غ� �� ���
	// @param tts �ʱ�ȭ�� �ν��Ͻ�
	public void onSuccessfulInit(TextToSpeech tts);

	// ��� �����Ͱ� �ʿ��ϴ�. ��ġ�� �Ϸ��� TestToSpeechInitializer.installLanguageData()�� ȣ���ؾ� �Ѵ�.
	public void onRequireLanguageData();

	// ���� �̹� ��� �����͸� ��û�ϰ� ����� ��ٸ��� �ִ� ��
	public void onWaitingForLanguageData();

	// �ʱ�ȭ�� �����ߴ�.
	public void onFailedToInit();
}
