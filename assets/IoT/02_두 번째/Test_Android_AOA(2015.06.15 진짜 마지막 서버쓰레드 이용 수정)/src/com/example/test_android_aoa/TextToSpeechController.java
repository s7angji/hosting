package com.example.test_android_aoa;

import java.util.HashMap;
import java.util.Locale;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Build;
import android.os.Handler;
import android.os.Message;
import android.speech.tts.TextToSpeech;
import android.speech.tts.TextToSpeech.OnUtteranceCompletedListener;
import android.speech.tts.UtteranceProgressListener;

public class TextToSpeechController implements TextToSpeechStartupListener{
	// ���⼭ ����� ����� utteranceId�̴�. ����� �Ϸ�ǰ� ���� onDone()�� ȣ�� �� ��,
	// onDone()���� �Ѿ�� �Ű������� utteranceId�� �̰Ͱ� ������ �̰ɷ� ����Ѱ��� �������̴�.
	private static final String UTTERANCE_ID_LAST_SPOKEN = "lastSpoken";

	private TextToSpeechInitializer ttsInit; // TTS �ʱ�ȭ ���� Ŭ����
	private TextToSpeech tts; // TTS

	MainActivity hostActivity;
	Handler hostHandler;

	public void sendHandlerMessage(String msg, int what) {
		Message m = new Message();
		m.what = what;
		m.obj = msg;
		hostHandler.sendMessage(m);
	}

	// ������
	public TextToSpeechController(MainActivity hostActivity) {
		this.hostActivity = hostActivity;
		this.hostHandler = hostActivity.mHandler;

		init();
	}

	private void init() {
		tts = null;
		// TextToSpeechInitializer�� �ʱ�ȭ ��Ȳ�� ����
		// TextToSpeechStartupListener�� �ݹ� �޼ҵ带 �ҷ��ش� ~!!
		// onSuccessfulInit(TextToSpeech tts) : tts�� �ʱ�ȭ�ǰ� ����� �غ� �� ���. ���⼭ tts �Ű������� �ʱ�ȭ�� ������Ʈ�̴�.
		// onRequireLanguageData() : ��� �����Ͱ� �ʿ��ϴ�. ��ġ�� ���� TextToSpeechInitalizer.installLanguageData()�� ȣ�� �ؾ��Ѵ�
		// onWaitingForLanguageData() : ���� �̹� ��� ������ ��ġ ��û�ϰ� ����� ��ٸ��� �ִ� ��
		// onFailedToInit() : ��Ⱑ �� �������� �����Ƿ� TTS �̿��� �Ұ����ϴ�. 
		ttsInit = new TextToSpeechInitializer(hostActivity, Locale.getDefault(), this);
	}

	// onSuccessfulInit(TextToSpeech tts) : tts�� �ʱ�ȭ�ǰ� ����� �غ� �� ���. ���⼭ tts �Ű������� �ʱ�ȭ�� ������Ʈ�̴�.
	@Override
	public void onSuccessfulInit(TextToSpeech tts) {
		// �Ű������� tts�� �ʱ�ȭ ������ ������Ʈ��
		this.tts = tts; // �ʱ�ȭ ������ TextToSpeech�� ������� ~
		setTtsListener(); // tts�� �����ʸ� �޾�����
	}

