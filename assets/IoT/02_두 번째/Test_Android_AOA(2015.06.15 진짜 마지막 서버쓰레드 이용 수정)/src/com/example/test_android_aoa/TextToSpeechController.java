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
	// 여기서 재생시 사용할 utteranceId이다. 재생이 완료되고 난후 onDone()이 호출 될 때,
	// onDone()에서 넘어온 매개변수인 utteranceId가 이것과 같으면 이걸로 재생한것이 끝난것이다.
	private static final String UTTERANCE_ID_LAST_SPOKEN = "lastSpoken";

	private TextToSpeechInitializer ttsInit; // TTS 초기화 관련 클래스
	private TextToSpeech tts; // TTS

	MainActivity hostActivity;
	Handler hostHandler;

	public void sendHandlerMessage(String msg, int what) {
		Message m = new Message();
		m.what = what;
		m.obj = msg;
		hostHandler.sendMessage(m);
	}

	// 생성자
	public TextToSpeechController(MainActivity hostActivity) {
		this.hostActivity = hostActivity;
		this.hostHandler = hostActivity.mHandler;

		init();
	}

	private void init() {
		tts = null;
		// TextToSpeechInitializer는 초기화 상황에 따라서
		// TextToSpeechStartupListener의 콜백 메소드를 불러준다 ~!!
		// onSuccessfulInit(TextToSpeech tts) : tts가 초기화되고 사용할 준비가 된 경우. 여기서 tts 매개변수는 초기화된 오브젝트이다.
		// onRequireLanguageData() : 언어 데이터가 필요하다. 설치를 위해 TextToSpeechInitalizer.installLanguageData()를 호출 해야한다
		// onWaitingForLanguageData() : 앱이 이미 언어 데이터 설치 요청하고 결과를 기다리고 있는 중
		// onFailedToInit() : 기기가 언어를 지원하지 않으므로 TTS 이용이 불가능하다. 
		ttsInit = new TextToSpeechInitializer(hostActivity, Locale.getDefault(), this);
	}

	// onSuccessfulInit(TextToSpeech tts) : tts가 초기화되고 사용할 준비가 된 경우. 여기서 tts 매개변수는 초기화된 오브젝트이다.
	@Override
	public void onSuccessfulInit(TextToSpeech tts) {
		// 매개변수인 tts는 초기화 성공한 오브젝트임
		this.tts = tts; // 초기화 성공한 TextToSpeech를 사용하자 ~
		setTtsListener(); // tts에 리스너를 달아주자
	}

	// TTS listener를 달아준다 ~. TTS는 글자를 읽고 난 후 달아준 listener의 onDone(String utteranceID)을 불러준다.
	// 이때 SDK 레벨이 15이상이면, UtteranceProgressListener를 이용해도 되지만
	// 이보다 낮은 API에서는 기존에 사용되던 setOnUtteranceCompletedListener를 이용해야한다.
	private void setTtsListener() {
		// SDK가 15 이상이므로 UtteranceProgressListener를 달아주자~
		if (Build.VERSION.SDK_INT >= 15){
			int listenerResult = // 리스너를 달아줄때도 실패 할 수 있다. 리턴되는 값으로 판단해야 한다 !!
					tts.setOnUtteranceProgressListener(new UtteranceProgressListener() {
						@Override
						public void onDone(String utteranceId) { // utteranceId == UTTERANCE_ID_LAST_SPOKEN, 사실 다른 ID는 이용하지 않음
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
				sendHandlerMessage("리스너를 달다가 실패했습니다 ..", MainActivity.DEBUG);
				release();
			}
		}
		else {
			int listenerResult =
					tts.setOnUtteranceCompletedListener(new OnUtteranceCompletedListener() {
						@Override
						public void onUtteranceCompleted(String utteranceId) { // utteranceId == UTTERANCE_ID_LAST_SPOKEN, 사실 다른 ID는 이용하지 않음
							TextToSpeechController.this.onDone(utteranceId);
						}
					});
			if (listenerResult != TextToSpeech.SUCCESS) {
				// "failed to add utterance completed listener"
				sendHandlerMessage("리스너를 달다가 실패했습니다 ..", MainActivity.DEBUG);
				release();
			}
		}
	}

	private void onDone(final String utteranceId) {
		if (utteranceId.equals(UTTERANCE_ID_LAST_SPOKEN)) {
			// "utterance completed: " + utteranceId (다 읽었다 매개변수로 넘어온 어터런스아이디는 아마도 UTTERANCE_ID_LAST_SPOKEN 이거겠지~
			sendHandlerMessage("읽어라는 글 다 읽었어", MainActivity.ON_DONE_FROM_TTS);
		}
		// runOnUiThread(new Runnable() {
		// 		@Override
		// 		public void run() {
		// 		}
		// });
	}

	// onFailedToInit() : 기기가 언어를 지원하지 않으므로 TTS 이용이 불가능하다.
	// 이건 언어 데이터 설치의 문제가 아니라 지원자체를 안하는 거다.
	@Override
	public void onFailedToInit() {
		DialogInterface.OnClickListener onClickOk = makeOnFailedToInitHandler();
		AlertDialog dlg =
				new AlertDialog.Builder(hostActivity).setTitle("Error")
				.setMessage("이 기기는 TTS를 지원하지 않습니다 !!")
				.setNeutralButton("Ok", onClickOk).create();
		dlg.show();
	}

	// onRequireLanguageData() : 언어 데이터가 필요하다. 설치를 위해 TextToSpeechInitalizer.installLanguageData()를 호출 해야한다
	@Override
	public void onRequireLanguageData() {
		DialogInterface.OnClickListener onClickOk = // 사용자가 OK를 누르면 TTS 언어 데이터 설치를 위해서 구글 플레이로 이동할 것이다.
				makeOnClickInstallDialogListener();
		DialogInterface.OnClickListener onClickCancel = // 사용자가 Cancel을 누르면 TTS 언어 데이터 설치를 거부한 것
				makeOnFailedToInitHandler();
		AlertDialog dlg =
				new AlertDialog.Builder(hostActivity).setTitle("Error")
				.setPositiveButton("Ok", onClickOk)
				.setNegativeButton("Cancel", onClickCancel)
				.setMessage(
						"언어 데이터 설치가 필요합니다.\n" +
						"설치하시겠습니까?").create();
		dlg.show();
	}

	// onWaitingForLanguageData() : 앱이 이미 언어 데이터 설치 요청하고 결과를 기다리고 있는 중
	@Override
	public void onWaitingForLanguageData() {
		DialogInterface.OnClickListener onClickWait = // 이미 언어 데이터 설치 진행중이니까 ~
				makeOnFailedToInitHandler();
		DialogInterface.OnClickListener onClickInstall = // 그래도 혹시 사용자가 구글플레이에서 잘못눌러서 설치를 못했을 수도 있으니까 ~
				makeOnClickInstallDialogListener();

		AlertDialog dlg =
				new AlertDialog.Builder(hostActivity).setTitle("Info")
				.setNegativeButton("Wait", onClickWait)
				.setPositiveButton("Retry", onClickInstall)
				.setMessage(
						"언어 데이터 설치가 끝나기까지 기다리는 중입니다.\n" +
						"다시 설치를 시도하시려면 Retry를 클릭하세요").create();
		dlg.show();
	}

	private DialogInterface.OnClickListener makeOnClickInstallDialogListener() {
		return new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				ttsInit.installLanguageData(); // TextToSpeechInitializer의 installLanguageData()호출로 언어 데이터 설치 ~!!!
			}
		};
	}

	private DialogInterface.OnClickListener makeOnFailedToInitHandler() {
		return new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				sendHandlerMessage("해당 기기에서 TTS를 사용할 수 없습니다.", MainActivity.NOT_SUPPORTED_LANGUAGE_FROM_TTS);
			}
		};
	}

	public void playScript(String script) {
		// utteranceID를 세팅해야지 다 읽고나서 onDone()이 불리고 어떤 글을 다 읽었는지 판단할 수 있다. 
		HashMap<String, String> lastSpokenWord = new HashMap<String, String>();
		lastSpokenWord.put(TextToSpeech.Engine.KEY_PARAM_UTTERANCE_ID, UTTERANCE_ID_LAST_SPOKEN);

		// earcon을 추가 시킨다 ~!!
		final String EARCON_NAME = "[tone]";
		tts.addEarcon(EARCON_NAME, "com.example.test_android_aoa", R.raw.tone); // TODO : 패키지 경로 수정 바람 "com.example.test_tts"

		// 미리 녹음된 음성을 추가 시킨다 ~!!
		final String CLOSING = "[Thank you]";
		tts.addSpeech(CLOSING, "com.example.test_android_aoa", R.raw.enjoytestapplication); // TODO : 패키지 경로 수정 바람 "com.example.test_tts"

		// 3번째 인자가 null일 경우 utteranceID를 주지 않았기 때문에 onDone()이 불리지 않는다 ~_~
		// TTS는 재생큐가 있으므로 TextToSpeech.QUEUE_ADD이 상수는 재생큐에 집어넣겠단 소리다
		tts.playEarcon(EARCON_NAME, TextToSpeech.QUEUE_ADD, null); // earcon이 재생됨
		tts.playSilence(300, TextToSpeech.QUEUE_ADD, null); // 1초 동안 조용히 하고 있어라
		tts.speak(script, TextToSpeech.QUEUE_ADD, null); // 진짜 읽음 ~!!
		tts.playSilence(300, TextToSpeech.QUEUE_ADD, lastSpokenWord); // 0.5초 동안 조용히 하고 있어라
		// tts.speak(CLOSING, TextToSpeech.QUEUE_ADD, lastSpokenWord); // 미리 녹음된 음성이 재생됨
	}

	public void release() { if (tts != null) tts.shutdown(); tts = null; }
	public boolean hasTts() { return (tts != null)? true : false; }
	public void speakingStop() { if(tts != null) tts.stop(); } // TTS 멈추기
}