	// TTS listener�� �޾��ش� ~. TTS�� ���ڸ� �а� �� �� �޾��� listener�� onDone(String utteranceID)�� �ҷ��ش�.
	// �̶� SDK ������ 15�̻��̸�, UtteranceProgressListener�� �̿��ص� ������
	// �̺��� ���� API������ ������ ���Ǵ� setOnUtteranceCompletedListener�� �̿��ؾ��Ѵ�.
	private void setTtsListener() {
		// SDK�� 15 �̻��̹Ƿ� UtteranceProgressListener�� �޾�����~
		if (Build.VERSION.SDK_INT >= 15){
			int listenerResult = // �����ʸ� �޾��ٶ��� ���� �� �� �ִ�. ���ϵǴ� ������ �Ǵ��ؾ� �Ѵ� !!
					tts.setOnUtteranceProgressListener(new UtteranceProgressListener() {
						@Override
						public void onDone(String utteranceId) { // utteranceId == UTTERANCE_ID_LAST_SPOKEN, ��� �ٸ� ID�� �̿����� ����
							TextToSpeechController.this.onDone(utteranceId);
						}
						@Override
						public void onError(String utteranceId) {
							// "TTS error"
						}
						@Override
						public void onStart(String utteranceId) {
							// "TTS start"
						}
					});
			if (listenerResult != TextToSpeech.SUCCESS) {
				// "failed to add utterance progress listener"
				sendHandlerMessage("�����ʸ� �޴ٰ� �����߽��ϴ� ..", MainActivity.DEBUG);
				release();
			}
		}
		else {
			int listenerResult =
					tts.setOnUtteranceCompletedListener(new OnUtteranceCompletedListener() {
						@Override
						public void onUtteranceCompleted(String utteranceId) { // utteranceId == UTTERANCE_ID_LAST_SPOKEN, ��� �ٸ� ID�� �̿����� ����
							TextToSpeechController.this.onDone(utteranceId);
						}
					});
			if (listenerResult != TextToSpeech.SUCCESS) {
				// "failed to add utterance completed listener"
				sendHandlerMessage("�����ʸ� �޴ٰ� �����߽��ϴ� ..", MainActivity.DEBUG);
				release();
			}
		}
	}

	private void onDone(final String utteranceId) {
		if (utteranceId.equals(UTTERANCE_ID_LAST_SPOKEN)) {
			// "utterance completed: " + utteranceId (�� �о��� �Ű������� �Ѿ�� ���ͷ������̵�� �Ƹ��� UTTERANCE_ID_LAST_SPOKEN �̰Ű���~
			sendHandlerMessage("�о��� �� �� �о���", MainActivity.ON_DONE_FROM_TTS);
		}
		// runOnUiThread(new Runnable() {
		// 		@Override
		// 		public void run() {
		// 		}
		// });
	}

	// onFailedToInit() : ��Ⱑ �� �������� �����Ƿ� TTS �̿��� �Ұ����ϴ�.
	// �̰� ��� ������ ��ġ�� ������ �ƴ϶� ������ü�� ���ϴ� �Ŵ�.
	@Override
	public void onFailedToInit() {
		DialogInterface.OnClickListener onClickOk = makeOnFailedToInitHandler();
		AlertDialog dlg =
				new AlertDialog.Builder(hostActivity).setTitle("Error")
				.setMessage("�� ���� TTS�� �������� �ʽ��ϴ� !!")
				.setNeutralButton("Ok", onClickOk).create();
		dlg.show();
	}

	// onRequireLanguageData() : ��� �����Ͱ� �ʿ��ϴ�. ��ġ�� ���� TextToSpeechInitalizer.installLanguageData()�� ȣ�� �ؾ��Ѵ�
	@Override
	public void onRequireLanguageData() {
		DialogInterface.OnClickListener onClickOk = // ����ڰ� OK�� ������ TTS ��� ������ ��ġ�� ���ؼ� ���� �÷��̷� �̵��� ���̴�.
				makeOnClickInstallDialogListener();
		DialogInterface.OnClickListener onClickCancel = // ����ڰ� Cancel�� ������ TTS ��� ������ ��ġ�� �ź��� ��
				makeOnFailedToInitHandler();
		AlertDialog dlg =
				new AlertDialog.Builder(hostActivity).setTitle("Error")
				.setPositiveButton("Ok", onClickOk)
				.setNegativeButton("Cancel", onClickCancel)
				.setMessage(
						"��� ������ ��ġ�� �ʿ��մϴ�.\n" +
						"��ġ�Ͻðڽ��ϱ�?").create();
		dlg.show();
	}

	// onWaitingForLanguageData() : ���� �̹� ��� ������ ��ġ ��û�ϰ� ����� ��ٸ��� �ִ� ��
	@Override
	public void onWaitingForLanguageData() {
		DialogInterface.OnClickListener onClickWait = // �̹� ��� ������ ��ġ �������̴ϱ� ~
				makeOnFailedToInitHandler();
		DialogInterface.OnClickListener onClickInstall = // �׷��� Ȥ�� ����ڰ� �����÷��̿��� �߸������� ��ġ�� ������ ���� �����ϱ� ~
				makeOnClickInstallDialogListener();

		AlertDialog dlg =
				new AlertDialog.Builder(hostActivity).setTitle("Info")
				.setNegativeButton("Wait", onClickWait)
				.setPositiveButton("Retry", onClickInstall)
				.setMessage(
						"��� ������ ��ġ�� ��������� ��ٸ��� ���Դϴ�.\n" +
						"�ٽ� ��ġ�� �õ��Ͻ÷��� Retry�� Ŭ���ϼ���").create();
		dlg.show();
	}

	private DialogInterface.OnClickListener makeOnClickInstallDialogListener() {
		return new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				ttsInit.installLanguageData(); // TextToSpeechInitializer�� installLanguageData()ȣ��� ��� ������ ��ġ ~!!!
			}
		};
	}

	private DialogInterface.OnClickListener makeOnFailedToInitHandler() {
		return new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				sendHandlerMessage("�ش� ��⿡�� TTS�� ����� �� �����ϴ�.", MainActivity.NOT_SUPPORTED_LANGUAGE_FROM_TTS);
			}
		};
	}

	public void playScript(String script) {
		// utteranceID�� �����ؾ��� �� �а��� onDone()�� �Ҹ��� � ���� �� �о����� �Ǵ��� �� �ִ�. 
		HashMap<String, String> lastSpokenWord = new HashMap<String, String>();
		lastSpokenWord.put(TextToSpeech.Engine.KEY_PARAM_UTTERANCE_ID, UTTERANCE_ID_LAST_SPOKEN);

		// earcon�� �߰� ��Ų�� ~!!
		final String EARCON_NAME = "[tone]";
		tts.addEarcon(EARCON_NAME, "com.example.test_android_aoa", R.raw.tone); // TODO : ��Ű�� ��� ���� �ٶ� "com.example.test_tts"

		// �̸� ������ ������ �߰� ��Ų�� ~!!
		final String CLOSING = "[Thank you]";
		tts.addSpeech(CLOSING, "com.example.test_android_aoa", R.raw.enjoytestapplication); // TODO : ��Ű�� ��� ���� �ٶ� "com.example.test_tts"

		// 3��° ���ڰ� null�� ��� utteranceID�� ���� �ʾұ� ������ onDone()�� �Ҹ��� �ʴ´� ~_~
		// TTS�� ���ť�� �����Ƿ� TextToSpeech.QUEUE_ADD�� ����� ���ť�� ����ְڴ� �Ҹ���
		tts.playEarcon(EARCON_NAME, TextToSpeech.QUEUE_ADD, null); // earcon�� �����
		tts.playSilence(300, TextToSpeech.QUEUE_ADD, null); // 1�� ���� ������ �ϰ� �־��
		tts.speak(script, TextToSpeech.QUEUE_ADD, null); // ��¥ ���� ~!!
		tts.playSilence(300, TextToSpeech.QUEUE_ADD, lastSpokenWord); // 0.5�� ���� ������ �ϰ� �־��
		// tts.speak(CLOSING, TextToSpeech.QUEUE_ADD, lastSpokenWord); // �̸� ������ ������ �����
	}

	public void release() { if (tts != null) tts.shutdown(); tts = null; }
	public boolean hasTts() { return (tts != null)? true : false; }
	public void speakingStop() { if(tts != null) tts.stop(); } // TTS ���߱�
}